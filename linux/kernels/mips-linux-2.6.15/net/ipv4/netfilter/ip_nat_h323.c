/*
 * H.323 'brute force' extension for NAT alteration.
 * Jozsef Kadlecsik <kadlec@blackhole.kfki.hu>
 * (c) 2005 Max Kellermann <max@duempel.org>
 *
 * Based on ip_masq_h323.c for 2.2 kernels from CoRiTel, Sofia project.
 * (http://www.coritel.it/projects/sofia/nat.html)
 * Uses Sampsa Ranta's excellent idea on using expectfn to 'bind'
 * the unregistered helpers to the conntrack entries.
 */


#include <linux/module.h>
#include <linux/netfilter.h>
#include <linux/ip.h>
#include <net/checksum.h>
#include <net/tcp.h>

#include <linux/netfilter_ipv4/ip_nat.h>
#include <linux/netfilter_ipv4/ip_nat_helper.h>
#include <linux/netfilter_ipv4/ip_nat_rule.h>
#include <linux/netfilter_ipv4/ip_conntrack_tuple.h>
#include <linux/netfilter_ipv4/ip_conntrack_helper.h>
#include <linux/netfilter_ipv4/ip_conntrack_h323.h>

MODULE_AUTHOR("Jozsef Kadlecsik <kadlec@blackhole.kfki.hu>");
MODULE_DESCRIPTION("H.323 'brute force' connection tracking module");
MODULE_LICENSE("GPL");

struct module *ip_nat_h323 = THIS_MODULE;

#if 0
#define DEBUGP printk
#else
#define DEBUGP(format, args...)
#endif

static void ip_nat_h225_signal(struct sk_buff **pskb,
			       struct ip_conntrack *ct,
			       enum ip_conntrack_info ctinfo,
			       unsigned int offset,
			       int dir,
			       int orig_dir)
{
	struct {
		u_int32_t ip;
		u_int16_t port;
	} __attribute__ ((__packed__)) newdata;

	/* Change address inside packet to match way we're mapping
	   this connection. */
	if (dir == IP_CT_DIR_ORIGINAL) {
		newdata.ip = ct->tuplehash[!orig_dir].tuple.dst.ip;
		newdata.port = ct->tuplehash[!orig_dir].tuple.dst.u.tcp.port;
	} else {
		newdata.ip = ct->tuplehash[!orig_dir].tuple.src.ip;
		newdata.port = ct->tuplehash[!orig_dir].tuple.src.u.tcp.port;
	}

	/* Modify the packet */
	ip_nat_mangle_tcp_packet(pskb, ct, ctinfo,
				 offset,
				 sizeof(newdata),
				 (const char*)&newdata, sizeof(newdata));
}

/**
 * This conntrack expect function replaces ip_conntrack_h245_expect()
 * which was set by ip_conntrack_h323.c. It calls both
 * ip_nat_follow_master() and ip_conntrack_h245_expect().
 */
static void ip_nat_h245_expect(struct ip_conntrack *new,
			       struct ip_conntrack_expect *this)
{
	ip_nat_follow_master(new, this);
	ip_conntrack_h245_expect(new, this);
}

static int ip_nat_h225(struct sk_buff **pskb,
		       enum ip_conntrack_info ctinfo,
		       unsigned int offset,
		       struct ip_conntrack_expect *exp)
{
	u_int16_t port;
	struct {
		u_int32_t ip;
		u_int16_t port;
	} __attribute__ ((__packed__)) newdata;
	int dir = CTINFO2DIR(ctinfo);
	struct ip_conntrack *ct = exp->master;
	int ret;

	/* Connection will come from wherever this packet goes, hence !dir */
	newdata.ip = ct->tuplehash[!dir].tuple.dst.ip;
	exp->saved_proto.tcp.port = exp->tuple.dst.u.tcp.port;
	exp->dir = !dir;

	/* When you see the packet, we need to NAT it the same as the
	 * this one. */
	BUG_ON(exp->expectfn != ip_conntrack_h245_expect);
	exp->expectfn = ip_nat_h245_expect;

	/* Try to get same port: if not, try to change it. */
	for (port = ntohs(exp->saved_proto.tcp.port); port != 0; port++) {
		exp->tuple.dst.u.tcp.port = htons(port);
		if (ip_conntrack_expect_related(exp) == 0)
			break;
	}

	if (port == 0) {
		ip_conntrack_expect_put(exp);
		return NF_DROP;
	}

	newdata.port = htons(port);

	/* now mangle packet */
	ret = ip_nat_mangle_tcp_packet(pskb, ct, ctinfo,
				       offset,
				       sizeof(newdata),
				       (const char*)&newdata, sizeof(newdata));
	if (!ret)
		return NF_DROP;

	return NF_ACCEPT;
}

static int ip_nat_h245(struct sk_buff **pskb,
		       enum ip_conntrack_info ctinfo,
		       unsigned int offset,
		       struct ip_conntrack_expect *exp)
{
	u_int16_t port;
	struct {
		u_int32_t ip;
		u_int16_t port;
	} __attribute__ ((__packed__)) newdata;
	int dir = CTINFO2DIR(ctinfo);
	struct ip_conntrack *ct = exp->master;
	int ret;

	/* Connection will come from wherever this packet goes, hence !dir */
	newdata.ip = ct->tuplehash[!dir].tuple.dst.ip;
	exp->saved_proto.tcp.port = exp->tuple.dst.u.tcp.port;
	exp->dir = !dir;

	/* When you see the packet, we need to NAT it the same as the
	 * this one. */
	exp->expectfn = ip_nat_follow_master;

	/* Try to get same port: if not, try to change it. */
	for (port = ntohs(exp->saved_proto.tcp.port); port != 0; port++) {
		exp->tuple.dst.u.tcp.port = htons(port);
		if (ip_conntrack_expect_related(exp) == 0)
			break;
	}

	if (port == 0) {
		ip_conntrack_expect_put(exp);
		return NF_DROP;
	}

	newdata.port = htons(port);

	/* now mangle packet */
	ret = ip_nat_mangle_tcp_packet(pskb, ct, ctinfo,
				       offset,
				       sizeof(newdata),
				       (const char*)&newdata, sizeof(newdata));
	if (!ret)
		return NF_DROP;

	return NF_ACCEPT;
}

static int __init init(void)
{
	BUG_ON(ip_nat_h225_hook != NULL);
	BUG_ON(ip_nat_h245_hook != NULL);

	ip_nat_h225_hook = ip_nat_h225;
	ip_nat_h225_signal_hook = ip_nat_h225_signal;
	ip_nat_h245_hook = ip_nat_h245;
	DEBUGP("ip_nat_h323 : loading\n");

	return 0;
}

static void __exit fini(void)
{
	ip_nat_h225_hook = NULL;
	ip_nat_h225_signal_hook = NULL;
	ip_nat_h245_hook = NULL;
}

module_init(init);
module_exit(fini);
