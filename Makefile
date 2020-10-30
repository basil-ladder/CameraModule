.PHONY: all

all: camera.so

camera.so: $(wildcard *.cpp)
	g++ -shared -Wall -O3 -DOPENBW -fPIC $^ -I ~/openbw/bwapi/bwapi/include/ -o $@