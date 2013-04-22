/*
 * ip_conntrack_pptp.c	- Version 3.0
 *
 * Connection tracking support for PPTP (Point to Point Tunneling Protocol).
 * PPTP is a a protocol for creating virtual private networks.
 * It is a specification defined by Microsoft and some vendors
 * working with Microsoft.  PPTP is built on top of a modified
 * version of the Internet Generic Routing Encapsulation Protocol.
 * GRE is defined in RFC 1701 and RFC 1702.  Documentation of
 * PPTP can be found in RFC 2637
 *
 * (C) 2000-2005 by Harald Welte <laforge@gnumonks.org>
 *
 * Development of this code funded by Astaro AG (http://www.astaro.com/)
 *
 * Limitations:
 * 	 - We blindly assume that control connections are always
 * 	   established in PNS->PAC direction.  This is a violation
 * 	   of RFFC2673
 * 	 - We can only support one single call within each session
 *
 * TODO:
 *	 - testing of incoming PPTP calls 
 *
 * Changes: 
 * 	2002-02-05 - Version 1.3
 * 	  - Call ip_conntrack_unexpect_related() from 
 * 	    pptp_timeout_related() to destroy expectations in case
 * 	    CALL_DISCONNECT_NOTIFY or tcp fin packet was seen
 * 	    (Philip Craig <philipc@snapgear.com>)
 * 	  - Add Version information at module loadtime
 * 	2002-02-10 - Version 1.6
 * 	  - move to C99 style initializers
 * 	  - remove second expectation if first arrives
 * 	2004-10-22 - Version 2.0
 * 	  - merge Mandrake's 2.6.x port with recent 2.6.x API changes
 * 	  - fix lots of linear skb assumptions from Mandrake's port
 * 	2005-06-10 - Version 2.1
 * 	  - use ip_conntrack_expect_free() instead of kfree() on the
 * 	    expect's (which are from the slab for quite some time)
 * 	2005-06-10 - Version 3.0
 * 	  - port helper to post-2.6.11 API changes,
 * 	    funded by Oxcoda NetBox Blue (http://www.netboxblue.com/)
 *
 */

#include <linux/config.h>
#include <linux/module.h>
#include <linux/netfilter.h>
#include <linux/ip.h>
#include <net/checksum.h>
#include <net/tcp.h>

#include <linux/netfilter_ipv4/lockhelp.h>
#include <linux/netfilter_ipv4/ip_conntrack.h>
#include <linux/netfilter_ipv4/ip_conntrack_core.h>
#include <linux/netfilter_ipv4/ip_conntrack_helper.h>
#include <linux/netfilter_ipv4/ip_conntrack_proto_gre.h>
#include <linux/netfilter_ipv4/ip_conntrack_pptp.h>

#define IP_CT_PPTP_VERSION "3.0"
#define ip_conntrack_expect_free(x) kfree(x)

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Harald Welte <laforge@gnumonks.org>");
MODULE_DESCRIPTION("Netfilter connection tracking helper module for PPTP");

DECLARE_LOCK(ip_pptp_lock);

int
(*ip_nat_pptp_hook_outbound)(struct sk_buff **pskb,
			  struct ip_conntrack *ct,
			  enum ip_conntrack_info ctinfo,
			  struct PptpControlHeader *ctlh,
			  union pptp_ctrl_union *pptpReq);

int
(*ip_nat_pptp_hook_inbound)(struct sk_buff **pskb,
			  struct ip_conntrack *ct,
			  enum ip_conntrack_info ctinfo,
			  struct PptpControlHeader *ctlh,
			  union pptp_ctrl_union *pptpReq);

int
(*ip_nat_pptp_hook_exp_gre)(struct ip_conntrack_expect *expect_orig,
			    struct ip_conntrack_expect *expect_reply);

void
(*ip_nat_pptp_hook_expectfn)(struct ip_conntrack *ct,
			     struct ip_conntrack_expect *exp);

#if 0
#include "ip_conntrack_pptp_priv.h"
#define DEBUGP(format, args...)	printk(KERN_DEBUG "%s:%s: " format, __FILE__, __FUNCTION__, ## args)
#else
#define DEBUGP(format, args...)
#endif

#define SECS *HZ
#define MINS * 60 SECS
#define HOURS * 60 MINS
#define DAYS * 24 HOURS

