#include <linux/types.h> 
#include <linux/ip.h> 
#include <linux/tcp.h> 
#include <linux/timer.h> 
#include <linux/module.h> 
#include <linux/netfilter.h> 
#include <linux/netdevice.h> 
#include <linux/if.h> 
#include <linux/inetdevice.h> 
#include <net/protocol.h> 
#include <net/checksum.h> 
#include <linux/in.h>
 
#include <linux/netfilter_ipv4.h> 
#include <linux/netfilter_ipv4/ip_tables.h> 
#include <linux/netfilter_ipv4/ip_conntrack.h> 
#include <linux/netfilter_ipv4/ip_conntrack_core.h> 
#include <linux/netfilter_ipv4/ip_conntrack_tuple.h>
#include <linux/netfilter_ipv4/lockhelp.h> 
#include <linux/netfilter_ipv4/ip_nat_rule.h> 
#include <linux/netfilter_ipv4/ipt_AUTOFW.h> 
 
/* This rwlock protects the main hash table, protocol/helper/expected 
 *    registrations, conntrack timers*/ 
 
#define ASSERT_READ_LOCK(x) MUST_BE_READ_LOCKED(&ip_conntrack_lock) 
#define ASSERT_WRITE_LOCK(x) MUST_BE_WRITE_LOCKED(&ip_conntrack_lock) 
 
#include <linux/netfilter_ipv4/listhelp.h> 
 
#if 0
#define DEBUGP printk 
#else 
#define DEBUGP(format, args...) 
#endif 
 
struct ipt_autofw { 
        struct list_head list;          /* Trigger list */ 
        struct timer_list timeout;      /* Timer for list destroying */ 
        u_int32_t srcip;                /* Outgoing source address */ 
        u_int32_t dstip;                /* Outgoing destination address */ 
        u_int16_t mproto;               /* Trigger protocol */ 
        u_int16_t rproto;               /* Related protocol */ 
        struct ipt_autofw_ports ports; /* Trigger and related ports */ 
        u_int8_t reply;                 /* Confirm a reply connection */ 
}; 
 
LIST_HEAD(autofw_list); 
 
static void autofw_refresh(struct ipt_autofw *trig, unsigned long extra_jiffies) 
{ 
    DEBUGP("%s: \n", __FUNCTION__); 
    IP_NF_ASSERT(trig); 
    WRITE_LOCK(&ip_conntrack_lock); 
 
    /* Need del_timer for race avoidance (may already be dying). */ 
    if (del_timer(&trig->timeout)) { 
        trig->timeout.expires = jiffies + extra_jiffies; 
        add_timer(&trig->timeout); 
    } 
 
    WRITE_UNLOCK(&ip_conntrack_lock); 
} 
 
static void __del_autofw(struct ipt_autofw *trig) 
{ 
    DEBUGP("%s: \n", __FUNCTION__); 
    IP_NF_ASSERT(trig); 
    MUST_BE_WRITE_LOCKED(&ip_conntrack_lock); 
 
     /* delete from 'autofw_list' */ 
    list_del(&trig->list); 
    kfree(trig); 
} 
 
static void autofw_timeout(unsigned long ul_trig) 
{ 
    struct ipt_autofw *trig= (void *) ul_trig; 
 
    DEBUGP("autofw list %p timed out\n", trig); 
    WRITE_LOCK(&ip_conntrack_lock); 
    __del_autofw(trig); 
    WRITE_UNLOCK(&ip_conntrack_lock); 
} 
 
static unsigned int 
add_new_autofw(struct ipt_autofw *trig) 
{ 
    struct ipt_autofw *new; 
    DEBUGP("############# %s ############ %d\n", __FUNCTION__, __LINE__); 
 
    DEBUGP("!!!!!!!!!!!! %s !!!!!!!!!!!\n", __FUNCTION__); 
    WRITE_LOCK(&ip_conntrack_lock); 
    new = (struct ipt_autofw *) 
        kmalloc(sizeof(struct ipt_autofw), GFP_ATOMIC); 
 
    DEBUGP("############# %s ############ %d\n", __FUNCTION__, __LINE__); 
    if (!new) { 
    DEBUGP("############# %s ############ %d\n", __FUNCTION__, __LINE__); 
        WRITE_UNLOCK(&ip_conntrack_lock); 
        DEBUGP("%s: OOM allocating autofw list\n", __FUNCTION__); 
        return -ENOMEM; 
    } 
    DEBUGP("############# %s ############ %d\n", __FUNCTION__, __LINE__); 
 
    memset(new, 0, sizeof(*trig)); 
    INIT_LIST_HEAD(&new->list); 
    memcpy(new, trig, sizeof(*trig)); 
    DEBUGP("############# %s ############ %d\n", __FUNCTION__, __LINE__); 

    /* add to global table of autofw */ 
    list_prepend(&autofw_list, &new->list); 
    /* add and start timer if required */ 
    init_timer(&new->timeout); 
    new->timeout.data = (unsigned long)new; 
    new->timeout.function = autofw_timeout; 
    new->timeout.expires = jiffies + (AUTOFW_TIMEOUT * HZ); 
    add_timer(&new->timeout); 
    DEBUGP("############# %s ############ %d\n", __FUNCTION__, __LINE__); 
             
    WRITE_UNLOCK(&ip_conntrack_lock); 
    DEBUGP("############# %s ############ %d\n", __FUNCTION__, __LINE__); 

    return 0; 
} 
 
