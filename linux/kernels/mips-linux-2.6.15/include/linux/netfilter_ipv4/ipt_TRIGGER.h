#ifndef _IPT_TRIGGER_H
#define _IPT_TRIGGER_H

struct ipt_trigger_info {
	char process_name[28];
	unsigned int timer;
};

#endif /*_IPT_TRIGGER_H*/