#define PPTP_GRE_TIMEOUT 		(10 MINS)
#define PPTP_GRE_STREAM_TIMEOUT 	(5 DAYS)

static void pptp_expectfn(struct ip_conntrack *ct,
			 struct ip_conntrack_expect *exp)
{
	DEBUGP("increasing timeouts\n");

	/* increase timeout of GRE data channel conntrack entry */
	ct->proto.gre.timeout = PPTP_GRE_TIMEOUT;
	ct->proto.gre.stream_timeout = PPTP_GRE_STREAM_TIMEOUT;

	/* Can you see how rusty this code is, compared with the pre-2.6.11
	 * one? That's what happened to my shiny newnat of 2002 ;( -HW */

	if (!ip_nat_pptp_hook_expectfn) {
		struct ip_conntrack_tuple inv_t;
		struct ip_conntrack_expect *exp_other;

		/* obviously this tuple inversion only works until you do NAT */
		invert_tuplepr(&inv_t, &exp->tuple);
		DEBUGP("trying to unexpect other dir: ");
		DUMP_TUPLE(&inv_t);
	
		exp_other = __ip_conntrack_exp_find(&inv_t);
		if (exp_other) {
			/* delete other expectation.  */
			DEBUGP("found\n");
			ip_conntrack_unexpect_related(exp_other);
		} else {
			DEBUGP("not found\n");
		}
	} else {
		/* we need more than simple inversion */
		ip_nat_pptp_hook_expectfn(ct, exp);
	}
}

static int timeout_ct_or_exp(const struct ip_conntrack_tuple *t)
{
	struct ip_conntrack_tuple_hash *h;
	struct ip_conntrack_expect *exp;

	DEBUGP("trying to timeout ct or exp for tuple ");
	DUMP_TUPLE(t);

	h = __ip_conntrack_find(t, NULL);
	if (h)  {
		struct ip_conntrack *sibling = tuplehash_to_ctrack(h);
		DEBUGP("setting timeout of conntrack %p to 0\n", sibling);
		sibling->proto.gre.timeout = 0;
		sibling->proto.gre.stream_timeout = 0;
		/* refresh_acct will not modify counters if skb == NULL */
		ip_ct_refresh_acct(sibling, 0, NULL, 0);
		return 1;
	} else {
		exp = __ip_conntrack_exp_find(t);
		if (exp) {
			DEBUGP("unexpect_related of expect %p\n", exp);
			ip_conntrack_unexpect_related(exp);
			return 1;
		}
	}

	return 0;
}


/* timeout GRE data connections */
static int pptp_timeout_related(struct ip_conntrack *ct)
{
	struct ip_conntrack_tuple t;
	int ret;

	/* Since ct->sibling_list has literally rusted away in 2.6.11, 
	 * we now need another way to find out about our sibling
	 * contrack and expects... -HW */

	/* try original (pns->pac) tuple */
	memcpy(&t, &ct->tuplehash[IP_CT_DIR_ORIGINAL].tuple, sizeof(t));
	t.dst.protonum = IPPROTO_GRE;
	t.src.u.gre.key = htons(ct->help.ct_pptp_info.pns_call_id);
	t.dst.u.gre.key = htons(ct->help.ct_pptp_info.pac_call_id);

	ret = timeout_ct_or_exp(&t);

	/* try reply (pac->pns) tuple */
	memcpy(&t, &ct->tuplehash[IP_CT_DIR_REPLY].tuple, sizeof(t));
	t.dst.protonum = IPPROTO_GRE;
	t.src.u.gre.key = htons(ct->help.ct_pptp_info.pac_call_id);
	t.dst.u.gre.key = htons(ct->help.ct_pptp_info.pns_call_id);

	ret += timeout_ct_or_exp(&t);

	return ret;
}

