#!/bin/bash

# Remove module
sudo rmmod test_module
ret=$?
if [ $ret -ne 0 ]; then
  "Removing module failed, quitting"
  exit
fi

# Recompile
sudo make
ret=$?
if [ $ret -ne 0 ]; then
  "Making module failed, quitting"
  exit
fi

sudo insmod test_module.ko