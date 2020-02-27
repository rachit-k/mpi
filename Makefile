all:	block nonblock collective blockO nonblockO collectiveO
OS := $(shell uname)
ifeq ($(OS), Darwin)
	CFLAGS=  -O1
else
	CFLAGS= -O1
endif
block:
	mpic++ B_P2P.cpp -o block
blockO:
	mpic++ $(CFLAGS) B_P2P.cpp -o blockO
nonblock:
	mpic++ NB_P2P.cpp -o nonblock
nonblockO:
	mpic++ $(CFLAGS) NB_P2P.cpp -o nonblockO
collective:
	mpic++ collective.cpp -o collective
collectiveO:
	mpic++ $(CFLAGS) collective.cpp -o collectiveO
clean: 
	rm -f block nonblock collective blockO nonblockO collectiveO check.py *.out