/* expect GRE connections (PNS->PAC and PAC->PNS direction) */
static inline int
exp_gre(struct ip_conntrack *master,
	u_int32_t seq,
	u_int16_t callid,
	u_int16_t peer_callid)
{
	struct ip_conntrack_tuple inv_tuple;
	struct ip_conntrack_tuple exp_tuples[] = {
		/* tuple in original direction, PNS->PAC */
		{ .src = { .ip = master->tuplehash[IP_CT_DIR_ORIGINAL].tuple.src.ip,
			   .u = { .gre = { .key = peer_callid } }
			 },
		  .dst = { .ip = master->tuplehash[IP_CT_DIR_ORIGINAL].tuple.dst.ip,
			   .u = { .gre = { .key = callid } },
			   .protonum = IPPROTO_GRE
			 },
		 },
		/* tuple in reply direction, PAC->PNS */
		{ .src = { .ip = master->tuplehash[IP_CT_DIR_REPLY].tuple.src.ip,
			   .u = { .gre = { .key = callid } }
			 },
		  .dst = { .ip = master->tuplehash[IP_CT_DIR_REPLY].tuple.dst.ip,
			   .u = { .gre = { .key = peer_callid } },
			   .protonum = IPPROTO_GRE
			 },
		 }
	};

	struct ip_conntrack_expect *exp_orig, *exp_reply;

	exp_orig = ip_conntrack_expect_alloc(master);
	if (exp_orig == NULL)
		return 1;

	exp_reply = ip_conntrack_expect_alloc(master);
	if (exp_reply == NULL) {
		ip_conntrack_expect_free(exp_orig);
		return 1;
	}

	memcpy(&exp_orig->tuple, &exp_tuples[0], sizeof(exp_orig->tuple));

	exp_orig->mask.src.ip = 0xffffffff;
	exp_orig->mask.src.u.all = 0;
	exp_orig->mask.dst.u.all = 0;
	exp_orig->mask.dst.u.gre.key = 0xffff;
	exp_orig->mask.dst.ip = 0xffffffff;
	exp_orig->mask.dst.protonum = 0xff;
		
	exp_orig->master = master;
	exp_orig->expectfn = pptp_expectfn;

	exp_orig->dir = IP_CT_DIR_ORIGINAL;

	/* both expectations are identical apart from tuple */
	memcpy(exp_reply, exp_orig, sizeof(*exp_reply));
	memcpy(&exp_reply->tuple, &exp_tuples[1], sizeof(exp_reply->tuple));

	exp_reply->dir = !exp_orig->dir;

	if (ip_nat_pptp_hook_exp_gre)
		return ip_nat_pptp_hook_exp_gre(exp_orig, exp_reply);
	else {

		DEBUGP("calling expect_related PNS->PAC");
		DUMP_TUPLE(&exp_orig->tuple);

		if (ip_conntrack_expect_related(exp_orig) != 0) {
			ip_conntrack_expect_free(exp_orig);
			ip_conntrack_expect_free(exp_reply);
			DEBUGP("cannot expect_related()\n");
			return 1;
		}

		DEBUGP("calling expect_related PAC->PNS");
		DUMP_TUPLE(&exp_reply->tuple);

		if (ip_conntrack_expect_related(exp_reply) != 0) {
			ip_conntrack_unexpect_related(exp_orig);
			ip_conntrack_expect_free(exp_reply);
			DEBUGP("cannot expect_related()\n");
			return 1;
		}

		/* Add GRE keymap entries */
		if (ip_ct_gre_keymap_add(master, &exp_reply->tuple, 0) != 0) {
			ip_conntrack_unexpect_related(exp_orig);
			ip_conntrack_unexpect_related(exp_reply);
			DEBUGP("cannot keymap_add() exp\n");
			return 1;
		}

		invert_tuplepr(&inv_tuple, &exp_reply->tuple);
		if (ip_ct_gre_keymap_add(master, &inv_tuple, 1) != 0) {
			ip_conntrack_unexpect_related(exp_orig);
			ip_conntrack_unexpect_related(exp_reply);
			ip_ct_gre_keymap_destroy(master);
			DEBUGP("cannot keymap_add() exp_inv\n");
			return 1;
		}
	
	}

	return 0;
}

static inline int 
pptp_inbound_pkt(struct sk_buff **pskb,
		 struct tcphdr *tcph,
		 unsigned int ctlhoff,
		 size_t datalen,
		 struct ip_conntrack *ct,
		 enum ip_conntrack_info ctinfo)
{
	struct PptpControlHeader _ctlh, *ctlh;
	unsigned int reqlen;
	union pptp_ctrl_union _pptpReq, *pptpReq;
	struct ip_ct_pptp_master *info = &ct->help.ct_pptp_info;
	u_int16_t msg, *cid, *pcid;
	u_int32_t seq;	

	ctlh = skb_header_pointer(*pskb, ctlhoff, sizeof(_ctlh), &_ctlh);
	if (unlikely(!ctlh)) {
		DEBUGP("error during skb_header_pointer\n");
		return NF_ACCEPT;
	}

