#!/bin/sh -x

#
# $1 == u-boot/tools path
# $2 == kernel tree path
# $3 == optional additions to filename
#
MKIMAGE=$1/mkimage
VMLINUX=$2/vmlinux
VMLINUXBIN=$2/arch/mips/boot/vmlinux.bin
# NOTE You can direct the outputs elsewhere by pre-defining TFTPPATH
if [ -z "$TFTPPATH" ]
then
    TFTPPATH=/tftpboot/`whoami`
fi
echo $0 Using TFTPPATH=$TFTPPATH  ###### DEBUG

TFTPPATH=$PWD/../../tftpboot/`whoami`

ENTRY=`readelf -a ${VMLINUX}|grep "Entry"|cut -d":" -f 2`
LDADDR=`readelf -a ${VMLINUX}|grep "\[ 1\]"|cut -d" " -f 26`

./lzma e ${VMLINUXBIN} ${VMLINUXBIN}.lzma
  
${MKIMAGE} -A mips -O linux -T kernel -C lzma \
        -a 0x${LDADDR} -e ${ENTRY} -n "Linux Kernel Image"    \
                -d ${VMLINUXBIN}.lzma ${IMAGEPATH}/vmlinux$3.lzma.uImage
cp ${IMAGEPATH}/vmlinux$3.lzma.uImage ${TFTPPATH}

${MKIMAGE} -A mips -O linux -T kernel -C gzip \
        -a 0x${LDADDR} -e ${ENTRY} -n "Linux Kernel Image"    \
                -d ${VMLINUXBIN}.gz ${IMAGEPATH}/vmlinux$3.gz.uImage
cp ${IMAGEPATH}/vmlinux$3.gz.uImage ${TFTPPATH}
