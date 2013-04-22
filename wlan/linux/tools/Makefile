# $Id: //depot/sw/releases/7.3_AP/wlan/linux/tools/Makefile#8 $
#
# Makefile for Atheros tools.
#

ifeq ($(obj),)
obj := .
endif

DEPTH := ../..

BINDIR=	/usr/local/bin

include $(obj)/${DEPTH}/linux/Makefile.inc



ALL=	athstats athstatsclear 80211stats athkey athchans athctrl\
	athdebug 80211debug wlanconfig athcwm athampdutrc atrc \
	pktlogconf pktlogdump

ATH_TOOLS= athstats athstatsclr pktlogconf pktlogdump 80211stats

ifneq ($(strip ${ATH_SUPPORT_DFS}),0)
ALL+= radartool
ATH_TOOLS+= radartool
endif

all:	${ALL}

INCS+=	-I${HAL} -I${HAL}/${OS} -I${ATH_HAL} -I${obj}/${DEPTH} -I${COMMON} -I${ATH_RATE} -I${ATH} -I${WLAN} -I$(COMMON)/ath_pktlog -I$(COMMON)/dfs -I../ath
CFLAGS=	${INCS} ${COPTS}

ifeq (${BUILD_STATIC}, y)
LDFLAGS= -static
endif

ifeq (${BUILD_DEBUG}, y)
CFLAGS += -g
endif

ifeq ($(ATH_CONFIG_NVRAM), 1)
CFLAGS += -DCONFIG_NVRAM=1
endif

CFILES = cgiMain.c

ifeq ($(ATH_CONFIG_ETH_EXT), 1)
CFLAGS += -DCONFIG_ATHR_ETH_EXTENSION=1 -I$(TOPDIR)/linux/drivers/net/ag7240
CFILES += ethext.c
endif

ifeq ($(ATH_SINGLE_CFG),1)
IWDIR = $(TOPDIR)/apps/wireless_tools.28
CFLAGS += -DATH_SINGLE_CFG=1 -I $(IWDIR) -lm
CFILES +=  wlancfg.c $(IWDIR)/iwlib.c
endif

ifeq ($(BUILD_OPTIMIZED),y)
CFLAGS += -Os -pipe -mips32r2 -mtune=mips32r2 -funit-at-a-time
endif

all:	${ALL}

ath_tools: ${ATH_TOOLS}
ifneq ($(BOARD_TYPE),ap91-2MB)
	cp athstats $(INSTALL_ROOT)/sbin;\
	cp athstatsclr $(INSTALL_ROOT)/sbin;\
	cp pktlogconf $(INSTALL_ROOT)/sbin;\
	cp radartool $(INSTALL_ROOT)/sbin;\
	cp 80211stats $(INSTALL_ROOT)/sbin;\
	cp pktlogdump $(INSTALL_ROOT)/sbin;
else
	@echo Athtools done....
endif

#- athampdutrc athcwm atrc pktlogconf pktlogdump radartool 

athstats: athstats.o
	${CC} -o athstats ${CFLAGS} -I../ath ${LDFLAGS} athstats.o
#athstats: athstats.c
#	${CC} -o athstats ${CFLAGS} -I../ath ${LDFLAGS} athstats.c

athstatsclr: athstatsclr.o
	${CC} -o athstatsclr ${CFLAGS} -I../ath ${LDFLAGS} athstatsclr.o
#athstatsclr: athstatsclr.c
#	${CC} -o athstatsclr ${CFLAGS} -I../ath ${LDFLAGS} athstatsclr.c
athctrl: athctrl.c
	${CC} -o athctrl ${CFLAGS} ${LDFLAGS} athctrl.c

athampdutrc: athampdutrc.c
	${CC} -o athampdutrc ${CFLAGS} -I../ath ${LDFLAGS} athampdutrc.c

80211stats: 80211stats.o
	${CC} -o 80211stats ${CFLAGS} ${LDFLAGS} 80211stats.o
