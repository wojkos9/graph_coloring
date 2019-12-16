CXX=g++
PATH=C:\x64-4.8.1-release-posix-sjlj-rev5\mingw64\bin
CFLAGS=-std=c++11 -g

ifeq ($(OS), Linux)
	CFLAGS += -pthread
endif

main:
	$(CXX) -omain src/main.cpp src/debug_stream/debug_stream.cpp $(CFLAGS)