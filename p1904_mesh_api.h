#ifndef P1904_MESH_API_H
#define P1904_MESH_API_H

#include <stdint.h>
#include <stddef.h>


typedef struct p1904_mesh_s p1904_mesh_t;

struct p1904_mesh_addr_s {
    uint32_t addr; 
};

struct p1904_mesh_s {
    int fd;

};


p1904_mesh_t *p1904_mesh_create(const char *device, const char *addr);
int p1904_mesh_sendto(p1904_mesh_t *mesh, const char *addr, const char *str,
    size_t len);

// int p1904_mesh_listen();

void p1904_mesh_destroy(p1904_mesh_t *mesh);

#endif /* P1904_MESH_API_H */
