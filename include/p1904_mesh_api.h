#ifndef P1904_MESH_API_H
#define P1904_MESH_API_H

#include <stdint.h>
#include <stddef.h>

#include "p1904_errno.h"
#include "p1904_lora_rak811.h"

#define P1904_DEFAULT_TTL      32
#define P1904_MAX_PACKET_SIZE  512
#define P1904_INVALID_ADDR     0
#define P1904_ADDR_ANY         "255.255.255.255"
#define p1904_packet_data_offset(ptr)  (ptr + sizeof(p1904_mesh_header_t))

typedef struct p1904_mesh_s p1904_mesh_t;
typedef struct p1904_mesh_header_s p1904_mesh_header_t;
typedef uint32_t p1904_mesh_addr_t;


struct p1904_mesh_header_s {
    p1904_mesh_addr_t src;
    p1904_mesh_addr_t dst;
    p1904_mesh_addr_t gtw;
    uint32_t checksum;
    uint16_t size;
    uint16_t ttl;
    /* ... */
};

struct p1904_mesh_s {
    p1904_lora_module_t module;
    p1904_mesh_addr_t addr;
    /* ... */
};


p1904_mesh_t *p1904_mesh_create(const char *device, const char *addr);
p1904_err_t p1904_mesh_sendto(p1904_mesh_t *mesh, const char *addr,
    const char *data, size_t len);
p1904_err_t p1904_mesh_sendto_packet(p1904_mesh_t *mesh, uint8_t *packet,
    size_t packet_size);
p1904_err_t p1904_mesh_recvfrom(p1904_mesh_t *mesh, const char *addr,
    const char *buf, size_t size);
p1904_err_t p1904_mesh_do_routing(p1904_mesh_t *mesh);
void p1904_mesh_destroy(p1904_mesh_t *mesh);

p1904_mesh_addr_t p1904_mesh_addr_to_bin(const char *addr);
const char *p1904_mesh_bin_to_addr(p1904_mesh_addr_t addr);

#endif /* P1904_MESH_API_H */
