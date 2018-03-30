FROM ubuntu:xenial
MAINTAINER 2018s-IS561-Team4

### Packages setup
RUN DIST=xenial && \
    sed -i 's/deb.debian.org/ftp.daumkakao.com/' /etc/apt/sources.list && \
    apt-get update && \
    apt-get install -y --no-install-recommends --no-install-suggests \
    make git libgpgme11-dev libncurses-dev g++ libcurl4-openssl-dev \
    alsa-utils
RUN apt-get install -y ca-certificates
RUN apt-get install -y golang-go
RUN apt-get install -y apt-utils
ADD team4.sh /root/team4.sh
RUN chmod +x /root/team4.sh
RUN rm -rf /var/lib/apt/lists/* && \
    apt-get clean


WORKDIR /root
ENV GOPATH /root
ENV PATH $PATH:$GOPATH/bin
CMD ["/root/team4.sh"]
