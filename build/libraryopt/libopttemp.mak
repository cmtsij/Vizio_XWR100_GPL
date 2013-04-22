
#
# libopttemp.mak -- included into $(TOPDIR)/build/Makefile .
# Makes a temporary directory $(TOPDIR)/libopt.temp
# that the library optimizer needs in order to do it's work.
#
# The library optimizer (libopt) is a program (python script) that
# creates shared object libraries that contain only the objects actually
# needed for a set of target executables.
# We are using libraryopt-1.0.1 from http://libraryopt.sourceforge.net/
# which was released from Monta Vista Software.
# The script requires a particular layout of tools, configuration files,
# and object files (which is what Monta Vista uses).
# Rather than modify the script, we accomodate it and produce a
# temporary directory $(TOPDIR)/libopt.temp that contains the needed
# files in the required tree.
#
# Prerequisites: the environmental variables that are checked or created
# from the main build/Makefile are used here (without checking).
# For each shared library to be worked on, there must be
# $(TOPDIR)/build/libraryopt/optinfo/<libraryname> subdirectory;
# see $(TOPDIR)/build/libraryopt/optinfo/README.txt for details.
#



LIBOPTINFOSRC=$(TOPDIR)/build/libraryopt/optinfo
LIBOPTSCRIPTSRC=$(TOPDIR)/build/libraryopt/libraryopt-1.0.1

# defined in build/Makefile: LIBOPTTEMP=$(TOPDIR)/build/libopt.temp
# libopt requires target/usr/lib/optinfo below LIBOPTTEMP
LIBOPTTEMPINFO=$(LIBOPTTEMP)/target/usr/lib/optinfo
# TODO FIXME TODO FIXME "mips" is appropriate for only some targets...
# ... others need "mips_nofpu" or ???
LIBOPTTOOLSRC=$(TOPDIR)/build/$(TOOLCHAIN)/build_mips
LIBOPTTOOLSRC2=$(TOPDIR)/build/$(TOOLCHAIN)/toolchain_build_mips/uClibc

libopt_temp : # depends on tools all done!
	rm -rf $(LIBOPTTEMP)
	mkdir -p $(LIBOPTTEMP)
	# libopt expects tools in bin, all with same prefix,
	# including libopt and libindex scripts themselves.
	mkdir -p $(LIBOPTTEMP)/bin
	ln -s $(LIBOPTTOOLSRC)/bin/* $(LIBOPTTEMP)/bin/.
	ln -s $(LIBOPTSCRIPTSRC)/src/libopt $(LIBOPTTEMP)/bin/$(TOOLPREFIX)libopt
	ln -s $(LIBOPTSCRIPTSRC)/src/libindex $(LIBOPTTEMP)/bin/$(TOOLPREFIX)libindex
	# and for our own convenience we build a lib directory with all
	# of the various files we will need
	mkdir -p $(LIBOPTTEMP)/lib
	ln -s $(LIBOPTTOOLSRC)/lib/* $(LIBOPTTEMP)/lib/.
	# libgcc.a is hard to find. E.g. it can be found at:
	#  build/gcc-3.4.4-2.16.1/build_mips/lib/gcc/mips-linux-uclibc/3.4.4/libgcc.a
	ln -s $(LIBOPTTOOLSRC)/lib/gcc/*/*/libgcc.a $(LIBOPTTEMP)/lib/.
	ln -s $(LIBOPTTOOLSRC2)/*/*_so.a $(LIBOPTTEMP)/lib/.
	ln -s $(LIBOPTTOOLSRC2)/*/*/*_so.a $(LIBOPTTEMP)/lib/.
	ln -s $(LIBOPTTOOLSRC2)/lib/interp.os $(LIBOPTTEMP)/lib/.
	mkdir -p $(LIBOPTTEMPINFO)
	set -e ; \
	for sobject in `cd $(INSTALL_ROOT)/lib && echo *.so`; do \
	  if [ -L $(INSTALL_ROOT)/lib/$$sobject ] ; then true; else \
	    echo Looking at $$sobject ... ; \
	    if [ -d $(LIBOPTINFOSRC)/$$sobject ] ; then \
	      echo Creating libopt temp info for shared object file $$sobject ; \
	      cp -a $(LIBOPTINFOSRC)/$$sobject $(LIBOPTTEMPINFO)/. ; \
	      cp -a $(INSTALL_ROOT)/lib/$$sobject $(LIBOPTTEMPINFO)/$$sobject/.;  \
              ln -s $(LIBOPTTEMP)/lib $(LIBOPTTEMPINFO)/$$sobject/required; \
	      (cd $(LIBOPTTEMPINFO)/$$sobject && ./prebuild $(LIBOPTTEMP)/bin/$(TOOLPREFIX)libindex $(LIBOPTTOOLSRC)/lib) ; \
	    else echo Missing $(LIBOPTINFOSRC)/$$sobject ; \
	    fi; \
	  fi; \
        done
	


