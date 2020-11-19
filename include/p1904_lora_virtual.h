#ifndef P1904_LORA_VIRTUAL_H
#define P1904_LORA_VIRTUAL_H

#include <stdint.h>
#include <stddef.h>

#include "p1904_errno.h"


typedef struct p1904_lora_module_s p1904_lora_module_t;

struct p1904_lora_module_s {
    int fd;
    int active;
    const char *device;
    char *nodes[30];
    size_t n_nodes;
};

p1904_err_t p1904_lora_virtual_init(p1904_lora_module_t *module,
    const char *device);
p1904_err_t p1904_lora_virtual_activate(p1904_lora_module_t *module);
ssize_t p1904_lora_virtual_send(p1904_lora_module_t *module, uint8_t *data,
    size_t len);
ssize_t p1904_lora_virtual_recv(p1904_lora_module_t *module, uint8_t *buf,
    size_t size);
void p1904_lora_virtual_fini(p1904_lora_module_t *module);

p1904_err_t p1904_lora_virtual_set_map_file(p1904_lora_module_t *module,
    const char *fname);
void p1904_lora_virtual_set_accuracy(p1904_lora_module_t *module,
    uint32_t perc);

#endif /* P1904_LORA_VIRTUAL_H */
