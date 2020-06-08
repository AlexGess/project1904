/* 
 * Network Control Message Protocol
 */

#ifndef P1904_NCMP_H
#define P1904_NCMP_H


#define P1904_NCMP_TYPE_ECHO_ANSER           1
#define P1904_NCMP_TYPE_ECHO_REQUEST         2
#define P1904_NCMP_TYPE_UNREACHABLE          3
#define P1904_NCMP_TYPE_TTL_EXPIRE           4

#define P1904_NCMP_CODE_DEFAUL               1
#define P1904_NCMP_CODE_NETWORK_UNREACHABLE  2
#define P1904_NCMP_CODE_NODE_UNREACHABLE     3



typedef struct p1904_ncmp_header_s p1904_ncmp_header_t;

struct p1904_ncmp_header_s {
    uint8_t type;
    uint8_t code;
};

int p1904_mesh_ncmp_send(p1904_mesh_t *mesh, const char *addr,
    uint8_t type, uint8_t code);

uint8_t p1904_mesh_ncmp_get_type(p1904_ncmp_header_t *packet);
uint8_t p1904_mesh_ncmp_get_code(p1904_ncmp_header_t *packet);

#endif /* P1904_NCMP_H */
