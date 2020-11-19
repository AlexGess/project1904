#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "p1904_mesh_api.h"

#if (P1904_VIRTUAL_DEV)
#include "p1904_lora_virtual.h"
#else
#include "p1904_lora_rak811.h"
#endif


#if (P1904_VIRTUAL_DEV)
#define p1904_lora_init      p1904_lora_virtual_init
#define p1904_lora_activate  p1904_lora_virtual_activate
#define p1904_lora_send      p1904_lora_virtual_send
#define p1904_lora_recv      p1904_lora_virtual_recv
#define p1904_lora_fini      p1904_lora_virtual_fini
#else
#define p1904_lora_init      p1904_lora_rak811_init
#define p1904_lora_activate  p1904_lora_rak811_activate
#define p1904_lora_send      p1904_lora_rak811_send
#define p1904_lora_recv      p1904_lora_rak811_recv
#define p1904_lora_fini      p1904_lora_rak811_fini
#endif


p1904_mac_addr_t
p1904_mesh_addr_to_bin(const char *addr)
{
    char buf[P1904_MAC_ADDR_STR_LEN];
    char *token, *save_ptr;
    uint8_t *byte_ptr;

    p1904_mac_addr_t temp;

    memmove(buf, addr, strlen(addr) + 1);

    byte_ptr = (uint8_t *) &temp;

    token = strtok_r(buf, ":", &save_ptr);

    for (size_t i = 0; i < 6; i++) {
        *(byte_ptr + i) = strtol(token, NULL, 16);
        token = strtok_r(NULL, ":", &save_ptr);
    }

    return temp;
}

const char *
p1904_mesh_addr_to_str(char *buf, p1904_mac_addr_t addr)
{
    uint8_t *byte_ptr;

    byte_ptr = (uint8_t *) &addr;

    snprintf(buf, P1904_MAC_ADDR_STR_LEN,
        "%02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx",
        *(byte_ptr+0),
        *(byte_ptr+1),
        *(byte_ptr+2),
        *(byte_ptr+3),
        *(byte_ptr+4),
        *(byte_ptr+5));

    return buf;
}

p1904_mesh_t *
p1904_mesh_create(const char *device, const char *addr)
{
    p1904_mesh_t *mesh;
    int err;

    mesh = malloc(sizeof(p1904_mesh_t));
    if (!mesh) {
        goto failed;
    }

    err = p1904_lora_init(&(mesh->module), device);
    if (err != P1904_OK) {
        goto failed;
    }

    mesh->addr = p1904_mesh_addr_to_bin(addr);

    return mesh;

failed:
    if (mesh) {
        free(mesh);
    }
    /* p1904_lora_fini() */

    return NULL;
}

void
p1904_mesh_destroy(p1904_mesh_t *mesh)
{
    free(mesh);
}


p1904_err_t
p1904_mesh_sendto(p1904_mesh_t *mesh, p1904_mac_addr_t addr,
    uint8_t *data, size_t len)
{


    p1904_mac_sendto(&(mesh->module), addr, data, len);


}

#if 0
    static uint8_t packet[P1904_MAX_PACKET_SIZE];
    p1904_mesh_header_t *header;
    p1904_mesh_addr_t addr_bin;
    p1904_mesh_addr_t gtw;
    size_t packet_size;
    uint32_t checksum;
    ssize_t bytes_sent;

    addr_bin = p1904_mesh_addr_to_bin(addr);

    header = (p1904_mesh_header_t *) packet;
    packet_size = len + sizeof(p1904_mesh_header_t);

    if (packet_size > P1904_MAX_PACKET_SIZE) {
        return P1904_FAILED;
    }

    if (!mesh->module.active) {
        p1904_lora_activate(&(mesh->module));
    }

    // gtw = p1904_route_table_find(addr_bin);
    gtw = p1904_mesh_addr_to_bin(P1904_ADDR_ANY);
    if (gtw == P1904_INVALID_ADDR) {
        return P1904_FAILED;
    }

    header->src = mesh->addr;
    header->dst = addr_bin;
    header->gtw = gtw;
    header->ttl = P1904_DEFAULT_TTL;
    header->size = packet_size;
    header->checksum = 0;

    memmove(p1904_packet_data_offset(packet), data, len);

    checksum = p1904_do_crc32(packet, packet_size);
    header->checksum = checksum;

    bytes_sent = p1904_lora_send(&(mesh->module), packet, packet_size);
    if (bytes_sent < 0) {
        return P1904_FAILED;
    }


#ifdef DEBUG
    printf("src: %s\n", p1904_mesh_bin_to_addr(header->src));
    printf("dst: %s\n", p1904_mesh_bin_to_addr(header->dst));
    printf("gtw: %s\n", p1904_mesh_bin_to_addr(header->gtw));
    printf("ttl: %u\n", header->ttl);
    printf("size: %u\n", header->size);
    printf("checksum: %#x\n", header->checksum);
#endif

    return P1904_OK;
}

p1904_err_t
p1904_mesh_sendto_packet(p1904_mesh_t *mesh, uint8_t *packet,
    size_t packet_size)
{
    p1904_mesh_header_t *header;
    ssize_t bytes_sent;

    header = (p1904_mesh_header_t *) packet;

    if (packet_size > P1904_MAX_PACKET_SIZE) {
        return P1904_FAILED;
    }

    if (!mesh->module.active) {
        p1904_lora_activate(&(mesh->module));
    }

    bytes_sent = p1904_lora_send(&(mesh->module), packet, packet_size);
    if (bytes_sent < 0) {
        return P1904_FAILED;
    }

#ifdef DEBUG
    printf("src: %s\n", p1904_mesh_bin_to_addr(header->src));
    printf("dst: %s\n", p1904_mesh_bin_to_addr(header->dst));
    printf("gtw: %s\n", p1904_mesh_bin_to_addr(header->gtw));
    printf("ttl: %u\n", header->ttl);
    printf("size: %u\n", header->size);
    printf("checksum: %#x\n", header->checksum);
#endif

    return P1904_OK;
}

