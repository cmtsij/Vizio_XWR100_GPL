/* SIP extension for IP connection tracking.
 *
 * (C) 2005 by Christian Hentschel <chentschel@arnet.com.ar>
 * based on RR's ip_conntrack_ftp.c and other modules.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/config.h>
#include <linux/module.h>
#include <linux/netfilter.h>
#include <linux/ip.h>
#include <linux/ctype.h>
#include <linux/in.h>
#include <linux/udp.h>
#include <net/checksum.h>
#include <net/udp.h>

#include <linux/netfilter_ipv4/ip_conntrack_helper.h>
#include <linux/netfilter_ipv4/ip_conntrack_sip.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Christian Hentschel <chentschel@arnet.com.ar>");
MODULE_DESCRIPTION("SIP connection tracking helper");

static DEFINE_SPINLOCK(sipbf_lock);


#define MAX_PORTS	8
static int ports[MAX_PORTS];
static int ports_c;
module_param_array(ports, int, &ports_c, 0400);
MODULE_PARM_DESC(ports, " port numbers of sip servers");

static unsigned int sip_timeout = SIP_TIMEOUT;

module_param(sip_timeout, int, 0600);
MODULE_PARM_DESC(sip_timeout, "timeout for the master sip session");

unsigned int (*ip_nat_sip_hook)(struct sk_buff **pskb, 
				enum ip_conntrack_info ctinfo,
				struct ip_conntrack *ct,
				const char **dptr);
EXPORT_SYMBOL_GPL(ip_nat_sip_hook);
				
unsigned int (*ip_nat_sdp_hook)(struct sk_buff **pskb, 
				enum ip_conntrack_info ctinfo,
				struct ip_conntrack_expect *exp,
				const char *dptr);
EXPORT_SYMBOL_GPL(ip_nat_sdp_hook);

int ct_sip_get_info(const char *dptr, size_t dlen, 
				unsigned int *matchoff, 
				unsigned int *matchlen, 
				struct sip_header_nfo *hnfo);
EXPORT_SYMBOL(ct_sip_get_info);

#if 0
#define DEBUGP printk
#else
#define DEBUGP(format, args...)
#endif

static int digits_len(const char *dptr, const char *limit, int *shift);
static int epaddr_len(const char *dptr, const char *limit, int *shift);
static int skp_digits_len(const char *dptr, const char *limit, int *shift);
static int skp_epaddr_len(const char *dptr, const char *limit, int *shift);

struct sip_header_nfo ct_sip_hdrs[] = {
	{ 	/* Via header */
		"Via:",		sizeof("Via:") - 1,
		"\r\nv:",	sizeof("\r\nv:") - 1, /* rfc3261 "\r\n" */
		"UDP ", 	sizeof("UDP ") - 1,
		epaddr_len
	},
	{ 	/* Contact header */
		"Contact:",	sizeof("Contact:") - 1,
		"\r\nm:",	sizeof("\r\nm:") - 1,
		"sip:",		sizeof("sip:") - 1,
		skp_epaddr_len
	},
	{ 	/* Content length header */
		"Content-Length:", sizeof("Content-Length:") - 1,
		"\r\nl:",	sizeof("\r\nl:") - 1,
		":",		sizeof(":") - 1, 
		skp_digits_len
	},
	{	/* SDP media info */
		"\nm=",		sizeof("\nm=") - 1,	
		"\rm=",		sizeof("\rm=") - 1,
		"audio ",	sizeof("audio ") - 1,
		digits_len
	},
	{ 	/* SDP owner address*/	
		"\no=",		sizeof("\no=") - 1, 
		"\ro=",		sizeof("\ro=") - 1,
		"IN IP4 ",	sizeof("IN IP4 ") - 1,
		epaddr_len
	},
	{ 	/* SDP connection info */
		"\nc=",		sizeof("\nc=") - 1, 
		"\rc=",		sizeof("\rc=") - 1,
		"IN IP4 ",	sizeof("IN IP4 ") - 1,
		epaddr_len
	},
	{ 	/* Requests headers */
		"sip:",		sizeof("sip:") - 1,
		"sip:",		sizeof("sip:") - 1, /* yes, i know.. ;) */
		"@", 		sizeof("@") - 1, 
		epaddr_len
	},
	{ 	/* SDP version header */
		"\nv=",		sizeof("\nv=") - 1,
		"\rv=",		sizeof("\rv=") - 1,
		"=", 		sizeof("=") - 1, 
		digits_len
	}
};
EXPORT_SYMBOL(ct_sip_hdrs);


