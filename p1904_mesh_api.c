#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "p1904_mesh_api.h"


p1904_mesh_addr_t p1904_mesh_addr_to_bin(const char *addr)
{
    char buf[16];
    char *token;
    char *save_ptr;
    uint8_t *byte_ptr;
    uint32_t temp;

    memmove(buf, addr, strlen(addr) + 1);

    byte_ptr = (uint8_t *) &temp;

    token = strtok_r(buf, ".", &save_ptr);

    for (size_t i = 0; i < 4; i++) {
        *(byte_ptr + i) = atoi(token);
        token = strtok_r(NULL, ".", &save_ptr);
    }

    return temp;
}

const char *p1904_mesh_bin_to_addr(p1904_mesh_addr_t addr)
{
    static _Thread_local char buf[16];
    uint8_t *byte_ptr;

    byte_ptr = (uint8_t *) &addr;

    snprintf(buf, sizeof(buf), "%u.%u.%u.%u",
        *(byte_ptr+0),
        *(byte_ptr+1),
        *(byte_ptr+2),
        *(byte_ptr+3));
    return buf;
}


p1904_mesh_t *p1904_mesh_create(const char *device, const char *addr)
{
    p1904_mesh_t *mesh;
    p1904_mesh_addr_t bin_addr;
    int err;

    mesh = malloc(sizeof(p1904_mesh_t));
    if (!mesh) {
        goto failed;
    }

    err = p1904_lora_rak811_init(&(mesh->module), device);
    if (err != EXIT_SUCCESS) {
        goto failed;
    }

    bin_addr = p1904_mesh_addr_to_bin(addr);


    mesh->addr = bin_addr;

    return mesh;

failed:
    if (mesh) {
        free(mesh);
    }
    /* p1904_lora_rak811_fini() */

    return NULL;
}


int p1904_mesh_sendto(p1904_mesh_t *mesh, const char *addr, const char *str,
    size_t len)
{
    static p1904_mesh_header_t header;
    static char packet[P1904_MAX_PACKET_SIZE];
    size_t total_size;

    total_size = sizeof(p1904_mesh_header_t) + len;

    if (len == 0) {
        return EXIT_FAILURE;
    }

    if (total_size > P1904_MAX_PACKET_SIZE) {
        return EXIT_FAILURE;
    }

    header.src = mesh->addr;
    header.dst = p1904_mesh_addr_to_bin(addr);
    header.ttl = P1904_DEFAULT_TTL;
    header.size = len;

    memmove(packet, &header, sizeof(p1904_mesh_header_t));
    memmove(p1904_packet_data_offset(packet), str, len);

    if (!mesh->module.active_send) {
        p1904_lora_rak811_init_to_send(&(mesh->module));
    }
    p1904_lora_rak811_send(&(mesh->module), packet, total_size);
}






void p1904_mesh_destroy(p1904_mesh_t *mesh)
{
    free(mesh);
}



