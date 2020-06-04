#include <stdio.h>
#include <stdint.h>
#include <stddef.h>

#include "p1904_mesh_api.h"
#include "p1904_route_table.h"


int main(int argc, char *const argv[])
{
    p1904_mesh_t *mesh1;
    const char *client_addr = "127.0.0.5";

    if (argc != 2) {
        fprintf(stderr, "%s\n", "Invalid arguments");
        return 1;
    }

    mesh1 = p1904_mesh_create(argv[1], client_addr);
    if (!mesh1) {
        fprintf(stderr, "p1904_mesh_create() failed\n");
        return 1;
    }

    p1904_route_table_add("127.0.0.1", "10.0.0.5", 1, P1904_DEFAULT_TTL);
    p1904_route_table_add("127.0.0.2", "10.0.0.5", 1, P1904_DEFAULT_TTL);
    p1904_route_table_add("127.0.0.3", "10.0.0.5", 1, P1904_DEFAULT_TTL);

    p1904_route_table_print();

    p1904_mesh_sendto(mesh1, "127.0.0.1", "hello", sizeof("hello"));
    // p1904_mesh_sendto(mesh1, "127.0.0.2", "hello", sizeof("hello"));
    // p1904_mesh_sendto(mesh1, "127.0.0.3", "hello", sizeof("hello"));

    p1904_mesh_destroy(mesh1);

    return 0;
}
