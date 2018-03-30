#! /bin/bash
git clone -b proto http://github.com/KAIST-IS521/2018s-onion-team4.git
cd 2018s-onion-team4
make
go get github.com/stdupp/goasciiart
./OnionMessenger $OPT
