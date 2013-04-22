/*
 *********************************************************
 *   Copyright 2008, Gemtek  Inc.  All Rights Reserved *
 *********************************************************
 #v1.0 Added the 404 packet filter module. Marmot_Chen 20081015
 	Known issue:
 							1.Since I don't modify the total length of IP header, if the original packet size was less than the new packet size 
 								it will fail to transmit this packet.
 								iph->tot_len = htons(new_length);
								iph->frag_off = 0;
								iph->check = 0;
								iph->check = ip_fast_csum((unsigned char *) iph,iph->ihl);
*/
/*
 * This is a module which is used for monitor the 404 packet from internet.
 */
#include <linux/module.h>
#include <linux/skbuff.h>
#include <linux/netfilter_ipv4.h>
#include <net/checksum.h>
#include <net/icmp.h>
#include <net/ip.h>
#include <net/tcp.h>
#include <net/udp.h>
#include <linux/spinlock.h>
#include <linux/netfilter_ipv4/ip_tables.h>
#include <net/route.h>
#include <linux/netfilter_ipv4/ipt_404FT.h>

/*	For putting processes to sleep and waking them up	*/
#include <linux/sched.h>
unsigned char buffer_404[24]={0x48, 0x54, 0x54, 0x50, 0x2f, 0x31, 0x2e, 0x31, 0x20, 0x34, 
															0x30, 0x34, 0x20, 0x4e, 0x6f, 0x74, 0x20, 0x46, 0x6f, 0x75, 
															0x6e, 0x64, 0x0d, 0x0a};

