#ifndef P1904_TRACEROUTE_H
#define P1904_TRACEROUTE_H

#include "p1904_errno.h"

p1904_err_t p1904_do_traceroute(p1904_mesh_t *mesh, const char *addr,
    size_t n);

#endif /* P1904_TRACEROUTE_H */