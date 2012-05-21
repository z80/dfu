@echo off

rem OpenOCD\bin\openocd.exe -d3 -f interface/jtagkey.cfg -f target/stm32.cfg -c init

start OpenOCD\bin\openocd.exe -d1 -f interface/jtagkey2.cfg -f target/stm32.cfg -c init -c "target_request debugmsgs enable"
start telnet localhost 4444

rem set HEXFILE=FLASH_RUN/project.hex
rem OpenOCD\bin\openocd.exe -d0 -f interface/jtagkey.cfg -f target/stm32.cfg -c init -c "reset halt" -c "flash write_image erase %HEXFILE%" -c "verify_image %HEXFILE%" -c "reset run" -c shutdown
rem pause