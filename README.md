Continued from dwm version 6.2



dwm - dynamic window manager
============================
dwm is an extremely fast, small, and dynamic window manager for X.


Requirements
------------
In order to build dwm you need the Xlib header files.


Installation
------------
Edit config.mk to match your local setup (dwm is installed into
the /usr/local namespace by default).

Afterwards enter the following command to build and install dwm (if
necessary as root):

    make clean install


Running dwm
-----------
Add the following line to your .xinitrc to start dwm using startx:

    exec dwm

In order to connect dwm to a specific display, make sure that
the DISPLAY environment variable is set correctly, e.g.:

    DISPLAY=foo.bar:1 exec dwm

(This will start dwm on display :1 of the host foo.bar.)

In order to display status info in the bar, you can do something
like this in your .xinitrc:

    while xsetroot -name "`date` `uptime | sed 's/.*,//'`"
    do
    	sleep 1
    done &
    exec dwm


Configuration
-------------
The configuration of dwm is done by creating a custom config.h
and (re)compiling the source code.


Patches
-------

- [fullgaps](https://dwm.suckless.org/patches/fullgaps/) - For showing gaps in dwm 6.2.
- [autostart](https://dwm.suckless.org/patches/autostart/) - Autostart applications after startup.
- [systray](https://dwm.suckless.org/patches/autostart/) - Showing system tray to hold icons for applications like nm-applet.
- [restartsig](https://dwm.suckless.org/patches/restartsig/) - Apply changes without quitting dwm.
- [pertag](https://dwm.suckless.org/patches/pertag/) - This patch keeps layout, mwfact, barpos and nmaster separate for each tag.
- [bottomstack with ru_gaps](https://dwm.suckless.org/patches/ru_gaps/) - Same as tile layout but instead of single columns, new windows are attached in single row below master window. ru_gaps adds gaps to it.
---
Scripts used in `config.h` can be found [here](https://github.com/chinmaychhajed/scripts/).
