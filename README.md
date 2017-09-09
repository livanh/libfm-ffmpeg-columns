#libfm ffmpeg modules

Modules for pcmanfm/libfm to show informations about audio and video files
(obtained with ffmpeg libraries) in additional columns.

Requires libfm-gtk-dev, libavcodec-dev, libavformat-dev, and libavutil-dev
for building, e.g. on Debian-based distros use:
```
$ sudo apt-get install libfm-gtk-dev libavcodec-dev libavformat-dev libavutil-dev
```
To build and install, use:
```
$ make
$ sudo make install
```
Modules are installed under /usr/lib/x86_64-linux-gnu, which is correct
for multiarch 64-bit installations. For 32-bit installation change LIBDIR
to /usr/lib/i386-linux-gnu in the Makefile. For other architectures
change as appropriate.

### Contacts
<livanh@bulletmail.org>