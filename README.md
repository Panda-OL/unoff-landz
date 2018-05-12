Unoff-Landz (Rebirth)
======================
An Open Source Server for the Eternal Lands/Other-Life Clients
======================

### Note from Panda ###
This is purely a side project of mine to delve more into the C language. I've never attempted anything this big, but due to having some time on my hands, and a wonderful source to work from (thanks the:ebul:munt!) my hopes are pretty high. As with any project, you need some goals, these are the 'overrall' goals I would like to achieve for the source, while learning everything I can along the way:
- Finish of NPCs / Attribute system
- Simple combat + mixing system implemented
- Support PROPER map changes.

These goals may update, or grow larger depending on how much time I throw at it!


## Building the project
I'd definitely recommend using Ubuntu for this project. It's pretty messy when trying to compile under Windows.

### Pre-Requisites
    Latest GCC/C-Lang compiler
    LibEV  
    LibSqlite3 

### Linux - CMake
The build process after acquiring/installing those should be straightforward:

```bash
    cd <cloned_repository>
    mkdir build_dir
    cd build_dir
    cmake ..
    make
```
To run the server, first copy the 'lists' directly into the new "build_dir\Server" folder then
``` bash
    ./unoff -C
    ./unoff -S
```
Change your server.lsts file in your ~.olc to point to 127.0.0.1 or Your IP.

If you want to know more/need some help with setting this up, message PandemiC on OL/Forums.
