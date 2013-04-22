/* Shared library add-on to iptables to add LOG support. */
#include <stdio.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <syslog.h>
#include <getopt.h>
#include <iptables.h>
#include <linux/netfilter_ipv4/ip_tables.h>
#include <linux/netfilter_ipv4/ipt_PROCESS.h>

#define IPT_PROCESS_PROCESSNAME 	0x01


/* Function which prints out usage message. */
static void	help(void)
{
	printf(
		"TRIGGER v%s options:\n"
		" --process-path process_full_path\n",
		IPTABLES_VERSION);
}

static struct option opts[] = {
	{ "process-path", 1, 0, '!' },
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
	struct ipt_process_info *processinfo = (struct ipt_process_info *)(*target)->data;

	switch (c) {
	case '!':
		if (*flags & IPT_PROCESS_PROCESSNAME)
			exit_error(PARAMETER_PROBLEM, "Can't specify --process-path twice");

		if (check_inverse(optarg, &invert, NULL, 0))
			exit_error(PARAMETER_PROBLEM, "Unexpected `!' after --process-path");

		if (strlen(optarg) > sizeof(processinfo->path) - 1)
			exit_error(PARAMETER_PROBLEM, "Maximum prefix length %u for --process-path", (int)(sizeof(processinfo->path) - 1));

		strcpy(processinfo->path, optarg);
		
		*flags |= IPT_PROCESS_PROCESSNAME;
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
	const struct ipt_process_info *processinfo = (const struct ipt_process_info *)target->data;

	if (strcmp(processinfo->path, "") != 0)
		printf("path '%s' ", processinfo->path);
}

/* Saves the union ipt_targinfo in parsable form to stdout. */
static void
save(const struct ipt_ip *ip, const struct ipt_entry_target *target)
{
	const struct ipt_process_info *processinfo = (const struct ipt_process_info *)target->data;

	if (strcmp(processinfo->path, "") != 0)
		printf("--process-path \"%s\" ", processinfo->path);
}

/*static
struct iptables_target process
= { NULL,
    "PROCESS",
    IPTABLES_VERSION,
    IPT_ALIGN(sizeof(struct ipt_process_info)),
    IPT_ALIGN(sizeof(struct ipt_process_info)),
    &help,
    &init,
    &parse,
    &final_check,
    &print,
    &save,
    opts
};
*/
static struct iptables_target process = {
	.name		= "PROCESS",
	.version	= IPTABLES_VERSION,
	.size		= IPT_ALIGN(sizeof(struct ipt_process_info)),
	.userspacesize	= IPT_ALIGN(sizeof(struct ipt_process_info)),
	.help		= help,
	.parse		= parse,
	.final_check	= final_check,
	.print		= print,
	.save		= save,
	.extra_opts	= opts,
};

void _init(void)
{
	register_target(&process);
}
