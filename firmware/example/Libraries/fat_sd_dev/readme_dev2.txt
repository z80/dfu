April 4, 2010  FatFs R0.08.dev.2


1. API: Changed argument type name.

  The data type XCHAR was used for the path name on the API.
  It has been renamed to TCHAR which is usually used on the
  Win32 platforms.



2. Unicode: Changed short file name type

  Changed fname member in the FILINFO structure from char to TCHAR.
  Also the short file name is returned in Unicode string when the FatFs
  is configured to Unicode API.



3. Unicode: String functions can handle UTF-8 encoding text.

  When the FatFs is configured to Unicode API, the string functions
  f_gets, f_putc, f_puts and f_prinrf input/output the text file in
  UTF-8 encoding. The text encoding on the read/write buffer is
  UCS-2 (TCHAR).