static inline int autofw_out_matched(const struct ipt_autofw *i, 
        const u_int16_t proto, const u_int16_t dport) 
{ 
    /* DEBUGP("%s: i=%p, proto= %d, dport=%d.\n", __FUNCTION__, i, proto, dport); 
    DEBUGP("%s: Got one, mproto= %d, mport[0..1]=%d, %d.\n", __FUNCTION__,  
            i->mproto, i->ports.mport[0], i->ports.mport[1]); */ 

    DEBUGP("############# %s ############ %d\n", __FUNCTION__, __LINE__); 
    return ((i->mproto == proto) && (i->ports.mport[0] <= dport)  
            && (i->ports.mport[1] >= dport)); 
} 
 
static unsigned int 
autofw_out(struct sk_buff **pskb, 
               unsigned int hooknum, 
                const struct net_device *in, 
                const struct net_device *out, 
                const void *targinfo) 
{ 
    const struct ipt_autofw_info *info = targinfo; 
    struct ipt_autofw trig, *found; 
    const struct iphdr *iph = (*pskb)->nh.iph; 
    struct tcphdr *tcph = (void *)iph + iph->ihl*4;     /* Might be TCP, UDP */  
    //struct ip_conntrack *ct;
//		enum ip_conntrack_info ctinfo;
		
// 		ct = ip_conntrack_get(*pskb, &ctinfo);
// 		ip_ct_refresh(ct, *pskb, 1 * HZ);
    DEBUGP("############# %s ############\n", __FUNCTION__); 
    /* Check if the autofw range has already existed in 'autofw_list'. */  
    /* cdrouter issue : Gemtek Modify change the iph->protocol to info->proto (use the target protocol replace the ip protocol 20090206 */
    //found = LIST_FIND(&autofw_list, autofw_out_matched, 
    //        struct ipt_autofw *, iph->protocol, ntohs(tcph->dest)); 
    found = LIST_FIND(&autofw_list, autofw_out_matched, 
            struct ipt_autofw *, info->proto, ntohs(tcph->dest));
 
    if (found) { 
    DEBUGP("############# %s ############ %d\n", __FUNCTION__, __LINE__); 
        /* Yeah, it exists. We need to update(delay) the destroying timer. */ 
        autofw_refresh(found, AUTOFW_TIMEOUT * HZ); 
        /* In order to allow multiple hosts use the same port range, we update 
           the 'saddr' after previous autofw has a reply connection. */ 
        if (found->reply) 
            found->srcip = iph->saddr; 
    } 
    else {
    DEBUGP("############# %s ############ %d\n", __FUNCTION__, __LINE__); 
        /* Create new autofw */ 
        memset(&trig, 0, sizeof(trig)); 
        trig.srcip = iph->saddr; 
        trig.mproto = iph->protocol; 
        trig.rproto = info->proto; 
        memcpy(&trig.ports, &info->ports, sizeof(struct ipt_autofw_ports)); 
        add_new_autofw(&trig); /* Add the new 'trig' to list 'autofw_list'. */ 
    } 
    DEBUGP("############# %s ############ %d\n", __FUNCTION__, __LINE__); 
 
    return IPT_CONTINUE;        /* We don't block any packet. */ 
} 
 
