

# # List all object files here
OFILES = \
         CdsDidlSerializer.o \
         CdsErrors.o \
         CdsMediaClass.o \
         CdsObject.o \
         FileIoAbstraction.o \
         DlnaHttp.o \
         DlnaHttpClient.o \
         DlnaHttpServer.o \
         DLNAProtocolInfo.o \
         MediaServerAbstraction.o \
         DownloadController.o \
         DmsIntegration.o \
         FileSystemEnumerator.o \
         MimeTypes.o \
         UTF8Utils.o \
         Main.o \
         ILibParsers.o\
         ILibAsyncSocket.o\
         ILibAsyncUDPSocket.o\
         ILibAsyncServerSocket.o\
         ILibWebClient.o\
         ILibWebServer.o\
         ILibThreadPool.o\
         MediaServer_MicroStack.o\

# Compiler command name
#CC = gcc

# Compiler flags applied to all files: -DINCLUDE_FEATURE_PLAYSINGLE
CFLAGS =  -I./ -I../  -I./CdsObjects  -I./FileAbstractionFiles  -I./HttpFiles  -I./ProtocolInfoParser  -I./MediaServerAbstraction  -I./DownloadController  -I./DmsIntegration  -I./DmsIntegration/FileSystemEnumerator  -I./StringUtils -Os -Wall -D_POSIX -D_DEBUG -D_VERBOSE
# Gemtek +++ 20100115 --- for Linux large file support: DMS cannot show media file with size over 2GB
CFLAGS += -D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64
#CFLAGS	+= -DGTK_AVOID_UPnP_CTT_CRASH
#CFLAGS	+= -DINCLUDE_FEATURE_UPLOAD -DGTK_UPLOAD_FEATURE -DGTK_CTT_SOL
#CFLAGS	+= -DGTK_SPEEDUP_UPnP_TEST
CFLAGS	+= $(PATHNVRAM)

LIBS = -lpthread -lm
LIBS	+= $(LIBNVRAM)
# Name of the executable
VPATH =  ./ ../  ./CdsObjects  ./FileAbstractionFiles  ./HttpFiles  ./ProtocolInfoParser  ./MediaServerAbstraction  ./DownloadController  ./DmsIntegration  ./DmsIntegration/FileSystemEnumerator  ./StringUtils
EXENAME = DMS

# Builds all object files and executable

all: clean dms

dms: $(OFILES)
	$(CC) $(CFLAGS) -o ${EXENAME} $(OFILES) $(LIBS)

# Macro rule for all object files.
$(OFILES) : \
        MediaServer_MicroStack.h\


ifeq ($(GMTK_DEVFLAG_NFS),Y)

romfs:
	mkdir -p $(NFSROOTDIR)/bin
	cp DMS $(NFSROOTDIR)/bin/
        
else

romfs:
	mkdir -p $(ROMFSDIR)/bin
	cp DMS $(ROMFSDIR)/bin

endif


# Clean up
clean :
#	rm -f $(OFILES) ${EXENAME} ${EXENAME}.exe *.bak
	rm -f ${EXENAME} ${EXENAME}.exe *.bak
	find . -name *.bak | xargs rm -rf 