	reqlen = datalen - sizeof(struct pptp_pkt_hdr) - sizeof(_ctlh);
	pptpReq = skb_header_pointer(*pskb, ctlhoff+sizeof(_ctlh),
				     reqlen, &_pptpReq);
	if (unlikely(!pptpReq)) {
		DEBUGP("error during skb_header_pointer\n");
		return NF_ACCEPT;
	}

	msg = ntohs(ctlh->messageType);
	DEBUGP("inbound control message %s\n", strMName[msg]);

	switch (msg) {
	case PPTP_START_SESSION_REPLY:
		if (reqlen < sizeof(_pptpReq.srep)) {
			DEBUGP("%s: short packet\n", strMName[msg]);
			break;
		}

		/* server confirms new control session */
		if (info->sstate < PPTP_SESSION_REQUESTED) {
			DEBUGP("%s without START_SESS_REQUEST\n",
				strMName[msg]);
			break;
		}
		if (pptpReq->srep.resultCode == PPTP_START_OK)
			info->sstate = PPTP_SESSION_CONFIRMED;
		else 
			info->sstate = PPTP_SESSION_ERROR;
		break;

	case PPTP_STOP_SESSION_REPLY:
		if (reqlen < sizeof(_pptpReq.strep)) {
			DEBUGP("%s: short packet\n", strMName[msg]);
			break;
		}

		/* server confirms end of control session */
		if (info->sstate > PPTP_SESSION_STOPREQ) {
			DEBUGP("%s without STOP_SESS_REQUEST\n",
				strMName[msg]);
			break;
		}
		if (pptpReq->strep.resultCode == PPTP_STOP_OK)
			info->sstate = PPTP_SESSION_NONE;
		else
			info->sstate = PPTP_SESSION_ERROR;
		break;

	case PPTP_OUT_CALL_REPLY:
		if (reqlen < sizeof(_pptpReq.ocack)) {
			DEBUGP("%s: short packet\n", strMName[msg]);
			break;
		}

		/* server accepted call, we now expect GRE frames */
		if (info->sstate != PPTP_SESSION_CONFIRMED) {
			DEBUGP("%s but no session\n", strMName[msg]);
			break;
		}
		if (info->cstate != PPTP_CALL_OUT_REQ &&
		    info->cstate != PPTP_CALL_OUT_CONF) {
			DEBUGP("%s without OUTCALL_REQ\n", strMName[msg]);
			break;
		}
		if (pptpReq->ocack.resultCode != PPTP_OUTCALL_CONNECT) {
			info->cstate = PPTP_CALL_NONE;
			break;
		}

		cid = &pptpReq->ocack.callID;
		pcid = &pptpReq->ocack.peersCallID;

		info->pac_call_id = ntohs(*cid);
		
		if (htons(info->pns_call_id) != *pcid) {
			DEBUGP("%s for unknown callid %u\n",
				strMName[msg], ntohs(*pcid));
			break;
		}

		DEBUGP("%s, CID=%X, PCID=%X\n", strMName[msg], 
			ntohs(*cid), ntohs(*pcid));
		
		info->cstate = PPTP_CALL_OUT_CONF;

		seq = ntohl(tcph->seq) + sizeof(struct pptp_pkt_hdr)
				       + sizeof(struct PptpControlHeader)
				       + ((void *)pcid - (void *)pptpReq);
			
		if (exp_gre(ct, seq, *cid, *pcid) != 0)
			printk("ip_conntrack_pptp: error during exp_gre\n");
		break;

	case PPTP_IN_CALL_REQUEST:
		if (reqlen < sizeof(_pptpReq.icack)) {
			DEBUGP("%s: short packet\n", strMName[msg]);
			break;
		}

		/* server tells us about incoming call request */
		if (info->sstate != PPTP_SESSION_CONFIRMED) {
			DEBUGP("%s but no session\n", strMName[msg]);
			break;
		}
		pcid = &pptpReq->icack.peersCallID;
		DEBUGP("%s, PCID=%X\n", strMName[msg], ntohs(*pcid));
		info->cstate = PPTP_CALL_IN_REQ;
		info->pac_call_id = ntohs(*pcid);
		break;

	case PPTP_IN_CALL_CONNECT:
		if (reqlen < sizeof(_pptpReq.iccon)) {
			DEBUGP("%s: short packet\n", strMName[msg]);
			break;
		}

		/* server tells us about incoming call established */
		if (info->sstate != PPTP_SESSION_CONFIRMED) {
			DEBUGP("%s but no session\n", strMName[msg]);
			break;
		}
		if (info->sstate != PPTP_CALL_IN_REP
		    && info->sstate != PPTP_CALL_IN_CONF) {
			DEBUGP("%s but never sent IN_CALL_REPLY\n",
				strMName[msg]);
			break;
		}

		pcid = &pptpReq->iccon.peersCallID;
		cid = &info->pac_call_id;

		if (info->pns_call_id != ntohs(*pcid)) {
			DEBUGP("%s for unknown CallID %u\n", 
				strMName[msg], ntohs(*cid));
			break;
		}

		DEBUGP("%s, PCID=%X\n", strMName[msg], ntohs(*pcid));
		info->cstate = PPTP_CALL_IN_CONF;

		/* we expect a GRE connection from PAC to PNS */
		seq = ntohl(tcph->seq) + sizeof(struct pptp_pkt_hdr)
				       + sizeof(struct PptpControlHeader)
				       + ((void *)pcid - (void *)pptpReq);
			
		if (exp_gre(ct, seq, *cid, *pcid) != 0)
			printk("ip_conntrack_pptp: error during exp_gre\n");

		break;

	case PPTP_CALL_DISCONNECT_NOTIFY:
		if (reqlen < sizeof(_pptpReq.disc)) {
			DEBUGP("%s: short packet\n", strMName[msg]);
			break;
		}

		/* server confirms disconnect */
		cid = &pptpReq->disc.callID;
		DEBUGP("%s, CID=%X\n", strMName[msg], ntohs(*cid));
		info->cstate = PPTP_CALL_NONE;

		/* untrack this call id, unexpect GRE packets */
		pptp_timeout_related(ct);
		break;

	case PPTP_WAN_ERROR_NOTIFY:
		break;

	case PPTP_ECHO_REQUEST:
	case PPTP_ECHO_REPLY:
		/* I don't have to explain these ;) */
		break;
	default:
		DEBUGP("invalid %s (TY=%d)\n", (msg <= PPTP_MSG_MAX)
			? strMName[msg]:strMName[0], msg);
		break;
	}


