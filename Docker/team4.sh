#! /bin/bash
#source ./test.sh

git clone -b proto https://github.com/KAIST-IS521/2018s-onion-team4.git
cd 2018s-onion-team4
make
./OnionMessenger --priv $PRIV --pub $PUB --node $NODE