static int digits_len(const char *dptr, const char *limit, int *shift)
{
	int len = 0;	
	while (dptr <= limit && isdigit(*dptr)) {
		dptr++;
		len++;
	}
	return len;
} 

/* get digits lenght, skiping blank spaces. */
static int skp_digits_len(const char *dptr, const char *limit, int *shift)
{
	for (; dptr <= limit && *dptr == ' '; dptr++)
		(*shift)++;
		
	return digits_len(dptr, limit, shift);
}

/* Simple ipaddr parser.. */
static int parse_ipaddr(const char *cp,	const char **endp, 
			uint32_t *ipaddr, const char *limit)
{
	unsigned long int val;
	int i, digit = 0;
	
	for (i = 0, *ipaddr = 0; cp <= limit && i < 4; i++) {
		digit = 0;
		if (!isdigit(*cp))
			break;
		
		val = simple_strtoul(cp, (char **)&cp, 10);
		if (val > 0xFF)
			return -1;
	
		((uint8_t *)ipaddr)[i] = val;	
		digit = 1;
	
		if (*cp != '.')
			break;
		cp++;
	}
	if (!digit)
		return -1;
	
	if (endp)
		*endp = cp;

	return 0;
}

/* skip ip address. returns it lenght. */
static int epaddr_len(const char *dptr, const char *limit, int *shift)
{
	const char *aux = dptr;
	uint32_t ip;
	
	if (parse_ipaddr(dptr, &dptr, &ip, limit) < 0) {
		DEBUGP("ip: %s parse failed.!\n", dptr);
		return 0;
	}

	/* Port number */
	if (*dptr == ':') {
		dptr++;
		dptr += digits_len(dptr, limit, shift);
	}
	return dptr - aux;
}

/* get address lenght, skiping user info. */
static int skp_epaddr_len(const char *dptr, const char *limit, int *shift)
{
	for (; dptr <= limit && *dptr != '@'; dptr++)
		(*shift)++;	
	
	if (*dptr == '@') {
		dptr++;
		(*shift)++;
		return epaddr_len(dptr, limit, shift);
	}	
	return  0;
}

/* Returns 0 if not found, -1 error parsing. */
int ct_sip_get_info(const char *dptr, size_t dlen, 
		unsigned int *matchoff, 
		unsigned int *matchlen,
		struct sip_header_nfo *hnfo)
{
	const char *limit, *aux, *k = dptr;
	int shift = 0;
	
	limit = dptr + (dlen - hnfo->lnlen);

	while (dptr <= limit) {
		if ((strncmp(dptr, hnfo->lname, hnfo->lnlen) != 0) &&
			(strncmp(dptr, hnfo->sname, hnfo->snlen) != 0))
		{
			dptr++;
			continue;
		}
		aux = ct_sip_search(hnfo->ln_str, dptr, hnfo->ln_strlen, 
						ct_sip_lnlen(dptr, limit));
		if (!aux) {
			DEBUGP("'%s' not found in '%s'.\n", hnfo->ln_str, hnfo->lname);
			return -1;
		}
		aux += hnfo->ln_strlen;
		
		*matchlen = hnfo->match_len(aux, limit, &shift);
		if (!*matchlen)
			return -1;

		*matchoff = (aux - k) + shift; 
		
		DEBUGP("%s match succeeded! - len: %u\n", hnfo->lname, *matchlen);
		return 1;
	}
	DEBUGP("%s header not found.\n", hnfo->lname);
	return 0;
}

static int set_expected_rtp(struct sk_buff **pskb, 
			struct ip_conntrack *ct,
			enum ip_conntrack_info ctinfo, 
			uint32_t ipaddr, uint16_t port,
			const char *dptr)
{
	struct ip_conntrack_expect *exp;
	int ret;
	
	exp = ip_conntrack_expect_alloc(ct);
	if (exp == NULL)
		return NF_DROP;

	exp->tuple = ((struct ip_conntrack_tuple)
		{ { ct->tuplehash[IP_CT_DIR_REPLY].tuple.src.ip, { 0 } },
		  { ipaddr, { .udp = { htons(port) } }, IPPROTO_UDP }});
		  
	exp->mask = ((struct ip_conntrack_tuple) 
		{ { 0xFFFFFFFF, { 0 } },
		  { 0xFFFFFFFF, { .udp = { 0xFFFF } }, 0xFF }});
	
	exp->expectfn = NULL;
	
	if (ip_nat_sdp_hook)
		ret = ip_nat_sdp_hook(pskb, ctinfo, exp, dptr);
	else {
		if (ip_conntrack_expect_related(exp) != 0)
			ret = NF_DROP;
		else
			ret = NF_ACCEPT;
	}
	ip_conntrack_expect_put(exp);

	return ret;
}