	if (ip_nat_pptp_hook_inbound)
		return ip_nat_pptp_hook_inbound(pskb, ct, ctinfo, ctlh,
						pptpReq);

	return NF_ACCEPT;

}

static inline int
pptp_outbound_pkt(struct sk_buff **pskb,
		  struct tcphdr *tcph,
		  unsigned int ctlhoff,
		  size_t datalen,
		  struct ip_conntrack *ct,
		  enum ip_conntrack_info ctinfo)
{
	struct PptpControlHeader _ctlh, *ctlh;
	unsigned int reqlen;
	union pptp_ctrl_union _pptpReq, *pptpReq;
	struct ip_ct_pptp_master *info = &ct->help.ct_pptp_info;
	u_int16_t msg, *cid, *pcid;

	ctlh = skb_header_pointer(*pskb, ctlhoff, sizeof(_ctlh), &_ctlh);
	if (!ctlh)
		return NF_ACCEPT;
	
	reqlen = datalen - sizeof(struct pptp_pkt_hdr) - sizeof(_ctlh);
	pptpReq = skb_header_pointer(*pskb, ctlhoff+sizeof(_ctlh), reqlen, 
				     &_pptpReq);
	if (!pptpReq)
		return NF_ACCEPT;

	msg = ntohs(ctlh->messageType);
	DEBUGP("outbound control message %s\n", strMName[msg]);

