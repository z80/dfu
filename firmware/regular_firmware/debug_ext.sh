#!/bin/sh
PATH=$PATH:~/programs/code-sourcery/2011.03-42/bin
ddd --eval-command "target remote 192.168.1.82:61234" --debugger arm-none-eabi-gdb ./target/target.elf