/*unsigned char *buffer_200={0x48, 0x54, 0x54, 0x50, 0x2f, 0x31, 0x2e, 0x30, 0x20, 0x32, 0x30, 0x30, 0x20, 0x4f, 0x6b, 0x0d, 
									0x0a, 0x53, 0x65, 0x72, 0x76, 0x65, 0x72, 0x3a, 0x20, 0x68, 0x74, 0x74, 0x70, 0x64, 0x0d, 0x0a, 
									0x44, 0x61, 0x74, 0x65, 0x3a, 0x20, 0x4d, 0x6f, 0x6e, 0x2c, 0x20, 0x31, 0x33, 0x20, 0x4f, 0x63, 
									0x74, 0x20, 0x32, 0x30, 0x30, 0x38, 0x20, 0x30, 0x32, 0x3a, 0x31, 0x30, 0x3a, 0x35, 0x35, 0x20, 
									0x47, 0x4d, 0x54, 0x0d, 0x0a, 0x43, 0x61, 0x63, 0x68, 0x65, 0x2d, 0x43, 0x6f, 0x6e, 0x74, 0x72, 
									0x6f, 0x6c, 0x3a, 0x20, 0x6e, 0x6f, 0x2d, 0x63, 0x61, 0x63, 0x68, 0x65, 0x0d, 0x0a, 0x50, 0x72, 
									0x61, 0x67, 0x6d, 0x61, 0x3a, 0x20, 0x6e, 0x6f, 0x2d, 0x63, 0x61, 0x63, 0x68, 0x65, 0x0d, 0x0a, 
									0x45, 0x78, 0x70, 0x69, 0x72, 0x65, 0x73, 0x3a, 0x20, 0x30, 0x0d, 0x0a, 0x43, 0x6f, 0x6e, 0x74, 
									0x65, 0x6e, 0x74, 0x2d, 0x54, 0x79, 0x70, 0x65, 0x3a, 0x20, 0x74, 0x65, 0x78, 0x74, 0x2f, 0x68, 
									0x74, 0x6d, 0x6c, 0x0d, 0x0a, 0x43, 0x6f, 0x6e, 0x6e, 0x65, 0x63, 0x74, 0x69, 0x6f, 0x6e, 0x3a, 
									0x20, 0x63, 0x6c, 0x6f, 0x73, 0x65, 0x0d, 0x0a, 0x0d, 0x0a, 0x3c, 0x48, 0x54, 0x4d, 0x4c, 0x3e, 
									0x3c, 0x48, 0x45, 0x41, 0x44, 0x3e, 0x3c, 0x54, 0x49, 0x54, 0x4c, 0x45, 0x3e, 0x49, 0x6e, 0x64, 
									0x65, 0x78, 0x20, 0x70, 0x61, 0x67, 0x65, 0x3c, 0x2f, 0x54, 0x49, 0x54, 0x4c, 0x45, 0x3e, 0x0a, 
									0x3c, 0x53, 0x43, 0x52, 0x49, 0x50, 0x54, 0x20, 0x6c, 0x61, 0x6e, 0x67, 0x75, 0x61, 0x67, 0x65, 
									0x3d, 0x4a, 0x61, 0x76, 0x61, 0x53, 0x63, 0x72, 0x69, 0x70, 0x74, 0x3e, 0x0a, 0x66, 0x75, 0x6e, 
									0x63, 0x74, 0x69, 0x6f, 0x6e, 0x20, 0x69, 0x6e, 0x69, 0x74, 0x28, 0x29, 0x0a, 0x7b, 0x0a, 0x76, 
									0x61, 0x72, 0x20, 
									0x6a, 0x6f, 0x62, 0x20, 0x3d, 0x20, 0x6c, 0x6f, 0x63, 0x61, 0x74, 0x69, 0x6f, 0x6e, 0x2e, 0x68, 
									0x6f, 0x73, 0x74, 0x20, 0x2b, 0x20, 0x6c, 0x6f, 0x63, 0x61, 0x74, 0x69, 0x6f, 0x6e, 0x2e, 0x70, 
									0x61, 0x74, 0x68, 0x6e, 0x61, 0x6d, 0x65, 0x3b, 0x0a, 0x76, 0x61, 0x72, 0x20, 0x6a, 0x6f, 0x62, 
									0x20, 0x3d, 0x20, 0x6a, 0x6f, 0x62, 0x2e, 0x72, 0x65, 0x70, 0x6c, 0x61, 0x63, 0x65, 0x28, 0x2f, 
									0x20, 0x2f, 0x67, 0x2c, 0x20, 0x22, 0x2b, 0x22, 0x29, 0x3b, 0x0a, 0x77, 0x69, 0x6e, 0x64, 0x6f, 
									0x77, 0x2e, 0x6c, 0x6f, 0x63, 0x61, 0x74, 0x69, 0x6f, 0x6e, 0x20, 0x3d, 0x20, 0x22, 0x68, 0x74, 
									0x74, 0x70, 0x3a, 0x2f, 0x2f, 0x77, 0x65, 0x62, 0x73, 0x65, 0x61, 0x72, 0x63, 0x68, 0x2e, 0x6c, 
									0x69, 0x6e, 0x6b, 0x73, 0x79, 0x73, 0x2e, 0x63, 0x6f, 0x6d, 0x2f, 0x3f, 0x65, 0x67, 0x3d, 0x22, 
									0x20, 0x2b, 0x20, 0x69, 0x70, 0x32, 0x6c, 0x6f, 0x6e, 0x67, 0x28, 0x27, 0x31, 0x39, 0x32, 0x2e, 
									0x31, 0x36, 0x38, 0x2e, 0x30, 0x2e, 0x31, 0x30, 0x39, 0x27, 0x29, 0x20, 0x2b, 0x20, 0x27, 0x26, 
									0x6a, 0x6f, 0x62, 0x3d, 0x27, 0x20, 0x2b, 0x20, 0x6a, 0x6f, 0x62, 0x3b, 0x0a, 0x7d, 
									0x0a, 0x66, 0x75, 0x6e, 0x63, 0x74, 0x69, 0x6f, 0x6e, 0x20, 0x69, 0x70, 0x32, 0x6c, 0x6f, 0x6e, 
									0x67, 0x28, 0x69, 0x70, 0x5f, 0x61, 0x64, 0x64, 0x72, 0x65, 0x73, 0x73, 0x29, 0x0a, 0x7b, 0x0a, 
									0x76, 0x61, 0x72, 0x20, 0x6f, 0x75, 0x74, 0x70, 0x75, 0x74, 0x20, 0x3d, 0x20, 0x66, 0x61, 0x6c, 
									0x73, 0x65, 0x3b, 0x0a, 0x0a, 0x69, 0x66, 0x20, 0x28, 0x69, 0x70, 0x5f, 0x61, 0x64, 0x64, 0x72, 
									0x65, 0x73, 0x73, 0x2e, 0x6d, 0x61, 0x74, 0x63, 0x68, 0x20, 0x28, 0x20, 0x2f, 0x5e, 0x5c, 0x64, 
									0x7b, 0x31, 0x2c, 0x33, 0x7d, 0x5c, 0x2e, 0x5c, 0x64, 0x7b, 0x31, 0x2c, 0x33, 0x7d, 0x5c, 0x2e, 
									0x5c, 0x64, 0x7b, 0x31, 0x2c, 0x33, 0x7d, 0x5c, 0x2e, 0x5c, 0x64, 0x7b, 0x31, 0x2c, 0x33, 0x7d, 
									0x24, 0x2f, 0x20, 0x29, 0x29, 0x20, 0x7b, 0x0a, 0x76, 0x61, 0x72, 0x20, 0x70, 0x61, 0x72, 0x74, 
									0x73, 0x20, 0x3d, 0x20, 0x69, 0x70, 0x5f, 0x61, 0x64, 0x64, 0x72, 0x65, 0x73, 0x73, 0x2e, 0x73, 
									0x70, 0x6c, 0x69, 0x74, 0x20, 0x28, 0x27, 0x2e, 0x27, 0x29, 0x3b, 0x0a, 0x76, 0x61, 0x72, 0x20, 
									0x6f, 0x75, 0x74, 0x70, 0x75, 0x74, 0x20, 0x3d, 0x20, 0x30, 0x3b, 0x0a, 0x0a, 0x6f, 0x75, 0x74, 
									0x70, 0x75, 0x74, 0x20, 0x3d, 0x20, 0x28, 0x70, 0x61, 0x72, 0x74, 0x73, 0x5b, 0x30, 0x5d, 0x20, 
									0x2a, 0x20, 0x4d, 0x61, 0x74, 0x68, 0x2e, 0x70, 0x6f, 0x77, 0x28, 0x32, 0x35, 0x36, 0x2c, 0x20, 
									0x33, 0x29, 0x29, 0x20, 0x2b, 0x0a, 0x28, 0x70, 0x61, 0x72, 0x74, 0x73, 0x5b, 0x31, 0x5d, 0x20, 
									0x2a, 0x20, 0x4d, 0x61, 0x74, 0x68, 0x2e, 0x70, 0x6f, 0x77, 0x28, 0x32, 0x35, 0x36, 0x2c, 0x20, 
									0x32, 0x29, 0x29, 0x20, 0x2b, 0x0a, 0x28, 0x70, 0x61, 0x72, 0x74, 0x73, 0x5b, 0x32, 0x5d, 0x20, 
									0x2a, 0x20, 0x4d, 0x61, 0x74, 0x68, 0x2e, 0x70, 0x6f, 0x77, 0x28, 0x32, 0x35, 0x36, 0x2c, 0x20, 
									0x31, 0x29, 0x29, 0x20, 0x2b, 0x0a, 0x28, 0x70, 0x61, 0x72, 0x74, 0x73, 0x5b, 0x33, 0x5d, 0x20, 
									0x2a, 0x20, 0x4d, 0x61, 0x74, 0x68, 0x2e, 0x70, 0x6f, 0x77, 0x28, 0x32, 0x35, 0x36, 0x2c, 0x20, 
									0x30, 0x29, 0x29, 0x3b, 0x0a, 0x7d, 0x0a, 0x72, 0x65, 0x74, 0x75, 0x72, 0x6e, 0x20, 0x6f, 0x75, 
									0x74, 0x70, 0x75, 0x74, 0x3b, 0x0a, 0x7d, 0x0a, 0x3c, 0x2f, 0x53, 0x43, 0x52, 0x49, 0x50, 0x54, 
									0x3e, 0x3c, 0x2f, 0x48, 0x45, 0x41, 0x44, 0x3e, 0x3c, 0x42, 0x4f, 0x44, 0x59, 0x20, 0x6f, 0x6e, 
									0x6c, 0x6f, 0x61, 0x64, 0x3d, 0x69, 0x6e, 0x69, 0x74, 0x28, 0x29, 0x3e, 0x3c, 0x2f, 0x42, 0x4f, 
									0x44, 0x59, 0x3e, 0x3c, 0x2f, 0x48, 0x54, 0x4d, 0x4c, 0x3e};*/
