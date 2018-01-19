#!/bin/sh
# script to build a binary IPL image for the J6EVM board
set -v  

# Convert CH header into Binary format
${QNX_HOST}/usr/bin/ntoarmv7-objcopy --input-format=elf32-littlearm --output-format=binary ../src/hardware/ipl/boards/dra74x/arm/vayu-evm.le.v7/ch_header.o ./tmp-ch-header.bin

# Convert IPL into Binary format
${QNX_HOST}/usr/bin/ntoarmv7-objcopy --input-format=elf32-littlearm --output-format=binary -R.tlb -R.scratch ../src/hardware/ipl/boards/dra74x/arm/vayu-evm.le.v7/ipl-dra74x-vayu-evm ./tmp-ipl-dra74x-vayu-evm.bin

# Cat boot header and ipl together
cat ./tmp-ch-header.bin ./tmp-ipl-dra74x-vayu-evm.bin > ./ipl-dra74x-vayu-evm.bin

# clean up temporary files
rm -f tmp*.bin

echo "done!!!!!!!"
