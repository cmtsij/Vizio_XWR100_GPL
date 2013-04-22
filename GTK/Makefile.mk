# Start of Gemtek Added 20080807
export MODEL = XWR100

gemtek_prepare:
	rm -f $(KERNELPATH)/arch/mips/ar7100/reboot.c
	ln -sf $(TOPDIR)/GTK/user/apps/GPIO/reboot.c $(KERNELPATH)/arch/mips/ar7100/reboot.c


gemtek_image:
	@echo making Gemtek Image
	cd ../GTK ; $(MAKE) release

gemtek_clean:
	@echo Clean Gemtek Applications
	cd ../GTK ; $(MAKE) clean
	
#
#	You can type "make gemtek_build MOD=rc" to build single modules
#

gemtek_build:
ifneq ($(MOD),)
	cd $(TOPDIR)/GTK; $(MAKE) $(MOD)
else
	cd $(TOPDIR)/GTK; $(MAKE) all
endif
# End of Gemtek Added 20080807