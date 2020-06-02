BIN_FILE_CLIENT = client
BIN_FILE_SERVER = server

SRC_CLIENT += p1904_lora_rak811.c
SRC_CLIENT += p1904_crc32.c
SRC_CLIENT += p1904_route_table.c
SRC_CLIENT += p1904_mesh_api.c
SRC_CLIENT += client.c

SRC_SERVER += p1904_lora_rak811.c
SRC_SERVER += p1904_crc32.c
SRC_SERVER += p1904_route_table.c
SRC_SERVER += p1904_mesh_api.c
SRC_SERVER += server.c

CFLAGS += -Wall
debug: CFLAGS += -g3 -ggdb3 -O0 -DDEBUG
build: CFLAGS += -g0 -s -O3 -DNDEBUG

CC = gcc

.PHONY: all debug build

all: build

debug: mkdirs
	$(CC) $(SRC_CLIENT) $(CFLAGS) $(LDFLAGS) -o bin/$(BIN_FILE_CLIENT)
	$(CC) $(SRC_SERVER) $(CFLAGS) $(LDFLAGS) -o bin/$(BIN_FILE_SERVER)

build: mkdirs
	$(CC) $(SRC_CLIENT) $(CFLAGS) $(LDFLAGS) -o bin/$(BIN_FILE_CLIENT)
	$(CC) $(SRC_SERVER) $(CFLAGS) $(LDFLAGS) -o bin/$(BIN_FILE_SERVER)

clean:
	rm -f bin/*

mkdirs: 
	@if [ ! -d bin ] ; then \
		mkdir bin ; \
	fi
