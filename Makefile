SRCS := $(filter-out src/x16sound.c,$(wildcard src/*.c)) $(wildcard src/*.cpp)

zsmplayer: src/x16sound.o $(SRCS)
	g++ -pthread -Isrc -O3 -fPIC -o zsmplayer $(SRCS) src/x16sound.o -ldl

winplayer: $(SRCS)
	i686-w64-mingw32-g++ -static -DNOSLEEP -L/usr/i686-w64-mingw32/lib/ -pthread -Isrc -O3 -fPIC -o zsmplayer.exe $(SRCS) src/x16sound.c

src/x16sound.o: src/x16sound.c
	@$(MKDIR_P) $(dir $@)
	g++ -Isrc -O3 -fPIC -o $@ -c $<

.PHONY src/x16sound.c:

MKDIR_P ?= mkdir -p
