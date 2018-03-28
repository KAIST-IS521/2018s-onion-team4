#! /bin/bash
git clone -b proto http://github.com/KAIST-IS521/2018s-onion-team4.git
cd 2018s-onion-team4
make
mkdir /bin/go
go get github.com/stdupp/goasciiart
a=0
if [ $NODE ];
	then a=`expr $a + 1`
fi
if [ $NOTUI ];
	then a=`expr $a + 2`
fi
if [ $HELP ];
	then ./OnionMessenger --help
elif [ $a = 0 ];
	then ./OnionMessenger --priv $PRIV --pub $PUB -p $PORT
elif [ $a = 1 ];
	then ./OnionMessenger --priv $PRIV --pub $PUB --node $NODE -p $PORT
elif [ $a = 2 ];
	then ./OnionMessenger --priv $PRIV --pub $PUB -p $PORT --notui
elif [ $a = 3 ];
  then ./OnionMessenger --priv $PRIV --pub $PUB -p $PORT --notui --node $NODE
fi
