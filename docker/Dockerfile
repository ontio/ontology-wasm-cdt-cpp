FROM ubuntu:18.04

MAINTAINER steven

RUN apt update && apt -y install python3 vim wget cmake openssl clang-8 llvm-8 && cd /root/

RUN git clone --recursive https://github.com/ontio/ontology-wasm-cdt-cpp && cd ontology-wasm-cdt-cpp && bash compiler_install.bash && echo "export PATH=$PATH:$(pwd)/install/bin" >> /root/.bashrc
