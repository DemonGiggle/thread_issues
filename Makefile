all: test.cpp other.cpp
	g++ -std=c++0x -g -pthread test.cpp other.cpp