static inline int autofw_in_matched(const struct ipt_autofw *i, 
       const u_int16_t proto, const u_int16_t dport) 
{ 
    /* DEBUGP("%s: i=%p, proto= %d, dport=%d.\n", __FUNCTION__, i, proto, dport); 
    DEBUGP("%s: Got one, rproto= %d, rport[0..1]=%d, %d.\n", __FUNCTION__,  
            i->rproto, i->ports.rport[0], i->ports.rport[1]); */ 
    u_int16_t rproto = i->rproto; 
 
    if (!rproto) 
        rproto = proto; 
 
    return ((rproto == proto) && (i->ports.rport[0] <= dport)  
            && (i->ports.rport[1] >= dport)); 
} 
 
static unsigned int 
autofw_in(struct sk_buff **pskb, 
                unsigned int hooknum, 
                const struct net_device *in, 
                const struct net_device *out, 
                const void *targinfo) 
{ 
    struct ipt_autofw *found; 
    const struct iphdr *iph = (*pskb)->nh.iph; 
    struct tcphdr *tcph = (void *)iph + iph->ihl*4;     /* Might be TCP, UDP */ 
    /* Check if the autofw-ed range has already existed in 'autofw_list'. */ 
    found = LIST_FIND(&autofw_list, autofw_in_matched, 
            struct ipt_autofw *, iph->protocol, ntohs(tcph->dest)); 
    if (found) { 
    		DEBUGP("############# %s ############ %d\n", __FUNCTION__, __LINE__); 
        /* Yeah, it exists. We need to update(delay) the destroying timer. */ 
        autofw_refresh(found, AUTOFW_TIMEOUT * HZ); 
        return NF_ACCEPT;       /* Accept it, or the imcoming packet could be  
                                   dropped in the FORWARD chain */ 
    } 
  
    return IPT_CONTINUE;        /* Our job is the interception. */ 
} 
 
static unsigned int 
autofw_dnat(struct sk_buff **pskb, 
                unsigned int hooknum, 
                const struct net_device *in, 
                const struct net_device *out, 
                const void *targinfo) 
{ 
    struct ipt_autofw *found; 
    const struct iphdr *iph = (*pskb)->nh.iph; 
    struct tcphdr *tcph = (void *)iph + iph->ihl*4;     /* Might be TCP, UDP */ 
    struct ip_conntrack *ct; 
    enum ip_conntrack_info ctinfo; 
    struct ip_nat_multi_range_compat newrange; 
 
    IP_NF_ASSERT(hooknum == NF_IP_PRE_ROUTING); 
    /* Check if the autofw-ed range has already existed in 'autofw_list'. */ 
    found = LIST_FIND(&autofw_list, autofw_in_matched, 
            struct ipt_autofw *, iph->protocol, ntohs(tcph->dest)); 
 
    if (!found || !found->srcip) 
        return IPT_CONTINUE;    /* We don't block any packet. */ 
 
    DEBUGP("############# %s ############ %d\n", __FUNCTION__, __LINE__); 
    found->reply = 1;   /* Confirm there has been a reply connection. */ 
    ct = ip_conntrack_get(*pskb, &ctinfo); 
    IP_NF_ASSERT(ct && (ctinfo == IP_CT_NEW)); 
 
    DEBUGP("%s: got ", __FUNCTION__); 
    DUMP_TUPLE(&ct->tuplehash[IP_CT_DIR_ORIGINAL].tuple); 
 
    /* Alter the destination of imcoming packet. */ 
    newrange = ((struct ip_nat_multi_range_compat) 
            { 1, { { IP_NAT_RANGE_MAP_IPS, 
                     found->srcip, found->srcip, 
                     { 0 }, { 0 } 
                   } } }); 
    DEBUGP("############# %s ############ %d\n", __FUNCTION__, __LINE__); 
 
    /* Hand modified range to generic setup. */ 
    return ip_nat_setup_info(ct, &newrange.range[0], hooknum); 
} 
 
