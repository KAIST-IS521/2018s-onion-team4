Dockerfile
=====

## Build Docker
```sh
$ docker build -t onion .
```

## Run
```sh
$ docker run -v `pwd`:/root/mount -e OPT="--priv /root/mount/[user].priv --pub /root/mount/[user].pub -p [port_num]" -i -t onion
#docker run -v /home/kim/2018s-onion-team4/Docker:/root/mount -e PRIV=/root/mount/saykim0727.pri -e PUB=/root/mount/saykim0727.pub -e PORT=1234 -i -t onion
```
This command mount pwd to /root/mount.
