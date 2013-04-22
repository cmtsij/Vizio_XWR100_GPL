/*
 *********************************************************
 *   Copyright 2008, Gemtek  Inc.  All Rights Reserved *
 *********************************************************
 #v1.0 Added the 404 packet filter module. Marmot_Chen 20081015
*/
/* Shared library add-on to iptables to add LOG support. */
#include <stdio.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <syslog.h>
#include <getopt.h>
#include <iptables.h>
#include <linux/netfilter_ipv4/ip_tables.h>
#include <linux/netfilter_ipv4/ipt_404FT.h>
#define IPT_WAN_IPADDR 	0x01


/* Function which prints out usage message. */
static void	help(void)
{
	printf(
		"404FT v%s\n"
		" --wan-ipaddr WAN IP Address\n",
		IPTABLES_VERSION);
}

static struct option opts[] = {
	{ "wan-ipaddr", 1, 0, '!' },
	{ 0 }
};

/* Initialize the target. */
static void
init(struct ipt_entry_target *t, unsigned int *nfcache)
{
	/* Can't cache this */
	*nfcache |= NFC_UNKNOWN;
}

/* Function which parses command options; returns true if it
   ate an option */
static int
parse(int c, char **argv, int invert, unsigned int *flags,
      const struct ipt_entry *entry,
      struct ipt_entry_target **target)
{
	struct ipt_ft_info *info = (struct ipt_ft_info *)(*target)->data;

	switch (c) {
	case '!':
		if (*flags & IPT_WAN_IPADDR)
			exit_error(PARAMETER_PROBLEM, "Can't specify --wan-ipaddr twice");

		if (check_inverse(optarg, &invert, NULL, 0))
			exit_error(PARAMETER_PROBLEM, "Unexpected `!' after --wan-ipaddr");

		if (strlen(optarg) > sizeof(info->wan_ipaddr) - 1)
			exit_error(PARAMETER_PROBLEM, "Maximum prefix length %u for --wan-ipaddr", (int)(sizeof(info->wan_ipaddr) - 1));

		strcpy(info->wan_ipaddr, optarg);
		
		*flags |= IPT_WAN_IPADDR;
		break;

	default:
		return 0;
	}
	return 1;
}

/* Final check; nothing. */
static void final_check(unsigned int flags)
{
}

/* Prints out the targinfo. */
static void
print(const struct ipt_ip *ip,
      const struct ipt_entry_target *target,
      int numeric)
{
	const struct ipt_ft_info *info = (const struct ipt_ft_info *)target->data;

	if (strcmp(info->wan_ipaddr, "") != 0)
		printf("wan ipaddr '%s' ", info->wan_ipaddr);

}

/* Saves the union ipt_targinfo in parsable form to stdout. */
static void
save(const struct ipt_ip *ip, const struct ipt_entry_target *target)
{
	const struct ipt_ft_info *info = (const struct ipt_ft_info *)target->data;

	if (strcmp(info->wan_ipaddr, "") != 0)
		printf("--wan-ipaddr \"%s\" ", info->wan_ipaddr);
}

static struct iptables_target ft = {
	.name		= "404FT",
	.version	= IPTABLES_VERSION,
	.size		= IPT_ALIGN(sizeof(struct ipt_ft_info)),
	.userspacesize	= IPT_ALIGN(sizeof(struct ipt_ft_info)),
	.help		= help,
	.parse		= parse,
	.final_check	= final_check,
	.print		= print,
	.save		= save,
	.extra_opts	= opts,
};

void _init(void)
{
	register_target(&ft);
}
