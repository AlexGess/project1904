#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h> 
#include <termios.h>

#include "p1904_lora_rak811.h"


static int p1904_set_usart_attr(int fd, int speed, int parity)
{
    struct termios tty;
    if (tcgetattr(fd, &tty) != 0) {
        return EXIT_FAILURE;
    }

    cfsetospeed(&tty, speed);
    cfsetispeed(&tty, speed);

    tty.c_cflag = (tty.c_cflag & ~CSIZE)|CS8;
    tty.c_iflag &= ~IGNBRK;
    tty.c_lflag = 0;
    tty.c_oflag = 0;
    tty.c_cc[VMIN] = 0;
    tty.c_cc[VTIME] = 5;
    tty.c_iflag &= ~(IXON|IXOFF|IXANY);
    tty.c_cflag |= (CLOCAL|CREAD);
    tty.c_cflag &= ~(PARENB|PARODD);
    tty.c_cflag &= ~CSTOPB;
    tty.c_cflag &= ~CRTSCTS;
    tty.c_cflag &= ~PARENB;

    if (tcsetattr(fd, TCSANOW, &tty) != 0) {
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

static int p1904_set_usart_blocking(int fd, int should_block)
{
    struct termios tty;
    memset(&tty, 0, sizeof tty);
    if (tcgetattr(fd, &tty) != 0) {
        return EXIT_FAILURE;
    }

    tty.c_cc[VMIN] = should_block ? 1 : 0;
    tty.c_cc[VTIME] = 5;

    if (tcsetattr(fd, TCSANOW, &tty) != 0) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

static ssize_t p1904_write_str_to_fd(int fd, const char *str)
{
    static char buf[1024];
    ssize_t len;
    ssize_t n;

    len = snprintf(buf, sizeof(buf), "%s\r\n", str);

    n = write(fd, buf, len);
    if (n < 0) {
        return -1;
    }

    return n;
}

static ssize_t p1904_read_str_from_fd(int fd, char *buf, size_t len)
{
    ssize_t n;

    n = read(fd, buf, len);
    if (n < 0) {
        return -1;
    }

    return n;
}

/* The output buffer must be at least twice as large as the input buffer */
static void p1094_convert_str_to_at_format(const char *buf_in,
    size_t buf_in_len, char *buf_out)
{
    char *ptr;

    ptr = buf_out;

    for (size_t i = 0; i < buf_in_len; i++) {
        sprintf(ptr, "%02x", buf_in[i]);
        ptr += 2; /* Hex number occupy 2 bytes */
    }
}


int p1904_lora_rak811_init(p1904_lora_module_t *m, const char *device)
{
    int fd;
    int err;

    memset(m, 0, sizeof(p1904_lora_module_t));

    fd = open(device, O_RDWR|O_NOCTTY|O_SYNC);
    if (fd < 0) {
        goto failed;
    }

    err = p1904_set_usart_attr(fd, B115200, '\n');
    if (err != EXIT_SUCCESS) {
        goto failed;
    }
    err = p1904_set_usart_blocking(fd, 0);
    if (err != EXIT_SUCCESS) {
        goto failed;
    }

    m->fd = fd;

    return EXIT_SUCCESS;

failed:
    if (fd != -1) {
        close(fd);
    }
    return EXIT_FAILURE;
}


int p1904_lora_rak811_send(p1904_lora_module_t *m, const char *addr,
    const char *str, size_t len)
{
    static char *cmd = 
        "at+mode=1\r\n"
        "at+rf_config=867700000,10,0,1,8,14\r\n"
        "at+txc=100,1000,";

    static char buf[P1904_MAX_DATA_SIZE + sizeof(cmd)];
    static char converted_data[P1904_MAX_DATA_SIZE];

    if (P1904_MAX_DATA_SIZE <= (len * 2)) {
        return EXIT_FAILURE;
    }

    p1094_convert_str_to_at_format(str, len, converted_data);

    snprintf(buf, sizeof(buf), "%s%s\r\n", cmd, converted_data);

    p1904_write_str_to_fd(m->fd, cmd);

    return EXIT_SUCCESS;
}


int p1904_lora_rak811_fini(p1904_lora_module_t *m)
{
    if (m->fd != -1) {
        close(m->fd);
    }
}
