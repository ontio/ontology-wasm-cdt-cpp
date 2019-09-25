FROM ubuntu:18.04

MAINTAINER steven

RUN apt update && apt -y install python3 vim wget cmake openssl && echo "export PATH=$PATH:/ontowasm/bin" >> /root/.bashrc && cd /root/

RUN git clone https://github.com/ontio/ontology-wasm-cdt-cpp && cd ontology-wasm-cdt-cpp && mkdir -p build && cd build && cmake -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_INSTALL_PREFIX="/ontowasm/bin" .. && make install
