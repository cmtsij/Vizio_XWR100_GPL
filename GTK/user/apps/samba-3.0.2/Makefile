all:
	$(MAKE) -C source

romfs: all
#	$(ROMFSINST) source/bin/CP850.so /lib/CP850.so
#	$(ROMFSINST) source/bin/CP437.so /lib/CP437.so
#	$(ROMFSINST) source/bin/libbigballofmud.so /lib/libbigballofmud.so
#	ln -snf libbigballofmud.so $(ROOTDIR)/romfs/lib/libbigballofmud.so.0
#	$(ROMFSINST) source/bin/nmbd /sbin/nmbd
#	$(ROMFSINST) source/bin/smbd /sbin/smbd
#	$(ROMFSINST) source/bin/smbpasswd /sbin/smbpasswd
#	$(ROMFSINST) /bin/samba.sh

#	cp source/bin/CP850.so $(ROMFSDIR)/lib/CP850.so
#	cp source/bin/CP437.so $(ROMFSDIR)/lib/CP437.so
#	cp source/bin/libbigballofmud.so $(ROMFSDIR)/lib/libbigballofmud.so
#	cd $(ROMFSDIR)/lib && ln -snf libbigballofmud.so libbigballofmud.so.0
#	cp source/bin/nmbd $(ROMFSDIR)/sbin/nmbd
#	cp source/bin/smbd $(ROMFSDIR)/sbin/smbd
#	cp source/bin/smbpasswd $(ROMFSDIR)/sbin/smbpasswd
#	cp samba.sh $(ROMFSDIR)/sbin/samba.sh
	rm -rf $(ROMFSDIR)/usr/local/samba/private
	rm -rf $(ROMFSDIR)/usr/local/samba/var/locks
#        $(MAKE) -C source install
	rm -rf $(ROMFSDIR)/usr/local/samba/include/*
#       rmdir $(ROMFSDIR)/usr/local/samba/include
	rm -rf $(ROMFSDIR)/usr/local/samba/swat
	rm -rf $(ROMFSDIR)/usr/local/samba/man
	rm -rf $(ROMFSDIR)/usr/local/samba/bin/*.old
	rm -rf $(ROMFSDIR)/usr/local/samba/sbin/*.old
	rm -rf $(ROMFSDIR)/usr/local/samba/bin/findsmb
	rm -rf $(ROMFSDIR)/usr/local/samba/bin/nmblookup
	rm -rf $(ROMFSDIR)/usr/local/samba/bin/rpcclient
	rm -rf $(ROMFSDIR)/usr/local/samba/bin/smbcacls
	rm -rf $(ROMFSDIR)/usr/local/samba/bin/smbclient
	rm -rf $(ROMFSDIR)/usr/local/samba/bin/smbspool
	rm -rf $(ROMFSDIR)/usr/local/samba/bin/smbtar
	rm -rf $(ROMFSDIR)/usr/local/samba/sbin/swat
	rm -rf $(ROMFSDIR)/usr/local/samba/bin/testparm
	rm -rf $(ROMFSDIR)/usr/local/samba/bin/testprns
	rm -rf $(ROMFSDIR)/usr/local/samba/bin/wbinfo
	rm -rf $(ROMFSDIR)/usr/local/samba/bin/ntlm_auth      
	rm -rf $(ROMFSDIR)/usr/local/samba/bin/pdbedit        
	rm -rf $(ROMFSDIR)/usr/local/samba/bin/profiles
	rm -rf $(ROMFSDIR)/usr/local/samba/bin/smbcquotas
	rm -rf $(ROMFSDIR)/usr/local/samba/bin/smbtree
	rm -rf $(ROMFSDIR)/usr/local/samba/bin/tdbdump
	rm -rf $(ROMFSDIR)/usr/local/samba/sbin/winbindd
#	install -D $(ROMFSDIR)/usr/local/samba/bin/libbigballofmud.so $(ROMFSDIR)/lib/libbigballofmud.so
	install -D source/bin/libbigballofmud.so $(ROMFSDIR)/lib/libbigballofmud.so
	cd $(ROMFSDIR)/lib && ln -snf libbigballofmud.so libbigballofmud.so.0
	rm -rf $(ROMFSDIR)/usr/local/samba/bin/libbigballofmud.so
	install -D source/bin/nmbd $(ROMFSDIR)/usr/local/samba/sbin/nmbd
	install -D source/bin/smbd $(ROMFSDIR)/usr/local/samba/sbin/smbd
	install -D source/bin/smbpasswd $(ROMFSDIR)/usr/local/samba/bin/smbpasswd
#	$(STRIP) $(ROMFSDIR)/lib/libbigballofmud.so
#	$(STRIP) $(ROMFSDIR)/usr/local/samba/sbin/nmbd
#	$(STRIP) $(ROMFSDIR)/usr/local/samba/sbin/smbd
#	$(STRIP) $(ROMFSDIR)/usr/local/samba/bin/smbpasswd
	install -d $(ROMFSDIR)/usr/local/samba/lib
	rm -rf $(ROMFSDIR)/usr/local/samba/lib/vfs/*
#	rmdir $(ROMFSDIR)/usr/local/samba/lib/vfs
	rm -f $(ROMFSDIR)/usr/local/samba/lib/*.msg
	rm -f $(ROMFSDIR)/usr/local/samba/lib/*.dat
	rm -f $(ROMFSDIR)/usr/local/samba/lib/*.so
	rm -f $(ROMFSDIR)/usr/local/samba/lib/smb.conf
	cd $(ROMFSDIR)/usr/local/samba/lib && ln -sf /tmp/samba/lib/smb.conf smb.conf
	rm -rf $(ROMFSDIR)/usr/local/samba/private
	cd $(ROMFSDIR)/usr/local/samba && ln -sf /tmp/samba/private private
	rm -rf $(ROMFSDIR)/usr/local/samba/var/locks
#	cd $(ROMFSDIR)/usr/local/samba/var && ln -sf /tmp/samba/var/locks locks
	install -d $(ROMFSDIR)/usr/local/samba/var 
	cd $(ROMFSDIR)/usr/local/samba/var && ln -sf /tmp/var/lock locks
	cd $(ROMFSDIR)/etc && ln -sf /tmp/samba/lib/smb.conf smb.conf

clean:
	$(MAKE) -C source clean
