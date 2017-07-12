CC      = g++
CFLAGS  = 
LDFLAGS = -lboost_system -lboost_thread -lboost_filesystem  -lboost_system

all: radMsgSender clean

radMsgSender: udp_client.o
	$(CC) -o $@ $^ $(LDFLAGS)

udp_client.o: udp_client.cpp udp_client.hpp
	$(CC) -c $(CFLAGS) $<

.PHONY: clean cleanest

clean:
	rm *.o

cleanest: clean
	rm radMsgSender
