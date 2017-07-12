#!/bin/sh

cd radius_event_handler
/usr/bin/make -f Makefile CONF=Debug clean
/usr/bin/make -f Makefile CONF=Debug
export LD_LIBRARY_PATH=:../../tbb-2017_U7/build/linux_intel64_gcc_cc4.9.2_libc2.19_kernel3.16.0_release
dist/Debug/GNU-Linux/radius_event_handler --id=777 --own-addr=127.0.0.1:30001 --thrift-addr=10.10.10.10:7777



