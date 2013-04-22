#ifndef _IPT_AUTOFW_H_target
#define _IPT_AUTOFW_H_target

#define AUTOFW_TIMEOUT 600	/* 600 secs */

enum ipt_autofw_type
{
	IPT_AUTOFW_DNAT = 1,
	IPT_AUTOFW_IN = 2,
	IPT_AUTOFW_OUT = 3
};

struct ipt_autofw_ports {
	u_int16_t mport[2];	/* Related destination port range */
	u_int16_t rport[2];	/* Port range to map related destination port range to */
};

struct ipt_autofw_info {
	enum ipt_autofw_type type;
	u_int16_t proto;	/* Related protocol */
	struct ipt_autofw_ports ports;
};

#endif /*_IPT_AUTOFW_H_target*/
