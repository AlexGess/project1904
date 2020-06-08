#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include "p1904_mesh_api.h"
#include "p1904_ncmp.h"


int p1904_mesh_ncmp_send(p1904_mesh_t *mesh, const char *addr,
    uint8_t type, uint8_t code)
{
    p1904_ncmp_header_t packet;

    packet.type = type;
    packet.code = code;
    return p1904_mesh_sendto(mesh, addr, (const char *) &packet,
        sizeof(p1904_ncmp_header_t));
}

uint8_t p1904_mesh_ncmp_get_type(p1904_ncmp_header_t *packet)
{
    return packet->type;
}

uint8_t p1904_mesh_ncmp_get_code(p1904_ncmp_header_t *packet)
{
    return packet->code;
}
