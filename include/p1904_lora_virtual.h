#ifndef P1904_LORA_VIRTUAL_H
#define P1904_LORA_VIRTUAL_H

#include <stdint.h>
#include <stddef.h>

#include "p1904_errno.h"

#define P1904_MAX_DATA_SIZE  512


typedef struct p1904_lora_module_s p1904_lora_module_t;

struct p1904_lora_module_s {
    int fd;
    int active;
    const char *device;
    char *nodes[30];
    size_t n_nodes;
};

p1904_err_t p1904_lora_virtual_init(p1904_lora_module_t *m,
    const char *device);
p1904_err_t p1904_lora_virtual_activate(p1904_lora_module_t *m);
ssize_t p1904_lora_virtual_send(p1904_lora_module_t *m, void *data,
    size_t len);
ssize_t p1904_lora_virtual_recv(p1904_lora_module_t *m, void *buf,
    size_t size);
void p1904_lora_virtual_fini(p1904_lora_module_t *m);

p1904_err_t p1904_lora_virtual_set_map_file(p1904_lora_module_t *m,
    const char *fname);
void p1904_lora_virtual_set_accuracy(p1904_lora_module_t *m, uint32_t perc);

#endif /* P1904_LORA_VIRTUAL_H */
