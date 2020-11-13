#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>
#include <libxml/parser.h>
#include <libxml/tree.h>

#include "p1904_lora_virtual.h"


static void
p1904_parse_node(xmlDoc *doc, xmlNode *node, p1904_lora_module_t *m,
    uint32_t valid_host)
{
    xmlNode *cur;
    char *str;


    for (cur = node; cur; cur = cur->next) {

        if (!strcmp((char *) cur->name, "host")) {
            str = (char *) xmlGetProp(cur, (uint8_t *) "name");
            if (!strcmp(str, m->device)) {
                valid_host = 1;
            }
            else {
                valid_host = 0;
            }
            xmlFree(str);
        }
        else if (!strcmp((char *) cur->name, "node") && valid_host) {
            str = (char *) xmlNodeGetContent(cur);
            m->nodes[m->n_nodes] = str;
            m->n_nodes += 1;
        }

        p1904_parse_node(doc, cur->children, m, valid_host);
    }
}

static p1904_err_t
p1904_parse_map_file(xmlDoc *doc, xmlNode *node, p1904_lora_module_t *m)
{
    p1904_parse_node(doc, node, m, 0);
    return P1904_OK;
}

static uint32_t
p1904_random_uint_in_range(uint32_t lower, uint32_t upper)
{
    return ((rand() % (upper - lower + 1)) + lower);
}

p1904_err_t
p1904_lora_virtual_init(p1904_lora_module_t *m, const char *device)
{
    int fd, flags;

    memset(m, 0, sizeof(p1904_lora_module_t));

    fd = open(device, O_RDONLY|O_CREAT|O_TRUNC, 0666);
    if (fd == -1) {
        fprintf(stderr, "open(%s) failed\n", device);
        return P1904_FAILED;
    }

    flags = fcntl(fd, F_GETFL);

    fcntl(fd, F_SETFL, flags & (~O_NONBLOCK));

    srand(time(NULL));


    m->fd = fd;
    m->device = device;

    return P1904_OK;
}

p1904_err_t
p1904_lora_virtual_activate(p1904_lora_module_t *m)
{
    m->active = 1;
    return P1904_OK;
}

ssize_t
p1904_lora_virtual_send(p1904_lora_module_t *m, void *data, size_t len)
{
    size_t i, n;
    int fd;

    for (i = 0; i < m->n_nodes; ++i) {
        fd = open(m->nodes[i], O_WRONLY|O_APPEND, 0666);
        if (fd == -1) {
            fprintf(stderr, "open(%s) failed\n", m->nodes[i]);
            continue;
        }
        n = write(fd, data, len);
        if (n != len) {
            fprintf(stderr, "write() to file %s failed\n", m->nodes[i]);
            continue;
        }
    }

    return len;
}

ssize_t
p1904_lora_virtual_recv(p1904_lora_module_t *m, void *buf, size_t size)
{
    return read(m->fd, buf, size);
}

void
p1904_lora_virtual_fini(p1904_lora_module_t *m)
{
    size_t i;

    m->active = 0;

    for (i = 0; i < m->n_nodes; ++i) {
        xmlFree(m->nodes[i]);
    }
}

p1904_err_t
p1904_lora_virtual_set_map_file(p1904_lora_module_t *m, const char *fname)
{
    xmlDoc *doc;
    xmlNode *root_element;


    xmlInitParser();

    doc = xmlReadFile(fname, NULL, 0);
    if (!doc) {
        goto failed;
    }

    root_element = xmlDocGetRootElement(doc);

    p1904_parse_map_file(doc, root_element, m);

    xmlFreeDoc(doc);
    xmlCleanupCharEncodingHandlers();
    xmlCleanupParser();

    return P1904_OK;

failed:
    if (doc) {
        xmlFreeDoc(doc);
    }
    xmlCleanupCharEncodingHandlers();
    xmlCleanupParser();

    return P1904_FAILED;
}

void
p1904_lora_virtual_set_accuracy(p1904_lora_module_t *m, uint32_t perc)
{

}
