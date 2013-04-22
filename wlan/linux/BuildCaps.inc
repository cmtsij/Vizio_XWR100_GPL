#
# defaults
#
ifndef ATH_CAP_SUPERG_FF
export ATH_CAP_SUPERG_FF=0
endif

ifndef ATH_CAP_DYNTURBO
export ATH_CAP_DYNTURBO=0
endif

ifndef ATH_CAP_SUPERG_COMP
export ATH_CAP_SUPERG_COMP=0
endif

ifndef ATH_CAP_XR
export ATH_CAP_XR=0
endif

ifndef ATH_CAP_TPC
export ATH_CAP_TPC=0
endif

ifndef ATH_CAP_CWM
export ATH_CAP_CWM=1
endif

ifndef ATH_CHAINMASK_SELECT
export ATH_CHAINMASK_SELECT=1
endif

ifndef ATH_FORCE_PPM
export ATH_FORCE_PPM=0
endif

ifndef ATH_FORCE_BIAS
export ATH_FORCE_BIAS=0
endif

ifndef AR5416_EMULATION
export AR5416_EMULATION=0
endif

ifndef AR9100
export AR9100=0
endif

ifndef ATH_RXBUF
export ATH_RXBUF=512
endif

ifndef AH_CAL_IN_FLASH
export AH_CAL_IN_FLASH=0
endif

ifndef AH_SUPPORT_DFS
export AH_SUPPORT_DFS=1
endif

ifndef AH_WAR_52640
export AH_WAR_52640=0
endif

ifndef ATH_WPS_IE
export ATH_WPS_IE=1
endif

ifndef CONFIG_SMP
  ifndef ATH_CAP_AMSDU
    export ATH_CAP_AMSDU=1
  endif
else
    export ATH_CAP_AMSDU=0
endif

ifndef ATH_SUPPORT_UAPSD
export ATH_SUPPORT_UAPSD=1
endif

ifndef BUILD_AP
export BUILD_AP=1
endif

ifndef ATH_WLAN_COMBINE
export ATH_WLAN_COMBINE=0
endif

ifndef ATH_NO_SW_CRYPTO
export ATH_NO_SW_CRYPTO=0
endif

ifndef ATH_NO_5G_SUPPORT
export ATH_NO_5G_SUPPORT=0
endif

ifndef IEEE80211_MCAST_ENHANCEMENT
export IEEE80211_MCAST_ENHANCEMENT=1
endif

ifndef ATH_SUPPORT_IQUE
export ATH_SUPPORT_IQUE=0
endif

ifndef AP_SLOW_ANT_DIV
export AP_SLOW_ANT_DIV=0
endif

ifndef LLC_HDR_WAR
export LLC_HDR_WAR=0
endif

ifndef ATH_WLAN_LED
export ATH_WLAN_LED=0
endif

ifndef NO_SIMPLE_CONFIG
export NO_SIMPLE_CONFIG=0
endif

ifndef CAVIUM_FLUSH_CACHE
export CAVIUM_FLUSH_CACHE=0
endif

#
# directives
#
#for station linux drivers only
ifeq ($(strip ${BOARD_TYPE}),localPC)
COPTS+= -DATH_SUPPORT_STA=1
endif

ifeq ($(strip ${BOARD_TYPE}),EeePC)
COPTS+= -DATH_SUPPORT_STA=1
endif

ifneq ($(strip ${ATH_CAP_SUPERG_FF}),0)
COPTS+= -DATH_SUPERG_FF=1
endif

ifneq ($(strip ${ATH_CAP_DYNTURBO}),0)
COPTS+= -DATH_SUPERG_DYNTURBO=1 -DATH_TURBO_SCAN=1
endif

ifneq ($(strip ${ATH_CAP_SUPERG_COMP}),0)
COPTS+= -DATH_SUPERG_COMP=1
endif

ifneq ($(strip ${ATH_CAP_XR}),0)
COPTS+= -DATH_SUPERG_XR=1
endif

ifneq ($(strip ${ATH_CAP_TPC}),0)
COPTS+= -DATH_CAP_TPC=1
endif

