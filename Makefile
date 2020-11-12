.PHONY: all

all: camera.so

clean:
	rm camera.so

camera.so: $(wildcard *.cpp)
	g++ -std=c++14 -shared -Wall -DOPENBW -fPIC $^ -I bwapi/bwapi/include/ -o $@ -lbfd -ldl