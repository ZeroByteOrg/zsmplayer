SRCS := $(filter-out src/x16sound.c,$(wildcard src/*.c)) $(wildcard src/*.cpp)

zsmplayer: src/x16sound.o $(SRCS)
	g++ -pthread -Isrc -O3 -fPIC -o zsmplayer $(SRCS) src/x16sound.o -ldl

src/x16sound.o: src/x16sound.c
	@$(MKDIR_P) $(dir $@)
	g++ -Isrc -O3 -fPIC -o $@ -c $<

.PHONY src/x16sound.c:

MKDIR_P ?= mkdir -p
