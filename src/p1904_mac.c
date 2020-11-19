#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/select.h>

#include "p1904_crc32.h"
#include "p1904_random.h"
#include "p1904_mac.h"

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

#define P1904_IFC   100000
#define P1904_SLOT  10000
#define P1904_CW    50

#define P1904_MAX_DATA_SIZE  2312

static ssize_t 
p1904_mac_sendto_with_frame(p1904_lora_module_t *module, p1904_mac_addr_t addr,
    uint8_t *data, size_t len)
{
    p1904_frame_t frame;

    memset(&frame, 0, sizeof(p1904_frame_t));

    if (len > P1904_MAX_DATA_SIZE) {
        len = P1904_MAX_DATA_SIZE;
    }

    p1904_lora_send(module, (uint8_t *) &frame, sizeof(p1904_frame_t));
    p1904_lora_send(module, data, len);

    return len;
}

static p1904_err_t
p1904_mac_wait_ack(p1904_lora_module_t *module)
{
    return P1904_OK;
}

ssize_t
p1904_mac_sendto(p1904_lora_module_t *module, p1904_mac_addr_t addr,
    uint8_t *data, size_t len)
{
    fd_set rfds;
    int fdmax, flag;
    struct timeval timeout;
    static char buf[256];

    size_t offset, n;
    uint32_t backoff;
    bool done_ifc;

    offset = 0;
    done_ifc = false;
    backoff = p1904_random_uint_in_range(0, P1904_CW);
    printf("%u\n", backoff);


    while (1) {

        FD_ZERO(&rfds);

        FD_SET(module->fd, &rfds);
        if (fdmax < module->fd) {
            fdmax = module->fd;
        }

        timeout.tv_sec = 0;

        if (done_ifc) {
            timeout.tv_usec = P1904_SLOT;
        }
        else {
            timeout.tv_usec = P1904_IFC;
        }
        
        flag = select(fdmax + 1, &rfds, NULL, NULL, &timeout);
        if (flag == -1) {
            fprintf(stderr, "select() failed\n");
            return -1;
        }
        else if (flag == 0) {
            printf("timeout\n");
            if (done_ifc && backoff) {
                backoff -= 1;
                printf("%u\n", backoff);
            }
            else if (done_ifc && !backoff) {
                printf("send\n");
                n = p1904_mac_sendto_with_frame(module, addr, data, len);
                offset += n;
                len -= n;
                if (p1904_mac_wait_ack(module) == P1904_FAILED) {

                }
                if (!len) {
                    return offset;
                }
            }
            else {
                done_ifc = true;
            }
        }
        else {
            if (FD_ISSET(module->fd, &rfds)) {
                done_ifc = false;
                /* Flush input data */
                read(module->fd, buf, sizeof(buf));
                printf("new data\n");
            }
        }

    } /* while (1) */
}

