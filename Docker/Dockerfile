FROM ubuntu:xenial
MAINTAINER 2018s-IS561-Team4

### Packages setup
RUN DIST=xenial && \
    sed -i 's/deb.debian.org/ftp.daumkakao.com/' /etc/apt/sources.list && \
    apt-get update && \
    apt-get install -y --no-install-recommends --no-install-suggests \
    make git libgpgme11-dev libncurses-dev g++ libcurl4-openssl-dev \
    apt-get install -y golang-go \
    apt-get install -y libcurl4-openssl-dev &&\
    apt-get install -y libcurl4-gnutls-dev \
    alsa-utils
RUN mkdir /bin/go
RUN export GOPATH=/bin/go
RUN export PATH=$PATH:$GOPATH/bin
RUN go get github.com/zyxar/image2ascii

RUN rm -rf /var/lib/apt/lists/* && \
    apt-get clean

WORKDIR /root

CMD ["/bin/team4_sh"]
###sudo docker run -e PRIV="priv path" -e PUB="pub path" -e NODE="node ip"
