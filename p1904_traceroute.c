#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "p1904_route_table.h"
#include "p1904_ncmp.h"
#include "p1904_crc32.h"
#include "p1904_mesh_api.h"
#include "p1904_traceroute.h"


int p1904_do_traceroute(p1904_mesh_t *mesh, const char *addr, size_t n)
{
    static uint8_t packet[P1904_MAX_PACKET_SIZE];
    static uint8_t buf[P1904_MAX_PACKET_SIZE];
    p1904_mesh_header_t *header;
    p1904_mesh_header_t *recv_header;
    p1904_mesh_addr_t addr_bin;
    p1904_mesh_addr_t gtw;
    uint32_t checksum;
    size_t packet_size;
    char *addr_text;

    memset(packet, 0, P1904_MAX_PACKET_SIZE);

    packet_size = P1904_MAX_PACKET_SIZE;

    addr_bin = p1904_mesh_addr_to_bin(addr);
    header = (p1904_mesh_header_t *) packet;
    recv_header = (p1904_mesh_header_t *) buf;

    gtw = p1904_route_table_find(addr_bin);
    if (gtw == P1904_INVALID_ADDR) {
        return EXIT_FAILURE;
    }

    header->src = mesh->addr;
    header->dst = addr_bin;
    header->gtw = gtw;

    for (size_t i = 0; i < n; i++) {
        header->ttl = i;
        header->size = packet_size;
        header->checksum = 0;
        checksum = p1904_do_crc32(packet, packet_size);
        header->checksum = checksum;

        p1904_mesh_sendto_packet(mesh, packet, P1904_MAX_PACKET_SIZE);
        if (p1904_mesh_recvfrom(mesh, P1904_ADDR_ANY, (const char *) buf,
            P1904_MAX_PACKET_SIZE) != EXIT_SUCCESS)
        {
            continue;
        }

        addr_text = (char *) p1904_mesh_bin_to_addr(recv_header->src);

        printf("ttl: %zu, addr: %s\n", i, addr_text);

    }

    return EXIT_SUCCESS;
}