#80211stats: 80211stats.c
#	${CC} -o 80211stats ${CFLAGS} ${LDFLAGS} 80211stats.c
athkey: athkey.c
	${CC} -o athkey ${CFLAGS} ${LDFLAGS} athkey.c

cgiMain: cgiMain.o
	${CC} -o cgiMain ${CFLAGS} ${LDFLAGS} cgiMain.o
#cgiMain: ${CFILES}
#	${CC} -o cgiMain ${CFLAGS} ${LDFLAGS} ${CFILES}
	cp cgiMain $(INSTALL_ROOT)/usr/www/cgi-bin

athdebug: athdebug.c
	${CC} -o athdebug ${CFLAGS} ${LDFLAGS} athdebug.c
wlanconfig: wlanconfig.o
	${CC} -o wlanconfig ${CFLAGS} ${LDFLAGS} wlanconfig.o
#wlanconfig: wlanconfig.c
#	${CC} -o wlanconfig ${CFLAGS} ${LDFLAGS} wlanconfig.c
80211debug: 80211debug.c
	${CC} -o 80211debug ${CFLAGS} ${LDFLAGS} 80211debug.c
athcwm: athcwm.c
	${CC} -o athcwm ${CFLAGS} -I../ath ${LDFLAGS} athcwm.c
atrc: atrc.c
	${CC} -o atrc ${CFLAGS} -I../atrc ${LDFLAGS} atrc.c
pktlogdump: pktlogdump.o
	${CC} -o pktlogdump ${CFLAGS} -I$(COMMON)/ath_pktlog ${LDFLAGS} pktlogdump.o -lm
#pktlogdump: pktlogdump.c
#	${CC} -o pktlogdump ${CFLAGS} -I$(COMMON)/ath_pktlog ${LDFLAGS} pktlogdump.c -lm
pktlogconf: pktlogconf.o
	${CC} -o pktlogconf ${CFLAGS} -I$(COMMON)/ath_pktlog ${LDFLAGS} pktlogconf.o
#pktlogconf: pktlogconf.c
#	${CC} -o pktlogconf ${CFLAGS} -I$(COMMON)/ath_pktlog ${LDFLAGS} pktlogconf.c
radartool: radartool.o
	${CC} -o radartool ${CFLAGS} -I$(COMMON)/dfs -I../ath ${LDFLAGS} radartool.o
#radartool: radartool.c
#	${CC} -o radartool ${CFLAGS} -I$(COMMON)/dfs -I../ath ${LDFLAGS} radartool.c

install: ${ALL}
	cp athstats ${DESTDIR}${BINDIR}
	cp athstatsclr ${DESTDIR}${BINDIR}
	cp athampdutrc ${DESTDIR}${BINDIR}
	cp athcwm ${DESTDIR}${BINDIR}
	cp athdebug ${DESTDIR}${BINDIR}
	cp 80211stats ${DESTDIR}${BINDIR}
	cp atrc ${DESTDIR}${BINDIR}
	cp pktlogconf ${DESTDIR}${BINDIR}
	cp pktlogdump ${DESTDIR}${BINDIR}
	-rm ${DESTDIR}/${BINDIR}/wlanstats
	ln ${DESTDIR}/${BINDIR}/80211stats ${DESTDIR}/${BINDIR}/wlanstats
	cp 80211debug ${DESTDIR}${BINDIR}
	-rm ${DESTDIR}/${BINDIR}/wlandebug
	ln ${DESTDIR}/${BINDIR}/80211debug ${DESTDIR}/${BINDIR}/wlandebug
	cp wlanconfig ${DESTDIR}${BINDIR}
	cp radartool ${DESTDIR}${BINDIR}/radartool

ath_tools_clean:
	@rm -f athstats athstatsclr pktlogdump pktlogconf radartool

clean: ath_tools_clean
	rm -f ${ALL} core a.out
