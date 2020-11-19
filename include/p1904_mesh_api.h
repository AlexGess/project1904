#ifndef P1904_MESH_API_H
#define P1904_MESH_API_H

#include "p1904_mac.h"

#define P1904_MAC_ADDR_STR_LEN  18

typedef struct p1904_mesh_s p1904_mesh_t;

struct p1904_mesh_s {
    p1904_lora_module_t module;
    p1904_mac_addr_t addr;
};

p1904_mesh_t *p1904_mesh_create(const char *device, const char *addr);
p1904_err_t p1904_mesh_sendto(p1904_mesh_t *mesh, p1904_mac_addr_t addr,
    uint8_t *data, size_t len);
p1904_err_t p1904_mesh_do_routing(p1904_mesh_t *mesh);
void p1904_mesh_destroy(p1904_mesh_t *mesh);

p1904_mac_addr_t p1904_mesh_addr_to_bin(const char *addr);
const char *p1904_mesh_addr_to_str(char *buf, p1904_mac_addr_t addr);

#endif /* P1904_MESH_API_H */
