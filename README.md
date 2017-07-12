# radius_event_handler - an RADIUS event receiver and handler
It receives RADIUS accounting messages (rfc2866) on UDP socket, extracts all AVPs and sends them to external server other UDP (server is not implemented yet).
For program building pull the master branch and unzip TBB library.
After that execute "run.sh" script and it will build and run "radius_event_handler" program
There is also udp_client program in corresponding folder. It can be used to generate two synthetic hardcoded RADIUS messages.
To build the udp_client program just run make and after that execute it without parameters.
