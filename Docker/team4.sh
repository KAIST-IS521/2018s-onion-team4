#! /bin/bash
#source ./test.sh
git clone -b proto http://github.com/KAIST-IS521/2018s-onion-team4.git
cd 2018s-onion-team4
make
mkdir /bin/go
go get github.com/stdupp/goasciiart
echo $PRIV;
if [ $NODE ];
  then ./OnionMessenger --priv $PRIV --pub $PUB --node $NODE -p $PORT
  else ./OnionMessenger --priv $PRIV --pub $PUB -p $PORT
fi
