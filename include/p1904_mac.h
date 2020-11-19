#ifndef P1904_MAC_H
#define P1904_MAC_H

#if (P1904_VIRTUAL_DEV)
#include "p1904_lora_virtual.h"
#else
#include "p1904_lora_rak811.h"
#endif


#define P1904_MAC_ADDR_LEN  6

typedef struct p1904_mac_addr_s p1904_mac_addr_t;
typedef struct p1904_frame_s p1904_frame_t;
typedef struct p1904_frame_control_s p1904_frame_control_t;


struct p1904_mac_addr_s {
    uint8_t addr[P1904_MAC_ADDR_LEN];
};

struct p1904_frame_control_s {
    uint16_t proto_version : 2;
    uint16_t type : 2;
    uint16_t sub_type : 4;
    uint16_t to_ap : 1;
    uint16_t from_ap : 1;
    uint16_t mf : 1;
    uint16_t retry : 1;
    uint16_t pm : 1;
    uint16_t md : 1;
    uint16_t encr : 1;
    uint16_t order : 1;
};

struct p1904_frame_s {
    p1904_frame_control_t fc;
    uint16_t duration;
    p1904_mac_addr_t addr1;
    p1904_mac_addr_t addr2;
    p1904_mac_addr_t addr3;
    uint16_t seq_cntl;
    p1904_mac_addr_t addr4;
    uint32_t crc;
    /* uint8_t data[2312]; */
};


ssize_t p1904_mac_sendto(p1904_lora_module_t *module, p1904_mac_addr_t addr,
    uint8_t *data, size_t len);




#endif /* P1904_MAC_H */
