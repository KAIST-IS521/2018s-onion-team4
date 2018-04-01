#!/bin/bash
if [ "$1" -eq "1" ]; then
  docker rm -f n1
  docker run --name n1 -v `pwd/keys`:/root/mount -e OPT="--priv /root/mount/dummy1.pri --pub /root/mount/dummy1.pub -p 1234" -i -t onion
elif  [ "$1" -eq "2" ]; then
  docker rm -f n2
  IP=$(docker inspect -f "{{ .NetworkSettings.IPAddress }}" n1)
  docker run --name n2 -v `pwd/keys`:/root/mount -e OPT="--priv /root/mount/dummy2.pri --pub /root/mount/dummy2.pub -p 1234 -n $IP:1234" -i -t onion
elif  [ "$1" -eq "3" ]; then
  docker rm -f n3
  IP=$(docker inspect -f "{{ .NetworkSettings.IPAddress }}" n1)
  docker run --name n3 -v `pwd/keys`:/root/mount -e OPT="--priv /root/mount/dummy3.pri --pub /root/mount/dummy3.pub -p 1234 -n $IP:1234" -i -t onion
elif  [ "$1" -eq "4" ]; then
  docker rm -f n4
  IP=$(docker inspect -f "{{ .NetworkSettings.IPAddress }}" n1)
  docker run --name n4 -v `pwd/keys`:/root/mount -e OPT="--priv /root/mount/dummy4.pri --pub /root/mount/dummy4.pub -p 1234 -n $IP:1234" -i -t onion
elif  [ "$1" -eq "5" ]; then
  docker rm -f n5
  IP=$(docker inspect -f "{{ .NetworkSettings.IPAddress }}" n1)
  docker run --name n5 -v `pwd/keys`:/root/mount -e OPT="--priv /root/mount/dummy5.pri --pub /root/mount/dummy5.pub -p 1234 -n $IP:1234" -i -t onion
fi
