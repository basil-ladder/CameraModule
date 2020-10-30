.PHONY: all

all: camera.so

clean:
	rm camera.so

camera.so: $(wildcard *.cpp)
	g++ -std=c++14 -shared -Wall -O3 -DOPENBW -fPIC $^ -I bwapi/bwapi/include/ -o $@