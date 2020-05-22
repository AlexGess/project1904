#include <stdio.h>
#include <stdint.h>
#include <stddef.h>

#include "p1904_mesh_api.h"


int main(int argc, char *const argv[])
{
    p1904_mesh_t *mesh1;

    if (argc != 2) {
        fprintf(stderr, "%s\n", "Invalid arguments");
        return 1;
    }

    mesh1 = p1904_mesh_create(argv[1], "10.0.0.1");
    if (!mesh1) {
        fprintf(stderr, "p1904_mesh_create() failed\n");
        return 1;
    }

    p1904_mesh_sendto(mesh1, "10.0.0.2", "hello", sizeof("hello"));

    p1904_mesh_destroy(mesh1);

    return 0;
}
