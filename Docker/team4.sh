#! /bin/bash
#source ./test.sh

git clone -b proto http://github.com/KAIST-IS521/2018s-onion-team4.git
cd 2018s-onion-team4
make
mkdir /bin/go
go get github.com/zyxar/image2ascii
./OnionMessenger --priv $PRIV --pub $PUB --node $NODE
