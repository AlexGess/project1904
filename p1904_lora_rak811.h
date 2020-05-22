#ifndef P1904_LORA_RAK811_H
#define P1904_LORA_RAK811_H

#include <stdint.h>
#include <stddef.h>

#define P1904_MAX_DATA_SIZE  512


typedef struct p1904_lora_module_s p1904_lora_module_t;

struct p1904_lora_module_s {
    int fd;
};


int p1904_lora_rak811_init(p1904_lora_module_t *m, const char *device);
int p1904_lora_rak811_send(p1904_lora_module_t *m, const char *addr,
    const char *str, size_t len);

int p1904_lora_rak811_fini(p1904_lora_module_t *m);



#endif /* P1904_LORA_RAK811_H */