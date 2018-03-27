Dockerfile
=====

## Build Docker
```sh
$ docker build -t onion .
```

## Run
```sh
$ docker run -v `pwd`:/root/mount -it onion
#docker run -v c:\Users\sayki\Desktop\is521_hw\2018s-onion-team4\Docker:/root/mount -e PRIV=/root/mount/saykim0727.pri -e PUB=/root/mount/saykim0727.pub -i -t onion
```
This command mount pwd to /root/mount.
