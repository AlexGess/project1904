/* 
 * Network Control Message Protocol
 */

#ifndef P1904_NCMP_H
#define P1904_NCMP_H

#define P1904_NCMP_TYPE1  0x1
#define P1904_NCMP_TYPE2  0x2

#define P1904_NCMP_ERR1  0x1
#define P1904_NCMP_ERR2  0x2
#define P1904_NCMP_ERR3  0x3
#define P1904_NCMP_ERR4  0x4

typedef struct p1904_ncmp_header_s p1904_ncmp_header_t;

struct p1904_ncmp_header_s {
    uint8_t type;
    uint8_t code;
};



#endif /* P1904_NCMP_H */
