#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "p1904_crc32.h"
#include "p1904_route_table.h"


struct p1904_route_table_record_s {
    bool active;
    p1904_mesh_addr_t dst;
    p1904_mesh_addr_t gtw;
    uint16_t metric;
    uint16_t ttl;
};

static p1904_route_table_record_t p1904_route_table[P1904_ROUTE_TABLE_SIZE];


p1904_err_t
p1904_route_table_add(const char *dst, const char *gtw, uint16_t metric,
    uint16_t ttl)
{
    p1904_route_table_record_t *record;
    p1904_mesh_addr_t dst_bin;
    p1904_mesh_addr_t gtw_bin;
    bool found = false;
    bool do_update = false;

    dst_bin = p1904_mesh_addr_to_bin(dst);
    gtw_bin = p1904_mesh_addr_to_bin(gtw);

    /* Try to find already exist records */
    for (size_t i = 0; i < P1904_ROUTE_TABLE_SIZE; i++) {
        record = &(p1904_route_table[i]);
        if (record->active) {
            if (record->dst == dst_bin && record->gtw == gtw_bin) {
                do_update = true;
                break;
            }
            else {
                continue;
            }
        }
    }

    if (do_update) {
        /* Update fields */
        record->metric = metric;
        record->ttl = ttl;
        return P1904_OK;
    }

    /* Try to find available space for new record */
    for (size_t i = 0; i < P1904_ROUTE_TABLE_SIZE; i++) {
        record = &(p1904_route_table[i]);
        if (record->active) {
            continue;
        }
        else {
            found = true;
            break;
        }
    }

    if (!found) {
        /* Route table is full */
        return P1904_FAILED;
    }

    record->dst = dst_bin;
    record->gtw = gtw_bin;
    record->metric = metric;
    record->ttl = ttl;

    record->active = true;

    return P1904_OK;
}

p1904_mesh_addr_t
p1904_route_table_find(p1904_mesh_addr_t dst)
{
    p1904_route_table_record_t *record;
    p1904_route_table_record_t *temp;

    uint16_t best_metric = 65535;
    bool found = false;

    for (size_t i = 0; i < P1904_ROUTE_TABLE_SIZE; i++) {
        record = &(p1904_route_table[i]);
        if (record->active) {
            if (record->dst == dst) {
                if (best_metric > record->metric) {
                    best_metric = record->metric;
                    temp = record;
                    found = true;
                }
                continue;
            }
        }
    }

    if (!found) {
        return P1904_INVALID_ADDR;
    }

    return temp->gtw;
}

p1904_mesh_addr_t
p1904_route_table_find1(const char *dst)
{
    p1904_mesh_addr_t dst_bin;

    dst_bin = p1904_mesh_addr_to_bin(dst);
    return p1904_route_table_find(dst_bin);
}


void
p1904_route_table_del(const char *dst, const char *gtw)
{
    p1904_route_table_record_t *record;
    p1904_mesh_addr_t dst_bin;
    p1904_mesh_addr_t gtw_bin;

    dst_bin = p1904_mesh_addr_to_bin(dst);
    gtw_bin = p1904_mesh_addr_to_bin(gtw);

    for (size_t i = 0; i < P1904_ROUTE_TABLE_SIZE; i++) {
        record = &(p1904_route_table[i]);
        if (record->active) {
            if (record->dst == dst_bin && record->gtw == gtw_bin) {
                record->active = false;
                return;
            }
            else {
                continue;
            }
        }
    }
}

void
p1904_route_table_print(void)
{
    p1904_route_table_record_t *record;
    static char buf1[50];
    static char buf2[50];
    char *temp;
    static const char *str1 =
        "+-----------------+-----------------+---------+----------+\n"
        "|   Destination   |     Gateway     | Metrics |    TTL   |\n"
        "+-----------------+-----------------+---------+----------+\n";
    static const char *str2 =
        "+-----------------+-----------------+---------+----------+\n";

    fprintf(stdout, "%s", str1);

    for (size_t i = 0; i < P1904_ROUTE_TABLE_SIZE; i++) {
        record = &(p1904_route_table[i]);
        if (record->active) {
            temp = (char *) p1904_mesh_bin_to_addr(record->dst);
            memmove(buf1, temp, strlen(temp));
            buf1[strlen(temp)] = '\0';

            temp = (char *) p1904_mesh_bin_to_addr(record->gtw);
            memmove(buf2, temp, strlen(temp));
            buf2[strlen(temp)] = '\0';

            fprintf(stdout, "| %*s | %*s | %*u | %*u |\n",
                15, buf1,
                15, buf2,
                7, record->metric,
                8, record->ttl);
        }
    }

    fprintf(stdout, "%s", str2);
}
