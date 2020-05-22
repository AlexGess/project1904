#ifndef P1904_MESH_API_H
#define P1904_MESH_API_H

#include <stdint.h>
#include <stddef.h>

#include "p1904_lora_rak811.h"


typedef struct p1904_mesh_s p1904_mesh_t;
typedef uint32_t p1904_mesh_addr_t;



struct p1904_mesh_s {
    p1904_lora_module_t module;
};


p1904_mesh_t *p1904_mesh_create(const char *device, const char *addr);
int p1904_mesh_sendto(p1904_mesh_t *mesh, const char *addr, const char *str,
    size_t len);

// int p1904_mesh_listen();

void p1904_mesh_destroy(p1904_mesh_t *mesh);

p1904_mesh_addr_t p1904_mesh_addr_to_bin(const char *addr);
const char *p1904_mesh_bin_to_addr(p1904_mesh_addr_t addr);

#endif /* P1904_MESH_API_H */
