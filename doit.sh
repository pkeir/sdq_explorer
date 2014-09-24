#!/bin/bash

DAPHNE="DAPHNE: First Ever Multiple Arcade Laserdisc Emulator =]"

# XID holds an X window identifier
#XID=`xdotool getactivewindow`
#XID=`xdotool search --name "DAPHNE: First Ever Multiple Arcade Laserdisc Emulator =]"`
#XID=`xdotool search --limit 1 --name "sdq_grab"`   # Likely this directory.
#XID=`xdotool search --limit 1 --name "scrot"`   # The scrot directory window. 
#XID=`xdotool search --limit 1 --name "DAPHNE"`   # The scrot directory window. 
XID=$(xdotool search --name "$DAPHNE")
#echo $XID

# g++ sdq_grab.cpp -lX11 -o sdq_grab
./sdq_grab $XID

