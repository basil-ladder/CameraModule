.PHONY: all

all: camera.so

camera.so: $(wildcard *.cpp)
	g++ -shared -fPIC $^ -I ~/openbw/bwapi/bwapi/include/ -o $@