static unsigned int 
autofw_target(struct sk_buff **pskb,
								const struct net_device *in,
								const struct net_device *out,
								unsigned int hooknum,
								const void *targinfo,
								void *userinfo) 
{ 
  const struct ipt_autofw_info *info = targinfo; 
  const struct iphdr *iph = (*pskb)->nh.iph;
	//struct ip_conntrack *ct;
	//enum ip_conntrack_info ctinfo;
	
	//ct = ip_conntrack_get(*pskb, &ctinfo);
	//ip_ct_refresh_acct(ct, ctinfo, *pskb, 1 * HZ); 
 
    DEBUGP("%s: type = %s\n", __FUNCTION__,  
            (info->type == IPT_AUTOFW_DNAT) ? "dnat" : 
            (info->type == IPT_AUTOFW_IN) ? "in" : "out"); 
 
    /* The Port-autofw only supports TCP and UDP. */ 
    if ((iph->protocol != IPPROTO_TCP) && (iph->protocol != IPPROTO_UDP)) 
        return IPT_CONTINUE; 
 
    DEBUGP("############# %s ############ %d\n", __FUNCTION__, __LINE__); 
    if (info->type == IPT_AUTOFW_OUT) 
        return autofw_out(pskb, hooknum, in, out, targinfo); 
    else if (info->type == IPT_AUTOFW_IN) 
        return autofw_in(pskb, hooknum, in, out, targinfo); 
    else if (info->type == IPT_AUTOFW_DNAT) 
        return autofw_dnat(pskb, hooknum, in, out, targinfo); 
 
    DEBUGP("############# %s ############ %d\n", __FUNCTION__, __LINE__); 
    return IPT_CONTINUE; 
} 
static int 
autofw_check(const char *tablename,
	       const struct ipt_entry *e,
	       void *targinfo,
	       unsigned int targinfosize,
	       unsigned int hook_mask) 
{ 
    DEBUGP("############# %s ############ %d\n", __FUNCTION__, __LINE__); 
        const struct ipt_autofw_info *info = targinfo; 
        struct list_head *cur_item, *tmp_item; 
 
    DEBUGP("############# %s ############ %d\n", __FUNCTION__, __LINE__); 
        if ((strcmp(tablename, "mangle") == 0)) { 
    DEBUGP("############# %s ############ %d\n", __FUNCTION__, __LINE__); 
                DEBUGP("autofw_check: bad table `%s'.\n", tablename); 
                return 0; 
        } 
    DEBUGP("############# %s ############ %d\n", __FUNCTION__, __LINE__); 
        if (hook_mask & ~((1 << NF_IP_PRE_ROUTING) | (1 << NF_IP_FORWARD))) { 
    DEBUGP("############# %s ############ %d\n", __FUNCTION__, __LINE__); 
                DEBUGP("autofw_check: bad hooks %x.\n", hook_mask); 
                return 0; 
        }
    DEBUGP("############# %s ############ %d %d\n", __FUNCTION__, __LINE__, info->type); 
        if (info->proto) { 
    DEBUGP("############# %s ############ %d\n", __FUNCTION__, __LINE__); 
            if (info->proto != IPPROTO_TCP && info->proto != IPPROTO_UDP) { 
    DEBUGP("############# %s ############ %d\n", __FUNCTION__, __LINE__); 
                DEBUGP("autofw_check: bad proto %d.\n", info->proto); 
                return 0; 
            } 
        } 
    DEBUGP("############# %s ############ %d\n", __FUNCTION__, __LINE__); 
        if (info->type == IPT_AUTOFW_OUT) { 
    DEBUGP("############# %s ############ %d\n", __FUNCTION__, __LINE__); 
            if (!info->ports.mport[0] || !info->ports.rport[0]) { 
    DEBUGP("############# %s ############ %d\n", __FUNCTION__, __LINE__); 
                DEBUGP("autofw_check: Try 'iptbles -j AUTOFW -h' for help.\n"); 
                return 0; 
            } 
        } 
    DEBUGP("############# %s ############ %d\n", __FUNCTION__, __LINE__); 
 
        /* Empty the 'autofw_list' */ 
        list_for_each_safe(cur_item, tmp_item, &autofw_list) { 
    DEBUGP("############# %s ############ %d\n", __FUNCTION__, __LINE__); 
            struct ipt_autofw *trig = (void *)cur_item; 
 
            DEBUGP("%s: list_for_each_safe(): %p.\n", __FUNCTION__, trig); 
            del_timer(&trig->timeout); 
            __del_autofw(trig); 
        } 
 
    DEBUGP("############# %s ############ %d\n", __FUNCTION__, __LINE__); 
        return 1; 
} 
 
 
static struct ipt_target autofw_reg = {  
        .name           = "AUTOFW", 
        .target         = &autofw_target,  
        .checkentry     = &autofw_check, 
        .me             = THIS_MODULE, 
}; 
 
static int __init init(void) 
{ 
	printk("ipt_AUTOFW loading\n");
        return ipt_register_target(&autofw_reg); 
} 
 
static void __exit fini(void) 
{ 
        ipt_unregister_target(&autofw_reg); 
	printk("ipt_AUTOFW unloaded\n");
} 
 
module_init(init); 
module_exit(fini); 
