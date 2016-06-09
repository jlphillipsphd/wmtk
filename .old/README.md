wmtk
====

Robot-PFC Working Memory Toolkit

****************************************************
Basic Installation for most flavors of Unix (Linux):
****************************************************

./configure
make
make install

(This places all headers in /usr/local/include/wmtk and the libraries in
/usr/local/lib. Use the --prefix option to place these in some other
location, type ./configure --help to find out more.)


Most Common Compilation Flags:

-I/usr/local/include/wmtk -L/usr/local/lib -lWMtk

Remember to set appropriate environment variables to use the shared library.
(However, /usr/local/lib is included by default on most GNU Linux systems.)

LD_LIBRARY_PATH=/usr/local/lib

Inside source.cpp: \#include \<WMtk.h\>

********************************************
Building the WMtk in Windows using VC++ 6.0:
********************************************

(Since the toolkit was not developed on a Windows platform, I can only
provide the following help so that you can perform the port yourself.
The code checks some Windows specific flags to make this fairly easy, but
we cannot guarantee that it will compile on your particular machine.)

1. Create a new project using the Win32 Static Library template.

2. Add all .h and .cpp files in the src/ directory of this archive
   to the project.

3. Find the Project -> Settings dialog and click on the C/C++ tab.

4. Add the current directory to the search include path (something like
   the following in the bottom box): /I"project directory"

5. Build the library. (You're done, but here's a little more help:)

6. To use the library in future projects, first just add the include path as
   described above to the new project.

7. Copy the library (WMtk.LIB) from the WMtk project/debug directory to the
   new project's main directory, and add the library file to the project.
   (This works exactly the same as adding source files to a project.)

8. Write and build your project as normal. (Remember to #include <WMtk.h>
   in any sources that use the toolkit.)


