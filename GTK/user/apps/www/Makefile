all: 
	
install:
	@if [ ! -d $(VENDOR)_$(MODEL)_$(WIRELESS_SEC) ] ; then \
		echo "ERROR :";\
		echo "ERROR : Incorrect GUI Path";\
		echo "ERROR : Please make sure you have assigned 'VENDOR' and 'MODEL' correctly";\
		echo "ERROR :";\
		exit 1 ;\
	fi
	$(MAKE) -C $(VENDOR)_$(MODEL)_$(WIRELESS_SEC) install

clean:
#	rm -rf $(KERNEL_INSTALL_ROOT)/www