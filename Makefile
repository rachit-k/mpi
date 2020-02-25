all:	NB_P2P NB_P2P collective
OS := $(shell uname)
ifeq ($(OS), Darwin)
	CFLAGS= -Xpreprocessor -fopenmp -pthread 
else
	CFLAGS= -Wall -fopenmp -pthread
endif
B_P2P:
	g++ $(CFLAGS) B_P2P.cpp -o B_P2P
NB_P2P:
	g++  NB_P2P.cpp -o NB_P2P
collective:
	g++ $(CFLAGS) collective.cpp -o collective
clean: 
	rm -f NB_P2P NB_P2P collective check.py *.out