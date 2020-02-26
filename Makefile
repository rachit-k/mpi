all:	block nonblock collective
OS := $(shell uname)
ifeq ($(OS), Darwin)
	CFLAGS=  
else
	CFLAGS= 
endif
block:
	mpic++ $(CFLAGS) B_P2P.cpp -o block
nonblock:
	mpic++  NB_P2P.cpp -o nonblock
collective:
	mpic++ $(CFLAGS) collective.cpp -o collective
clean: 
	rm -f block nonblock collective check.py *.out