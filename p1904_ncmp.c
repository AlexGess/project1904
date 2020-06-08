#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include "p1904_mesh_api.h"
#include "p1904_ncmp.h"


int p1904_mesh_send_ncmp(p1904_mesh_t *mesh, const char *addr,
    uint8_t type, uint8_t code)
{
    p1904_ncmp_header_t packet;

    packet.type = type;
    packet.code = code;
    return p1904_mesh_sendto(mesh, addr, (const char *) &packet,
        sizeof(p1904_ncmp_header_t));
}