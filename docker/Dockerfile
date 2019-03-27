FROM ubuntu:18.04

MAINTAINER steven

ADD https://github.com/ontio/ontology-wasm-cdt-cpp/blob/master/pkg/ontwasm-ubuntu18.04-amd64.deb?raw=true /root/

RUN apt update && apt -y install python3 vim wget && echo "export PATH=$PATH:/ontowasm/bin" >> /root/.bashrc && cd /root/ && dpkg -i ontwasm-ubuntu18.04-amd64.deb
