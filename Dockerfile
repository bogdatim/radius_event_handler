FROM debian:jessie
MAINTAINER Tim <dead-cell@yandex.ru>

ENV DEBIAN_FRONTEND noninteractive
ENV HOME /root

WORKDIR /root

RUN apt-get update && apt-get install -y git build-essential libboost-all-dev vim net-tools make g++

RUN git clone https://github.com/bogdatim/radius_event_handler.git && cd radius_event_handler && \
    tar zxvf tbb-2017_U7.tar.gz && cd radius_event_handler && \
    make -f Makefile CONF=Debug clean && \
    make -f Makefile CONF=Debug

ENV LD_LIBRARY_PATH /root/radius_event_handler/tbb-2017_U7/build/linux_intel64_gcc_cc4.9.2_libc2.19_kernel3.16.0_release
ENV PATH=$PATH:/root/radius_event_handler/radius_event_handler/dist/Debug/GNU-Linux

EXPOSE 30001 7777 5555
WORKDIR /root/radius_event_handler/radius_event_handler/dist/Debug/GNU-Linux
ENTRYPOINT ["radius_event_handler", "--id=777", "--own-addr=127.0.0.1:30001", "--thrift-addr=10.10.10.10:7777"]
