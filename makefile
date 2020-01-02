CXX=g++
CFLAGS=-std=c++11 -g

ifeq ($(OS), Windows_NT)
	PATH=C:\x64-4.8.1-release-posix-sjlj-rev5\mingw64\bin
else
	CFLAGS += -pthread
endif

main: src/*
	$(CXX) -omain src/main.cpp src/debug_stream/debug_stream.cpp $(CFLAGS)