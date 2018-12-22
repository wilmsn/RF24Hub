#!/bin/sh
#######################################
#
# Installskript for sensorhub deamon
#
#######################################
ID=`id | cut -d "=" -f 2 | cut -d "(" -f 1`
if [ $ID = 0 ]; then
  if [ -e /etc/init.d/rf24hub ]; then
    echo "Old installation found!"
    echo "Cleanup"
    echo "Stop rf24hub (if installed and running)"
    /etc/init.d/rf24hub stop
    rm /etc/init.d/rf24hub
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
  echo "  init_rf24hub ==> /etc/init.d/rf24hub"
  cp ./init_rf24hub /etc/init.d/rf24hub
  chown root:root /etc/init.d/rf24hub
  chmod 755 /etc/init.d/rf24hub
  echo "Register it:  update-rc.d rf24hub defaults"
  update-rc.d rf24hub defaults
  echo "Start it up"
  /etc/init.d/rf24hub start
  ps -efa | grep rf24hubd | grep -v grep
else
  echo "Error: not root ==> Please use user root"
fi
