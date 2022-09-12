SRCS := $(wildcard src/*.c) $(wildcard src/*.cpp)

.PHONY bin:
	g++ -pthread -Isrc -O3 -fPIC -o zsmplayer $(SRCS) -ldl
