#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "p1904_mesh_api.h"
#include "p1904_crc32.h"


p1904_mesh_addr_t p1904_mesh_addr_to_bin(const char *addr)
{
    char buf[16];
    char *token;
    char *save_ptr;
    uint8_t *byte_ptr;
    p1904_mesh_addr_t temp;

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

int p1904_mesh_sendto(p1904_mesh_t *mesh, const char *addr, const char *data,
    size_t len)
{
    static uint8_t packet[P1904_MAX_PACKET_SIZE];
    p1904_mesh_header_t *header;
    size_t packet_size;
    uint32_t checksum;
    ssize_t bytes_sent;


    header = (p1904_mesh_header_t *) packet;
    packet_size = len + sizeof(p1904_mesh_header_t);

    if (packet_size > P1904_MAX_PACKET_SIZE) {
        return EXIT_FAILURE;
    }

    if (!mesh->module.active) {
        p1904_lora_rak811_activate(&(mesh->module));
    }

    header->src = mesh->addr;
    header->dst = p1904_mesh_addr_to_bin(addr);
    header->ttl = P1904_DEFAULT_TTL;
    header->size = packet_size;
    header->checksum = 0;

    memmove(p1904_packet_data_offset(packet), data, len);

    checksum = p1904_do_crc32(packet, packet_size);
    header->checksum = checksum;

    bytes_sent = p1904_lora_rak811_send(&(mesh->module), packet, packet_size);
    if (bytes_sent < 0) {
        return EXIT_FAILURE;
    }

    printf("src: %s\n", p1904_mesh_bin_to_addr(header->src));
    printf("dst: %s\n", p1904_mesh_bin_to_addr(header->dst));
    printf("ttl: %u\n", header->ttl);
    printf("size: %u\n", header->size);
    printf("checksum: %u\n", header->checksum);

    return EXIT_SUCCESS;
}

int p1904_mesh_recvfrom(p1904_mesh_t *mesh, const char *addr, const char *buf,
    size_t size)
{
    static uint8_t packet[P1904_MAX_PACKET_SIZE];
    p1904_mesh_header_t *header;
    size_t packet_size;
    uint32_t checksum;
    uint32_t new_checksum;
    uint8_t *data;
    ssize_t bytes_recv;

    if (!mesh->module.active) {
        p1904_lora_rak811_activate(&(mesh->module));
    }

    header = (p1904_mesh_header_t *) packet;
    bytes_recv = 0;

    while (1) {
        bytes_recv = p1904_lora_rak811_recv(&(mesh->module), packet,
            P1904_MAX_PACKET_SIZE);
        if (bytes_recv < 0) {
            return EXIT_FAILURE;
        }
        if (bytes_recv < sizeof(p1904_mesh_header_t)) {
            continue;
        }

        packet_size = header->size;
        checksum = header->checksum; /* Save checksum value */
        header->checksum = 0;
        new_checksum = p1904_do_crc32(packet, packet_size);
        if (checksum == new_checksum) {
            header->checksum = checksum;
            break; /* Valid package was found */
        }
    }

    printf("src: %s\n", p1904_mesh_bin_to_addr(header->src));
    printf("dst: %s\n", p1904_mesh_bin_to_addr(header->dst));
    printf("ttl: %u\n", header->ttl);
    printf("size: %u\n", header->size);
    printf("checksum: %u\n", header->checksum);

    data = p1904_packet_data_offset(packet);
    if (packet_size > size) {
        packet_size = size;
    }

    memmove((void *) buf, data, packet_size);

    return EXIT_SUCCESS;
}

void p1904_mesh_destroy(p1904_mesh_t *mesh)
{
    free(mesh);
}
