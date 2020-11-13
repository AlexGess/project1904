#include <stdio.h>
#include <stdint.h>
#include <stddef.h>

#include "p1904_mesh_api.h"

/*
 * Usage: ./client <device> <addr> [map_file]
 */

int
main(int argc, char **argv)
{
    p1904_mesh_t *mesh1;

    if (argc < 3) {
        fprintf(stderr, "%s\n", "Invalid number of arguments");
        return 1;
    }

    mesh1 = p1904_mesh_create(argv[1], argv[2]);
    if (!mesh1) {
        fprintf(stderr, "p1904_mesh_create() failed\n");
        return 1;
    }

#if (P1904_VIRTUAL_DEV)
    if (argc >= 4) {
        p1904_lora_virtual_set_map_file(&(mesh1->module), argv[3]);
    }
    else {
        fprintf(stderr, "%s\n", "Missing network map file for virtual device");
        return 1;
    }
#endif



    const char data[] = "Hello, World!";

    p1904_mesh_sendto(mesh1, "127.0.0.2", data, sizeof(data));

    p1904_mesh_destroy(mesh1);

    return 0;
}
