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
    int err;

    mesh = malloc(sizeof(p1904_mesh_t));
    if (!mesh) {
        goto failed;
    }

    err = p1904_lora_rak811_init(&(mesh->module), device);


    /* convert addr */

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






}






void p1904_mesh_destroy(p1904_mesh_t *mesh)
{
    free(mesh);
}