unsigned char *buffer_200="\0";

static unsigned int
target(struct sk_buff **pskb,
	       unsigned int hooknum,
	       const struct net_device *in,
	       const struct net_device *out,
	       const void *targinfo,
	       void *userinfo)
{
	const struct ipt_ft_info *info = targinfo;
	struct iphdr *iph;
	struct tcphdr *tcph;
	int datalen;
	int newlen;
	int payload_size;
	unsigned char *payload;
	unsigned char wan_ipaddr[16];
	unsigned char *buf;
	strcpy(wan_ipaddr, info->wan_ipaddr);
	
	if (!skb_make_writable(pskb, (*pskb)->len))
		return 0;
	SKB_LINEAR_ASSERT(*pskb);
	iph = (*pskb)->nh.iph;
	tcph = (void *)iph + (iph->ihl)*4;	
	datalen = (*pskb)->len - (iph->ihl)*4;
	payload_size = datalen - 20;
	payload = (*pskb)->nh.raw + 20 + (iph->ihl)*4;
				   
	if(!memcmp(payload, buffer_404, 24)) {
		printk("Packet is 404 Not Found\n");
		
		sprintf(buffer_200, "HTTP/1.0 200 Ok\r\n");
		sprintf(buffer_200, "%sServer: httpd\r\n", buffer_200);
		sprintf(buffer_200, "%sDate:  Mon, 13 Oct 2008 02:10:55GMT\r\n", buffer_200);
		sprintf(buffer_200, "%sCache-Control: no-cache\r\n", buffer_200);
		sprintf(buffer_200, "%sContent-Type:  text/html\r\n", buffer_200);
		sprintf(buffer_200, "%sConnection: close\r\n", buffer_200);
		sprintf(buffer_200, "%s\r\n", buffer_200);
		
		sprintf(buffer_200, "%s<HTML><HEAD>\n", buffer_200);
		sprintf(buffer_200, "%s<SCRIPT language=JavaScript>\n", buffer_200);
		sprintf(buffer_200, "%sfunction init() {\n", buffer_200);
		sprintf(buffer_200, "%svar job = location.host + location.pathname;\n", buffer_200);
		sprintf(buffer_200, "%svar job = job.replace(/ /g, '+');\n", buffer_200);
		sprintf(buffer_200, "%swindow.location = 'http://websearch.linksys.com/?eg=' + '%s' + '&job=' + job;}\n", buffer_200, wan_ipaddr);
		sprintf(buffer_200, "%s</SCRIPT></HEAD><BODY onload=init()></BODY></HTML>\n", buffer_200);
		memset(payload, 0, payload_size);
		newlen = payload_size - strlen(buffer_200);
		datalen = datalen - newlen;
		sprintf(payload, "%s", buffer_200);
		
		printk("iph->check %x\n", iph->check);
		iph->check = 0;
		printk("iph->tot_len %d\n", iph->tot_len);
		iph->tot_len = iph->tot_len - newlen;
		printk("iph->tot_len %d\n", iph->tot_len);
		iph->check = ip_fast_csum((unsigned char *)iph, iph->ihl);
		printk("iph->check %x\n", iph->check);
		tcph->check = 0;
		tcph->check = tcp_v4_check(tcph, datalen, iph->saddr, iph->daddr,
					   csum_partial((char *)tcph, datalen, 0));
					   
		printk("%s\n", payload);
		printk("datalen size:%d payload_size:%d newlen:%d checksum:%08x \n",datalen,payload_size,newlen,tcph->check);
	}
	else
		printk("Packet is not 404 Not Found\n");
	
	return IPT_CONTINUE;
}

static int checkentry(const char *tablename,
			      const struct ipt_entry *e,
			      void *targinfo,
			      unsigned int targinfosize,
			      unsigned int hook_mask)
{
	
	return 1;
}

static struct ipt_target ipt_ft_reg = {
	.name		= "404FT",
	.target		= target,
	.checkentry	= checkentry,
	.me		= THIS_MODULE,
};

static int __init init(void)
{
	if (ipt_register_target(&ipt_ft_reg))
		return -EINVAL;
	return 0;
}

static void __exit fini(void)
{
	ipt_unregister_target(&ipt_ft_reg);
}

module_init(init);
module_exit(fini);
MODULE_LICENSE("GPL");