	switch (msg) {
	case PPTP_START_SESSION_REQUEST:
		/* client requests for new control session */
		if (info->sstate != PPTP_SESSION_NONE) {
			DEBUGP("%s but we already have one",
				strMName[msg]);
		}
		info->sstate = PPTP_SESSION_REQUESTED;
		break;
	case PPTP_STOP_SESSION_REQUEST:
		/* client requests end of control session */
		info->sstate = PPTP_SESSION_STOPREQ;
		break;

	case PPTP_OUT_CALL_REQUEST:
		if (reqlen < sizeof(_pptpReq.ocreq)) {
			DEBUGP("%s: short packet\n", strMName[msg]);
			/* FIXME: break; */
		}

		/* client initiating connection to server */
		if (info->sstate != PPTP_SESSION_CONFIRMED) {
			DEBUGP("%s but no session\n",
				strMName[msg]);
			break;
		}
		info->cstate = PPTP_CALL_OUT_REQ;
		/* track PNS call id */
		cid = &pptpReq->ocreq.callID;
		DEBUGP("%s, CID=%X\n", strMName[msg], ntohs(*cid));
		info->pns_call_id = ntohs(*cid);
		break;
	case PPTP_IN_CALL_REPLY:
		if (reqlen < sizeof(_pptpReq.icack)) {
			DEBUGP("%s: short packet\n", strMName[msg]);
			break;
		}

		/* client answers incoming call */
		if (info->cstate != PPTP_CALL_IN_REQ
		    && info->cstate != PPTP_CALL_IN_REP) {
			DEBUGP("%s without incall_req\n", 
				strMName[msg]);
			break;
		}
		if (pptpReq->icack.resultCode != PPTP_INCALL_ACCEPT) {
			info->cstate = PPTP_CALL_NONE;
			break;
		}
		pcid = &pptpReq->icack.peersCallID;
		if (info->pac_call_id != ntohs(*pcid)) {
			DEBUGP("%s for unknown call %u\n", 
				strMName[msg], ntohs(*pcid));
			break;
		}
		DEBUGP("%s, CID=%X\n", strMName[msg], ntohs(*pcid));
		/* part two of the three-way handshake */
		info->cstate = PPTP_CALL_IN_REP;
		info->pns_call_id = ntohs(pptpReq->icack.callID);
		break;

	case PPTP_CALL_CLEAR_REQUEST:
		/* client requests hangup of call */
		if (info->sstate != PPTP_SESSION_CONFIRMED) {
			DEBUGP("CLEAR_CALL but no session\n");
			break;
		}
		/* FUTURE: iterate over all calls and check if
		 * call ID is valid.  We don't do this without newnat,
		 * because we only know about last call */
		info->cstate = PPTP_CALL_CLEAR_REQ;
		break;
	case PPTP_SET_LINK_INFO:
		break;
	case PPTP_ECHO_REQUEST:
	case PPTP_ECHO_REPLY:
		/* I don't have to explain these ;) */
		break;
	default:
		DEBUGP("invalid %s (TY=%d)\n", (msg <= PPTP_MSG_MAX)? 
			strMName[msg]:strMName[0], msg);
		/* unknown: no need to create GRE masq table entry */
		break;
	}
	
	if (ip_nat_pptp_hook_outbound)
		return ip_nat_pptp_hook_outbound(pskb, ct, ctinfo, ctlh,
						 pptpReq);

	return NF_ACCEPT;
}


/* track caller id inside control connection, call expect_related */
static int 
conntrack_pptp_help(struct sk_buff **pskb,
		    struct ip_conntrack *ct, enum ip_conntrack_info ctinfo)

{
	struct pptp_pkt_hdr _pptph, *pptph;
	
	struct tcphdr _tcph, *tcph;
	u_int32_t tcplen = (*pskb)->len - (*pskb)->nh.iph->ihl * 4;
	u_int32_t datalen;
	void *datalimit;
	int dir = CTINFO2DIR(ctinfo);
	struct ip_ct_pptp_master *info = &ct->help.ct_pptp_info;
	unsigned int nexthdr_off;

	int oldsstate, oldcstate;
	int ret;

	/* don't do any tracking before tcp handshake complete */
	if (ctinfo != IP_CT_ESTABLISHED 
	    && ctinfo != IP_CT_ESTABLISHED+IP_CT_IS_REPLY) {
		DEBUGP("ctinfo = %u, skipping\n", ctinfo);
		return NF_ACCEPT;
	}
	
	nexthdr_off = (*pskb)->nh.iph->ihl*4;
	tcph = skb_header_pointer(*pskb, (*pskb)->nh.iph->ihl*4, sizeof(_tcph),
				  &_tcph);
	if (!tcph)
		return NF_ACCEPT;

	/* not a complete TCP header? */
	if (tcplen < sizeof(struct tcphdr) || tcplen < tcph->doff * 4) {
		DEBUGP("tcplen = %u\n", tcplen);
		return NF_ACCEPT;
	}


