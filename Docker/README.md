Dockerfile
=====

##Requirement
```sh
$ mv [user_pub_key_path] [project_path]/Docker
$ mv [user_priv_key_path] [project_path]/Docker
# take your priv_key and pub_key to Docker directory

$ chmod +x team4.sh
```

## Build Docker
```sh
$ docker build -t onion .
```

## Run
```sh
$ docker run -v `pwd`:/root/mount -e OPT="[program argument]" -i -t onion
```
This command mount pwd to /root/mount.