static int sip_help(struct sk_buff **pskb,
		struct ip_conntrack *ct,
		enum ip_conntrack_info ctinfo)
{
	unsigned int dataoff, datalen;
	const char *dptr;
	int ret = NF_ACCEPT;
	int matchoff, matchlen;
	uint32_t ipaddr;
	uint16_t port;
	
	/* No Data ? */ 
	dataoff = (*pskb)->nh.iph->ihl*4 + sizeof(struct udphdr);
	if (dataoff >= (*pskb)->len) {
		DEBUGP("skb->len = %u\n", (*pskb)->len);
		return NF_ACCEPT;
        }
        
	ip_ct_refresh(ct, *pskb, sip_timeout * HZ);
		
	spin_lock_bh(&sipbf_lock);
	
	if ((dataoff + (*pskb)->len - dataoff) <= skb_headlen(*pskb))
		dptr = (*pskb)->data + dataoff;
	else {
		DEBUGP("Copy of skbuff not supported yet.\n");
		goto out;
	}
	
	if (ip_nat_sip_hook) {
		if (!ip_nat_sip_hook(pskb, ctinfo, ct, &dptr)) {
			ret = NF_DROP;
			goto out;
		}
	}
	
	if ((ctinfo) >= IP_CT_IS_REPLY)
		goto out;

	/* After this point NAT, could have mangled skb, so 
	   we need to recalculate payload lenght. */
	datalen = (*pskb)->len - dataoff;

	if (datalen < (sizeof("SIP/2.0 200") - 1))
		goto out;
	
	/* RTP info only in some SDP pkts */
	if (memcmp(dptr, "INVITE", sizeof("INVITE") - 1) != 0 && 
	    memcmp(dptr, "SIP/2.0 200", sizeof("SIP/2.0 200") - 1) != 0) {
		goto out;
	}
	/* Get ip and port address from SDP packet. */
	if (ct_sip_get_info(dptr, datalen, &matchoff, &matchlen, 
	    &ct_sip_hdrs[POS_CONECTION]) > 0) {

		/* We'll drop only if there are parse problems. */
		if (parse_ipaddr(dptr + matchoff, NULL, &ipaddr, 
		    dptr + datalen) < 0) {
			ret = NF_DROP;
			goto out;
		}
		if (ct_sip_get_info(dptr, datalen, &matchoff, &matchlen, 
		    &ct_sip_hdrs[POS_MEDIA]) > 0) {

			port = simple_strtoul(dptr + matchoff, NULL, 10);
			if (port < 1024) {
				ret = NF_DROP;
				goto out;
			}
			ret = set_expected_rtp(pskb, ct, ctinfo,
						ipaddr, port, dptr);
		}
	}
out:	spin_unlock_bh(&sipbf_lock);
	return ret;
}

static struct ip_conntrack_helper sip[MAX_PORTS];
static char sip_names[MAX_PORTS][10];

static void fini(void)
{
	int i = 0;
	for (; i < ports_c; i++) {
		DEBUGP("unregistering helper for port %d\n", ports[i]);
		ip_conntrack_helper_unregister(&sip[i]);
	} 
}

static int __init init(void)
{
	int i, ret;
	char *tmpname;
  u_int32_t sip_port = SIP_PORT << 16;

	if (ports_c == 0)
		ports[ports_c++] = sip_port;

	for (i = 0; i < ports_c; i++) {
		/* Create helper structure */
		memset(&sip[i], 0, sizeof(struct ip_conntrack_helper));

		sip[i].tuple.dst.protonum = IPPROTO_UDP;
		sip[i].tuple.src.u.udp.port = htons(ports[i]);
		sip[i].mask.src.u.udp.port = 0xFFFF;
		sip[i].mask.dst.protonum = 0xFF;
		sip[i].max_expected = 1;
		sip[i].timeout = 3 * 60; /* 3 minutes */
		sip[i].me = THIS_MODULE;
		sip[i].help = sip_help;

		tmpname = &sip_names[i][0];
		if (ports[i] == sip_port)
			sprintf(tmpname, "sip");
		else
			sprintf(tmpname, "sip-%d", i);
		sip[i].name = tmpname;

		DEBUGP("port #%d: %d\n", i, ports[i]);

		ret=ip_conntrack_helper_register(&sip[i]);
		if (ret) {
			printk("ERROR registering helper for port %d\n",
				ports[i]);
			fini();
			return(ret);
		}
	}
	return(0);
}

module_init(init);
module_exit(fini);