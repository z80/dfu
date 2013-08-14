#!/bin/sh
PATH=$PATH:~/programs/code-sourcery/2011.03-42/bin:~/projects/stlink.git/stlink-texane/gdbserver
st-util -s 2 -p 4242 &
#arm-none-eabi-gdbtui --eval-command "target remote :4242" ./target/target.elf
ddd --eval-command "target remote :4242" --debugger arm-none-eabi-gdb ./build/ch.elf
