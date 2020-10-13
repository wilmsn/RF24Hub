#!/bin/sh
#######################################
#
# Installskript for sensorhub deamon
#
#######################################
ID=`id | cut -d "=" -f 2 | cut -d "(" -f 1`
if [ $ID = 0 ]; then
  if [ ! -e /etc/systemd/system/appl.target ]; then
    cp appl.target /etc/systemd/system
    ln -sf /etc/systemd/system/appl.target /etc/systemd/system/default.target
    systemctl isolate appl.target
  fi
  if [ ! -d /etc/systemd/system/appl.target.wants ]; then
    mkdir /etc/systemd/system/appl.target.wants
  fi
  if [ -e /etc/systemd/system/rf24gw.service ]; then
    echo "Old installation found!"
    echo "Cleanup"
    echo "Stop rf24gw (if installed and running)"
    systemctl stop rf24gw
    if [ -e /etc/systemd/system/multi-user.target.wants/rf24gw.service ]; then
      rm /etc/systemd/system/multi-user.target.wants/rf24gw.service
    fi
  fi
  if [ ! -e /etc/rf24hub/rf24hub.cfg ]; then
    if [ ! -d /etc/rf24hub ]; then
      mkdir -p /etc/rf24hub
    fi
    cp rf24hub.cfg /etc/rf24hub/rf24hub.cfg
    echo "Please edit config template: /etc/rf24hub/rf24hub.cfg"
    echo "and run \"sudo systemctl stop rf24hub; sudo systemctl start rf24hub\" after that"
  fi
  echo "Install as User Root";
  echo "copy rf24gwd ==> /usr/local/bin/rf24gwd"
  cp rf24gwd /usr/local/bin/rf24gwd
  echo "  rf24gw.service ==> /etc/systemd/system/rf24gw.service"
  cp ./rf24gw.service /etc/systemd/system/rf24gw.service
  ln -sf /etc/systemd/system/rf24gw.service /etc/systemd/system/appl.target.wants/rf24gw.service
  chown root:root /etc/systemd/system/rf24gw.service
  chmod 444 /etc/systemd/system/rf24gw.service
  systemctl daemon-reload
  echo "Start it up"
  systemctl start rf24gw
  ps -efa | grep rf24gwd | grep -v grep
else
  echo "Error: not root ==> Please use user root"
fi
