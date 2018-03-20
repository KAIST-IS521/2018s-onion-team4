Dockerfile
=====

## Build Docker
```sh
$ docker build -t onion .
```

## Run
```sh
$ docker -v `pwd`:/root/mount run -it onion /bin/bash
```
This command mount pwd to /root/mount.