p1904_err_t
p1904_mesh_recvfrom(p1904_mesh_t *mesh, const char *addr, const char *buf,
    size_t size)
{
    static uint8_t packet[P1904_MAX_PACKET_SIZE];
    p1904_mesh_header_t *header;
    p1904_mesh_addr_t addr_bin;
    size_t packet_size;
    uint32_t checksum;
    uint32_t new_checksum;
    uint8_t *data;
    ssize_t bytes_recv;

    if (!mesh->module.active) {
        p1904_lora_activate(&(mesh->module));
    }

    addr_bin = p1904_mesh_addr_to_bin(addr);
    header = (p1904_mesh_header_t *) packet;
    bytes_recv = 0;

    while (1) {
        bytes_recv = p1904_lora_recv(&(mesh->module), packet,
            P1904_MAX_PACKET_SIZE);
        if (bytes_recv < 0) {
            return P1904_FAILED;
        }
        if (bytes_recv < (ssize_t) sizeof(p1904_mesh_header_t)) {
            continue;
        }

        packet_size = header->size;
        checksum = header->checksum; /* Save checksum value */
        header->checksum = 0;
        new_checksum = p1904_do_crc32(packet, packet_size);
        if (checksum != new_checksum) {
            continue;
        }
        header->checksum = checksum; /* Restore checksum value */

#ifdef DEBUG
        printf("src: %s\n", p1904_mesh_bin_to_addr(header->src));
        printf("dst: %s\n", p1904_mesh_bin_to_addr(header->dst));
        printf("gtw: %s\n", p1904_mesh_bin_to_addr(header->gtw));
        printf("ttl: %u\n", header->ttl);
        printf("size: %u\n", header->size);
        printf("checksum: %#x\n", header->checksum);
#endif
        if (mesh->addr == header->dst &&
            addr_bin == p1904_mesh_addr_to_bin(P1904_ADDR_ANY))
        {
            break; /* We capture packets from ANY address */
        }

        if (mesh->addr == header->dst && addr_bin == header->src) {
            break; /* The package is for us and we wait it from this address */
        }
    }

    data = p1904_packet_data_offset(packet);
    if (packet_size > size) {
        packet_size = size;
    }

    memmove((void *) buf, data, packet_size);

    return P1904_OK;
}

p1904_err_t
p1904_mesh_do_routing(p1904_mesh_t *mesh)
{
    static uint8_t packet[P1904_MAX_PACKET_SIZE];
    p1904_mesh_header_t *header;
    p1904_mesh_addr_t new_gtw;
    size_t packet_size;
    uint32_t checksum;
    uint32_t new_checksum;
    ssize_t bytes_recv;
    ssize_t bytes_sent;

    if (!mesh->module.active) {
        p1904_lora_activate(&(mesh->module));
    }

    header = (p1904_mesh_header_t *) packet;
    bytes_recv = 0;

    while (1) {
        bytes_recv = p1904_lora_recv(&(mesh->module), packet,
            P1904_MAX_PACKET_SIZE);
        if (bytes_recv < 0) {
            return P1904_FAILED;
        }
        if (bytes_recv < (ssize_t) sizeof(p1904_mesh_header_t)) {
            continue;
        }

        packet_size = header->size;
        if (packet_size > P1904_MAX_PACKET_SIZE) {
            continue;
        }


        checksum = header->checksum; /* Save checksum value */
        header->checksum = 0;
        new_checksum = p1904_do_crc32(packet, packet_size);
        if (checksum != new_checksum) {
            continue;
        }
        header->checksum = checksum; /* Restore checksum value */

        // if (header->gtw != mesh->addr) {
        //     continue; /* This package is not for us */
        // }

        new_gtw = p1904_mesh_addr_to_bin(P1904_ADDR_ANY);
        // new_gtw = p1904_route_table_find(header->dst);
        // if (new_gtw == P1904_INVALID_ADDR) {
        //     continue; /* We don't have way for this destination address */
        // }

        if (header->ttl == 0) {
            // p1904_mesh_ncmp_send(mesh, p1904_mesh_bin_to_addr(header->src),
            //     P1904_NCMP_TYPE_TTL_EXPIRE, P1904_NCMP_CODE_DEFAUL);
            continue; /* The package has expired */
        }
        else {
            header->ttl -= 1;
        }

        header->gtw = new_gtw;

        bytes_sent = p1904_lora_send(&(mesh->module), packet,
            packet_size);
        if (bytes_sent < 0) {
            return P1904_FAILED;
        }

#ifdef DEBUG
        printf("src: %s\n", p1904_mesh_bin_to_addr(header->src));
        printf("dst: %s\n", p1904_mesh_bin_to_addr(header->dst));
        printf("gtw: %s\n", p1904_mesh_bin_to_addr(header->gtw));
        printf("ttl: %u\n", header->ttl);
        printf("size: %u\n", header->size);
        printf("checksum: %#x\n\n", header->checksum);
#endif
    }

    return P1904_OK;
}


#endif
