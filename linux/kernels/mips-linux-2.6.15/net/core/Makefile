#
# Makefile for the Linux networking core.
#

obj-y := sock.o request_sock.o skbuff.o iovec.o datagram.o stream.o scm.o \
	 gen_stats.o gen_estimator.o

obj-$(CONFIG_SYSCTL) += sysctl_net_core.o

obj-y		     += dev.o ethtool.o dev_mcast.o dst.o \
			neighbour.o rtnetlink.o utils.o link_watch.o filter.o

# for IGMP MLD snooping function			
#obj-y		     += igmp_mld_db.o

obj-$(CONFIG_XFRM) += flow.o
obj-$(CONFIG_SYSFS) += net-sysfs.o
obj-$(CONFIG_NET_DIVERT) += dv.o
obj-$(CONFIG_NET_PKTGEN) += pktgen.o
obj-$(CONFIG_NET_RADIO) += wireless.o
obj-$(CONFIG_NETPOLL) += netpoll.o
# +++ Gemtek Added +++
obj-$(CONFIG_NET_PPPOE_PASSTHRU) += pppoe_pthrough_alg.o
#obj-$(CONFIG_NET_MULTICAST_PASSTHRU) += multicast_pthrough_alg.o
obj-$(CONFIG_NET_IPv6_PASSTHRU) += ipv6_pthrough_alg.o
# --- Gemtek Added ---
