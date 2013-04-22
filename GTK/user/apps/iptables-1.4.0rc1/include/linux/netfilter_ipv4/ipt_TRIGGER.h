#include <stdio.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>
#include <iptables.h>

#include <linux/netfilter_ipv4/ip_tables.h>
#include <linux/netfilter_ipv4/ip_nat_rule.h>
#include <linux/netfilter_ipv4/ipt_TRIGGER.h>

/* Function which prints out usage message. */
static void
help(void)
{
	printf(
"autofw v%s options:\n"
" --related-type (dnat|in|out)\n"
"				Trigger type\n"
" --related-proto proto\n"
"				Trigger protocol\n"
" --related-dport port[-port]\n"
"				Trigger destination port range\n"
" --related-to port[-port]\n"
"				Port range to map related destination port range to.\n\n",
IPTABLES_VERSION);
}

static struct option opts[] = {
	{ "related-type", 1, 0, '1' },
	{ "related-proto", 1, 0, '2' },
	{ "related-match", 1, 0, '3' },
	{ "related-relate", 1, 0, '4' },
	{ 0 }
};

/* Initialize the target. */
static void
init(struct ipt_entry_target *t, unsigned int *nfcache)
{
	/* Can't cache this */
	*nfcache |= NFC_UNKNOWN;
}

/* Parses ports */
static void
parse_ports(const char *arg, u_int16_t *ports)
{
	const char *dash;
	int port;

	port = atoi(arg);
	if (port == 0 || port > 65535)
		exit_error(PARAMETER_PROBLEM, "Port range `%s' invalid\n", arg);

	dash = strchr(arg, '-');
	if (!dash)
		ports[0] = ports[1] = port;
	else {
		int maxport;

		maxport = atoi(dash + 1);
		if (maxport == 0 || maxport > 65535)
			exit_error(PARAMETER_PROBLEM,
				   "Port range `%s' invalid\n", dash+1);
		if (maxport < port)
			exit_error(PARAMETER_PROBLEM,
				   "Port range `%s' invalid\n", arg);
		ports[0] = port;
		ports[1] = maxport;
	}
}


/* Function which parses command options; returns true if it
   ate an option */
static int
parse(int c, char **argv, int invert, unsigned int *flags,
      const struct ipt_entry *entry,
      struct ipt_entry_target **target)
{
	struct ip_autofw_info *info = (struct ip_autofw_info *)(*target)->data;

	switch (c) {
	case '1':
		if (!strcasecmp(optarg, "dnat"))
			info->type = IPT_TRIGGER_DNAT;
		else if (!strcasecmp(optarg, "in"))
			info->type = IPT_TRIGGER_IN;
		else if (!strcasecmp(optarg, "out"))
			info->type = IPT_TRIGGER_OUT;
		else
			exit_error(PARAMETER_PROBLEM,
				   "unknown type `%s' specified", optarg);
		return 1;

	case '2':
		if (!strcasecmp(optarg, "tcp"))
			info->proto = IPPROTO_TCP;
		else if (!strcasecmp(optarg, "udp"))
			info->proto = IPPROTO_UDP;
		else if (!strcasecmp(optarg, "all"))
			info->proto = 0;
		else
			exit_error(PARAMETER_PROBLEM,
				   "unknown protocol `%s' specified", optarg);
		return 1;

	case '3':
		if (check_inverse(optarg, &invert, &optind, 0))
			exit_error(PARAMETER_PROBLEM,
				   "Unexpected `!' after --related-dport");

		parse_ports(optarg, info->ports.mport);
		return 1;

	case '4':
		if (check_inverse(optarg, &invert, &optind, 0))
			exit_error(PARAMETER_PROBLEM,
				   "Unexpected `!' after --related-to");

		parse_ports(optarg, info->ports.rport);
		*flags |= IP_NAT_RANGE_PROTO_SPECIFIED;
		return 1;

	default:
		return 0;
	}
}

/* Final check; don't care. */
static void final_check(unsigned int flags)
{
}

/* Prints out the targinfo. */
static void
print(const struct ipt_ip *ip,
      const struct ipt_entry_target *target,
      int numeric)
{
	struct ip_autofw_info *info = (struct ip_autofw_info *)target->data;

		fprintf(stderr, "%s %d %d\n", __FUNCTION__, __LINE__, info->type);
		fprintf(stderr, "%s %d %d\n", __FUNCTION__, __LINE__, info->proto);
		fprintf(stderr, "%s %d port[0]%d port[1]%d\n", __FUNCTION__, __LINE__, info->ports.mport[0], info->ports.mport[1]);
		fprintf(stderr, "%s %d port[0]%d port[1]%d\n", __FUNCTION__, __LINE__, info->ports.rport[0], info->ports.rport[1]);
	printf("autofw ");
	if (info->type == IPT_TRIGGER_DNAT)
		printf("type:dnat ");
	else if (info->type == IPT_TRIGGER_IN)
		printf("type:in ");
	else if (info->type == IPT_TRIGGER_OUT)
		printf("type:out ");

	if (info->proto == IPPROTO_TCP)
		printf("tcp ");
	else if (info->proto == IPPROTO_UDP)
		printf("udp ");

	printf("dport:%hu", info->ports.mport[0]);
	if (info->ports.mport[1] > info->ports.mport[0])
		printf("-%hu", info->ports.mport[1]);
	printf(" ");

	printf("to:%hu", info->ports.rport[0]);
	if (info->ports.rport[1] > info->ports.rport[0])
		printf("-%hu", info->ports.rport[1]);
	printf(" ");
}

/* Saves the union ipt_targinfo in parsable form to stdout. */
static void
save(const struct ipt_ip *ip, const struct ipt_entry_target *target)
{
	struct ip_autofw_info *info = (struct ip_autofw_info *)target->data;

	printf("--related-proto ");
	if (info->proto == IPPROTO_TCP)
		printf("tcp ");
	else if (info->proto == IPPROTO_UDP)
		printf("udp ");
	printf("--related-dport %hu-%hu ", info->ports.mport[0], info->ports.mport[1]);
	printf("--related-to %hu-%hu ", info->ports.rport[0], info->ports.rport[1]);
}


static struct iptables_target trigger = {
	.name		= "autofw",
	.version	= IPTABLES_VERSION,
	.size		= IPT_ALIGN(sizeof(struct ip_autofw_info)),
	.userspacesize	= IPT_ALIGN(sizeof(struct ip_autofw_info)),
	.help		= help,
	.parse		= parse,
	.final_check	= final_check,
	.print		= print,
	.save		= save,
	.extra_opts	= opts,
};

void _init(void)
{
	register_target(&trigger);
}
