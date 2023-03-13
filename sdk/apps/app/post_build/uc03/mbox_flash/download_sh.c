// *INDENT-OFF*
if [ -f $1.bin ];
then rm $1.bin;
fi
if [ -f $1_data.bin ];
then rm $1_data.bin;
fi
if [ -f $1_data1.bin ];
then rm $1_data1.bin;
fi
if [ -f $1.lst ];
then rm $1.lst;
fi

${OBJDUMP} -d -print-imm-hex -print-dbg $1.elf > $1.lst
${OBJSIZEDUMP} -lite -skip-zero -enable-dbg-info ${1}.elf | sort -k 1 >  ${1}.txt
${OBJCOPY} -O binary -j .app_code $1.elf  $1.bin
${OBJCOPY} -O binary -j .data $1.elf  data.bin
${OBJDUMP} -section-headers  $1.elf
cat $1.bin data.bin > app.bin

host-client -project ${NICKNAME} -mode flash_debug -f app.bin $1.elf isd_config.ini uboot.boot uc03loader.bin uc03loader.uart ota.bin ota_debug.bin
