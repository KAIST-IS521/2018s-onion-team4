Dockerfile
=====

## Build Docker
```sh
$ docker build -t onion .
```

## Run
```sh
$ docker run -v `pwd`:/root/mount -e OPT="[program argument]" -i -t onion
```
This command mount pwd to /root/mount.
