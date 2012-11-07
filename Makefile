all: test.cpp
	g++ -std=c++0x -O3 -pthread test.cpp
