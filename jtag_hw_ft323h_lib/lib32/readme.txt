The static library jtag_client.lib corresponds to the dynamic library jtag_client.dll version 13.1.
The names of the exported functions follow the style (C++ decoration) of the MS compilers and are therefore not compatible.
with other compilers. The executable file with this library is built only in MSVS.
To use the MinGW or Cygwin compilers, you need to create your own static library
with ordinal import or use dynamic loading and linking using ( LoadLibrary(), GetProcAddress() ).
