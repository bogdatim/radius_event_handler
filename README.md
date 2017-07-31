# radius_event_handler - RADIUS event receiver and handler
It receives RADIUS accounting messages (rfc2866) on UDP socket, extracts all AVPs and sends them to external server other UDP (server is not implemented yet).
Note: The program can be built and run on Linux 64 bit OS only!

For program building:
1) Pull the master branch and unzip TBB library.
2) Install boost library: sudo apt-get install libboost-all-dev
3) Execute "run.sh" script and it will build and run "radius_event_handler" program.

There is also udp_client program in corresponding folder. It can be used to generate two synthetic hardcoded RADIUS messages.
To build the udp_client program just run make and after that execute it without parameters.

Instruction on running docker image:
1) Build docker image using Dockerfile from the project.
2) Create and run container:

    sudo docker run --net=host -t -d --name reh tim/radius_event_handler radius_event_handler --id=777 --own-addr=127.0.0.1:30001 --thrift-addr=127.0.0.1:7777
3) If you want to access running container through Host interfaces add "--net=host" option to "docker run" command. It will link container ports with host ports:

    sudo docker run --net=host -t -d --name reh tim/radius_event_handler radius_event_handler --id=777 --own-addr=127.0.0.1:30001 --thrift-addr=127.0.0.1:7777

