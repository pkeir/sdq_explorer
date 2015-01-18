#!/bin/bash

XDOTOOL=$HOME/code/ext/xdotool
CPLUS_INCLUDE_PATH=$XDOTOOL:$CPLUS_INCLUDE_PATH
      LIBRARY_PATH=$XDOTOOL:$LIBRARY_PATH
   LD_LIBRARY_PATH=$XDOTOOL:$LD_LIBRARY_PATH


IMLIB2=$HOME/code/ext/imlib2-1.4.6/install
CPLUS_INCLUDE_PATH=$IMLIB2/include:$CPLUS_INCLUDE_PATH
      LIBRARY_PATH=$IMLIB2/lib:$LIBRARY_PATH
   LD_LIBRARY_PATH=$IMLIB2/lib:$LD_LIBRARY_PATH

export CPLUS_INCLUDE_PATH
export LIBRARY_PATH
export LD_LIBRARY_PATH