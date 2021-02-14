#!/bin/bash

while true ; do 
  cat irssi-screen-screen1.esc
  echo -en '\033[24;1H\033[41;34mFrom\033[0m'
  read
  cat irssi-screen-screen1.esc irssi-screen-screen2.esc
  echo -en '\033[24;1H\033[41;34mTo  \033[0m'
  read
done
