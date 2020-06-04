#ifndef P1904_ROUTE_TABLE_H
#define P1904_ROUTE_TABLE_H

#include "p1904_mesh_api.h"

#define P1904_ROUTE_TABLE_SIZE  64

typedef struct p1904_route_table_record_s p1904_route_table_record_t;

int p1904_route_table_add(const char *dst, const char *gtw, uint16_t metric,
    uint16_t ttl);
p1904_mesh_addr_t p1904_route_table_find(p1904_mesh_addr_t dst);
p1904_mesh_addr_t p1904_route_table_find1(const char *dst);
void p1904_route_table_del(const char *dst, const char *gtw);
void p1904_route_table_print(void);

#endif /* P1904_ROUTE_TABLE_H */