ifneq ($(strip ${ATH_CAP_CWM}),0)
COPTS+= -DATH_CWM=1
endif

ifneq ($(strip ${ATH_CHAINMASK_SELECT}),0)
COPTS+= -DATH_CHAINMASK_SELECT=1
endif

ifneq ($(strip ${ATH_FORCE_PPM}),0)
COPTS+= -DATH_FORCE_PPM=1
endif

ifneq ($(strip ${ATH_FORCE_BIAS}),0)
COPTS+= -DATH_FORCE_BIAS=1
endif

ifneq ($(strip ${AR5416_EMULATION}),0)
COPTS+= -DAR5416_EMULATION=1
endif

ifneq ($(strip ${AR9100}),0)
COPTS+= -DAR9100=1
endif

COPTS+= -DATH_RXBUF=${ATH_RXBUF}

ifneq ($(strip ${ATH_SUPPORT_DFS}),0)
COPTS+= -DATH_SUPPORT_DFS=1
endif

ifneq ($(strip ${AH_CAL_IN_FLASH}),0)
COPTS+= -DAH_CAL_IN_FLASH=1 -DAH_CAL_RADIOS=${AH_CAL_RADIOS} -DAH_CAL_LOCATIONS=${AH_CAL_LOCATIONS}
endif

ifneq ($(strip ${AH_WAR_52640}),0)
COPTS+= -DAH_WAR_52640=1
endif

ifneq ($(strip ${ATH_RB}),0)
COPTS+= -DATH_RB=1
endif

ifneq ($(strip ${ATH_CAP_AMSDU}),0)
COPTS+= -DATH_AMSDU=1
endif

ifneq ($(strip ${ATH_SUPPORT_UAPSD}),0)
COPTS+= -DATH_SUPPORT_UAPSD=1
endif

ifneq ($(strip ${BUILD_AP}),0)
COPTS+= -DBUILD_AP=1
endif

ifneq ($(strip ${ATH_WLAN_COMBINE}),0)
COPTS+= -DATH_WLAN_COMBINE=1
endif

ifneq ($(strip ${ATH_NO_SW_CRYPTO}),0)
COPTS+= -DATH_NO_SW_CRYPTO=1
endif

ifneq ($(strip ${ATH_NO_5G_SUPPORT}),0)
COPTS+= -DATH_NO_5G_SUPPORT=1
endif

ifneq ($(strip ${IEEE80211_MCAST_ENHANCEMENT}),0)
COPTS+= -DIEEE80211_MCAST_ENHANCEMENT=1
endif

ifneq ($(strip ${ATH_SUPPORT_IQUE}),0)
COPTS+= -DATH_SUPPORT_IQUE=1
endif

ifneq ($(strip ${AP_SLOW_ANT_DIV}),0)
COPTS+= -DAP_SLOW_ANT_DIV=1
endif

ifneq ($(strip ${LLC_HDR_WAR}),0)
COPTS+= -DLLC_HDR_WAR=1
endif

ifneq ($(strip ${ATH_WPS_IE}),0)
COPTS+= -DATH_WPS_IE=1
endif

ifneq ($(strip ${ATH_WLAN_LED}),0)
COPTS+= -DATH_WLAN_LED=${ATH_WLAN_LED}
COPTS+= -DGPIO_PIN_FUNC_0=${GPIO_PIN_FUNC_0}
COPTS+= -DGPIO_PIN_FUNC_1=${GPIO_PIN_FUNC_1}
COPTS+= -DGPIO_PIN_FUNC_2=${GPIO_PIN_FUNC_2}
endif

ifneq ($(strip ${NO_SIMPLE_CONFIG}),0)
COPTS+= -DNO_SIMPLE_CONFIG=1
endif

ifeq ($(strip ${HOWL_AES_CORR_WAR_58820}),1)
COPTS+= -DHOWL_AES_CORR_WAR_58820
endif

ifneq ($(strip ${CAVIUM_FLUSH_CACHE}),0)
COPTS+= -DCAVIUM_FLUSH_CACHE=1
endif

