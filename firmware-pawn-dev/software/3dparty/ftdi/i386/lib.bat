mkdir ftdi\i386\gcc
pexports.exe ftdi/i386/ftd2xx.dll > ftdi/i386/gcc/_ftd2xx.def
dlltool.exe --def ftdi/i386/gcc/_ftd2xx.def --dllname ftd2xx.dll --output-lib ftdi/i386/gcc/libftd2xx.a
