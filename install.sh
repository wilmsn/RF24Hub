#!/bin/sh
#######################################
#
# Installskript for sensorhub deamon
#
#######################################
ID=`id | cut -d "=" -f 2 | cut -d "(" -f 1`
if [ $ID = 0 ]; then
  if [ -e /etc/init.d/rf24hub ]; then
    /etc/init.d/rf24hub stop
    rm /etc/init.d/rf24hub
  fi
  if [ -e /etc/systemd/system/rf24hub.service ]; then
    echo "Old installation found!"
    echo "Cleanup"
    echo "Stop rf24hub (if installed and running)"
    systemctl stop rf24hub    
    cp rf24hub.service /etc/systemd/system/
  fi
  if [ ! -e /etc/rf24hub/rf24hub.cfg ]; then
    if [ ! -d /etc/rf24hub ]; then
      mkdir /etc/rf24hub
    fi
    cp rf24hubd.cfg /etc/rf24hub/rf24hub.cfg
    echo "Please edit config template: /etc/rf24hub/rf24hub.cfg"
    echo "and run \"sudo systemctl stop rf24hub; sudo systemctl start rf24hub\" after that"
  fi
  if [ ! -e /etc/rf24hub/rf24hub.cfg ]; then
    if [ ! -d /etc/rf24hub ]; then
      mkdir /etc/rf24hub
    fi
    cp rf24hubd.cfg /etc/rf24hub/rf24hub.cfg
    echo "Please edit config template: /etc/rf24hub/rf24hub.cfg"
    echo "and run \"sudo service rf24hub restart\" after that"
  fi
  echo "Install as User Root";
  echo "copy rf24hubd ==> /usr/local/bin/rf24hubd"
  cp rf24hubd /usr/local/bin/rf24hubd
  echo "  rf24hub.service ==> /etc/systemd/system/rf24hub.service"
  cp ./rf24hub.service /etc/systemd/system/rf24hub.service
  chown root:root /etc/systemd/system/rf24hub.service
  chmod 444 /etc/systemd/system/rf24hub.service
  systemctl daemon-reload
  echo "Start it up"
  systemctl start rf24hub
  ps -efa | grep rf24hubd | grep -v grep
else
  echo "Error: not root ==> Please use user root"
fi
