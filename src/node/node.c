#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>

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

    if (p1904_mesh_do_routing(mesh1) != P1904_OK) {
        fprintf(stderr, "p1904_mesh_do_routing() failed\n");
        return 1;
    }

    p1904_mesh_destroy(mesh1);

    return 0;
}
