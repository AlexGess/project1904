#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <libxml/parser.h>
#include <libxml/tree.h>

#include "p1904_random.h"
#include "p1904_lora_virtual.h"


static void
p1904_parse_node(xmlDoc *doc, xmlNode *node, p1904_lora_module_t *module,
    uint32_t valid_host)
{
    xmlNode *cur;
    char *str;


    for (cur = node; cur; cur = cur->next) {

        if (!strcmp((char *) cur->name, "host")) {
            str = (char *) xmlGetProp(cur, (uint8_t *) "name");
            if (!strcmp(str, module->device)) {
                valid_host = 1;
            }
            else {
                valid_host = 0;
            }
            xmlFree(str);
        }
        else if (!strcmp((char *) cur->name, "node") && valid_host) {
            str = (char *) xmlNodeGetContent(cur);
            module->nodes[module->n_nodes] = str;
            module->n_nodes += 1;
        }

        p1904_parse_node(doc, cur->children, module, valid_host);
    }
}

static p1904_err_t
p1904_parse_map_file(xmlDoc *doc, xmlNode *node, p1904_lora_module_t *module)
{
    p1904_parse_node(doc, node, module, 0);
    return P1904_OK;
}

p1904_err_t
p1904_lora_virtual_init(p1904_lora_module_t *module, const char *device)
{
    int fd, flags;

    memset(module, 0, sizeof(p1904_lora_module_t));

    unlink(device);

    if (mkfifo(device, 0666) == -1) {
        fprintf(stderr, "mkfifo(%s) failed\n", device);
        return P1904_FAILED;
    }

    fd = open(device, O_RDONLY|O_NONBLOCK);
    // fd = open(device, O_RDONLY);
    if (fd == -1) {
        fprintf(stderr, "open(%s) failed\n", device);
        return P1904_FAILED;
    }

    // flags = fcntl(fd, F_GETFL);

    // if (fcntl(fd, F_SETFL, flags & (~O_NONBLOCK)) == -1) {
    //     fprintf(stderr, "fcntl() failed\n");
    //     return P1904_FAILED;
    // }

    module->fd = fd;
    module->device = device;

    p1904_init_random();

    return P1904_OK;
}

p1904_err_t
p1904_lora_virtual_activate(p1904_lora_module_t *module)
{
    module->active = 1;
    return P1904_OK;
}

ssize_t
p1904_lora_virtual_send(p1904_lora_module_t *module, uint8_t *data, size_t len)
{
    int fds[module->n_nodes];
    size_t i, j, n;

    for (i = 0; i < module->n_nodes; ++i) {
        fds[i] = open(module->nodes[i], O_WRONLY|O_APPEND, 0666);
        if (fds[i] == -1) {
            fprintf(stderr, "open(%s) failed\n", module->nodes[i]);
            continue;
        }
    }

    for (i = 0; i < len; ++i) {
        for (j = 0; j < module->n_nodes; ++j) {
            n = write(fds[j], data + i, 1);
            if (n != 1) {
                // fprintf(stderr, "write() to file %s failed\n",
                //     module->nodes[j]);
                continue;
            }
        }
    }

    for (i = 0; i < module->n_nodes; ++i) {
        close(fds[i]);
    }

    return len;
}

ssize_t
p1904_lora_virtual_recv(p1904_lora_module_t *module, uint8_t *buf, size_t size)
{
    return read(module->fd, buf, size);
}

void
p1904_lora_virtual_fini(p1904_lora_module_t *module)
{
    size_t i;

    module->active = 0;

    for (i = 0; i < module->n_nodes; ++i) {
        xmlFree(module->nodes[i]);
    }
}

p1904_err_t
p1904_lora_virtual_set_map_file(p1904_lora_module_t *module, const char *fname)
{
    xmlDoc *doc;
    xmlNode *root_element;


    xmlInitParser();

    doc = xmlReadFile(fname, NULL, 0);
    if (!doc) {
        goto failed;
    }

    root_element = xmlDocGetRootElement(doc);

    p1904_parse_map_file(doc, root_element, module);

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
p1904_lora_virtual_set_accuracy(p1904_lora_module_t *module, uint32_t perc)
{

}