 	datalen = tcplen - tcph->doff * 4;

	/* checksum invalid? */
	if (tcp_v4_check(tcph, tcplen, (*pskb)->nh.iph->saddr,
			 (*pskb)->nh.iph->daddr,
			 csum_partial((char *) tcph, tcplen, 0))) {
		DEBUGP(" bad csum\n");
		/* W2K PPTP server sends TCP packets with wrong checksum :(( */
		//return NF_ACCEPT;
	}

	if (tcph->fin || tcph->rst) {
		DEBUGP("RST/FIN received, timeouting GRE\n");
		/* can't do this after real newnat */
		info->cstate = PPTP_CALL_NONE;

		/* untrack this call id, unexpect GRE packets */
		pptp_timeout_related(ct);
	}

	nexthdr_off += tcph->doff*4;
	pptph = skb_header_pointer(*pskb, (*pskb)->nh.iph->ihl*4 + tcph->doff*4,
				   sizeof(_pptph), &_pptph);
	if (!pptph) {
		DEBUGP("no full PPTP header, can't track\n");
		return NF_ACCEPT;
	}

	datalimit = (void *) pptph + datalen;

	/* if it's not a control message we can't do anything with it */
	if (ntohs(pptph->packetType) != PPTP_PACKET_CONTROL ||
	    ntohl(pptph->magicCookie) != PPTP_MAGIC_COOKIE) {
		DEBUGP("not a control packet\n");
		return NF_ACCEPT;
	}

	oldsstate = info->sstate;
	oldcstate = info->cstate;

	LOCK_BH(&ip_pptp_lock);

	nexthdr_off += sizeof(_pptph);
	/* FIXME: We just blindly assume that the control connection is always
	 * established from PNS->PAC.  However, RFC makes no guarantee */
	if (dir == IP_CT_DIR_ORIGINAL)
		/* client -> server (PNS -> PAC) */
		ret = pptp_outbound_pkt(pskb, tcph, nexthdr_off, datalen, ct,
					ctinfo);
	else
		/* server -> client (PAC -> PNS) */
		ret = pptp_inbound_pkt(pskb, tcph, nexthdr_off, datalen, ct,
				       ctinfo);
	DEBUGP("sstate: %d->%d, cstate: %d->%d\n",
		oldsstate, info->sstate, oldcstate, info->cstate);
	UNLOCK_BH(&ip_pptp_lock);

	return ret;
}

/* control protocol helper */
static struct ip_conntrack_helper pptp = { 
	.list = { NULL, NULL },
	.name = "pptp", 
	.me = THIS_MODULE,
	.max_expected = 2,
	.timeout = 5 * 60,
	.tuple = { .src = { .ip = 0, 
		 	    .u = { .tcp = { .port =  
				    __constant_htons(PPTP_CONTROL_PORT) } } 
			  }, 
		   .dst = { .ip = 0, 
			    .u = { .all = 0 },
			    .protonum = IPPROTO_TCP
			  } 
		 },
	.mask = { .src = { .ip = 0, 
			   .u = { .tcp = { .port = 0xffff } } 
			 }, 
		  .dst = { .ip = 0, 
			   .u = { .all = 0 },
			   .protonum = 0xff 
		 	 } 
		},
	.help = conntrack_pptp_help
};

/* ip_conntrack_pptp initialization */
static int __init init(void)
{
	int retcode;

	DEBUGP(" registering helper\n");
	if ((retcode = ip_conntrack_helper_register(&pptp))) {
		printk(KERN_ERR "Unable to register conntrack application "
				"helper for pptp: %d\n", retcode);
		return -EIO;
	}

	printk("ip_conntrack_pptp version %s loaded\n", IP_CT_PPTP_VERSION);
	return 0;
}

static void __exit fini(void)
{
	ip_conntrack_helper_unregister(&pptp);
	printk("ip_conntrack_pptp version %s unloaded\n", IP_CT_PPTP_VERSION);
}

module_init(init);
module_exit(fini);

EXPORT_SYMBOL(ip_pptp_lock);
EXPORT_SYMBOL(ip_nat_pptp_hook_outbound);
EXPORT_SYMBOL(ip_nat_pptp_hook_inbound);
EXPORT_SYMBOL(ip_nat_pptp_hook_exp_gre);
EXPORT_SYMBOL(ip_nat_pptp_hook_expectfn);
