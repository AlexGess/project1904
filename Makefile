BIN_FILE_CLIENT = client
BIN_FILE_NODE = node

SRC_CLIENT += client.c
# SRC_CLIENT += p1904_lora_rak811.c
SRC_CLIENT += p1904_lora_virtual.c
SRC_CLIENT += p1904_crc32.c
SRC_CLIENT += p1904_random.c
SRC_CLIENT += p1904_mac.c
SRC_CLIENT += p1904_mesh_api.c

SRC_NODE += node.c
# SRC_NODE += p1904_lora_rak811.c
SRC_NODE += p1904_lora_virtual.c
SRC_NODE += p1904_crc32.c
SRC_NODE += p1904_random.c
SRC_NODE += p1904_mac.c
SRC_NODE += p1904_mesh_api.c

override CFLAGS += -Iinclude
override CFLAGS += -Wall -Wextra -Wno-uninitialized
override CFLAGS += -I/usr/include/libxml2
override CFLAGS += -DP1904_VIRTUAL_DEV
debug: override CFLAGS += -g3 -ggdb3 -O0 -DDEBUG
build: override CFLAGS += -g0 -s -O3
override LDFLAGS += -lxml2

CC = gcc

OBJ_FILES_CLIENT := $(patsubst %.c,obj/%.o,$(SRC_CLIENT))
OBJ_FILES_NODE := $(patsubst %.c,obj/%.o,$(SRC_NODE))
QUIET_CC = @echo '   ' CC $(notdir $@);

VPATH += src
VPATH += src/node
VPATH += src/client

.PHONY: all debug build

all: build
build: mkdirs _build_node _build_client
debug: mkdirs _debug_node _debug_client


_build_node: $(OBJ_FILES_NODE)
	$(CC) $^ -o bin/$(BIN_FILE_NODE) $(LDFLAGS)

_build_client: $(OBJ_FILES_CLIENT)
	$(CC) $^ -o bin/$(BIN_FILE_CLIENT) $(LDFLAGS)

_debug_node: $(OBJ_FILES_NODE)
	$(CC) $^ -o bin/$(BIN_FILE_NODE) $(LDFLAGS)

_debug_client: $(OBJ_FILES_CLIENT)
	$(CC) $^ -o bin/$(BIN_FILE_CLIENT) $(LDFLAGS)

obj/%.o: %.c
	$(QUIET_CC) $(CC) -c $< -o $@ $(CFLAGS)

clean:
	rm -f obj/*
	rm -f bin/*

mkdirs: 
	@if [ ! -d bin ]; then \
		mkdir bin; \
	fi

	@if [ ! -d obj ]; then \
		mkdir obj; \
	fi
