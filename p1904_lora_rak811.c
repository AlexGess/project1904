#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h> 
#include <termios.h>

#include "p1904_lora_rak811.h"

#define P1904_DELAY_TIME  3000000


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
    ssize_t n;

    n = write(fd, str, strlen(str));
    if (n < 0) {
        return -1;
    }

    return n;
}

static ssize_t p1904_write_str_to_fd_crlf(int fd, const char *str)
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

static ssize_t p1904_read_str_from_fd(int fd, char *buf, size_t size)
{
    ssize_t n;

    n = read(fd, buf, size);
    if (n < 0) {
        return -1;
    }

    return n;
}

static char *p1904_lora_rak811_extract_data(void *buf, size_t *len_out)
{
    char *token;
    char *save_ptr;
    size_t len;

    token = strtok_r(buf, ",", &save_ptr);

    for (size_t i = 0; token; i++) {
        if (i == 3) {
            len = strlen(token);
            token[len-2] = '\0'; /* Remove CRLF */
            *len_out = len;
            return token;
        }
        token = strtok_r(NULL, ",", &save_ptr);

    }
    return NULL;
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

static void p1094_convert_at_format_to_str(char *buf_in,
    size_t buf_in_len, char *buf_out)
{
    char *ptr;
    char temp_buf[3];

    ptr = buf_in;
    temp_buf[2] = '\0';

    for (size_t i = 0; i < (buf_in_len / 2); i++) {
        memmove(temp_buf, ptr, 2);
        buf_out[i] = strtol(temp_buf, NULL, 16);
        ptr += 2; /* Hex number occupy 2 bytes */
    }
}

int p1904_lora_rak811_init(p1904_lora_module_t *m, const char *device)
{
    int fd;
    int err;

    memset(m, 0, sizeof(p1904_lora_module_t));

    fd = open(device, O_RDWR|O_NOCTTY|O_SYNC);
    // fd = STDIN_FILENO;
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

int p1904_lora_rak811_activate(p1904_lora_module_t *m)
{
    ssize_t bytes_wrote;
    static char *cmd = 
        "at+mode=1\r\n"
        "at+rf_config=867700000,10,0,1,8,14\r\n";
    
    bytes_wrote = p1904_write_str_to_fd(m->fd, cmd);
    if (bytes_wrote < 0) {
        return EXIT_FAILURE;
    }

    usleep(P1904_DELAY_TIME);

    m->active = 1;

    return EXIT_SUCCESS;
}

ssize_t p1904_lora_rak811_send(p1904_lora_module_t *m, void *data, size_t len)
{
    const char *cmd = "at+txc=1,1000,";
    static char converted_data[P1904_MAX_DATA_SIZE];

    if (P1904_MAX_DATA_SIZE <= (len * 2)) {
        return -1;
    }

    p1094_convert_str_to_at_format((const char *) data, len, converted_data);

    p1904_write_str_to_fd(m->fd, cmd);
    p1904_write_str_to_fd_crlf(m->fd, converted_data);
    printf("%s\n", converted_data);

    return len;
}



ssize_t p1904_lora_rak811_recv(p1904_lora_module_t *m, void *buf, size_t size)
{
    const char *cmd = "at+rxc=1\r\n";
    static char buf1[1024];
    ssize_t bytes_read;
    ssize_t bytes_wrote;
    size_t len;

    char *data;
    uint8_t *byte_ptr;


    bytes_wrote = p1904_write_str_to_fd(m->fd, cmd);
    if (bytes_wrote < 0) {
        return EXIT_FAILURE;
    }

    while (1) {
        bytes_read = p1904_read_str_from_fd(m->fd, buf1, sizeof(buf1));
        buf1[bytes_read] = '\0';
        if (bytes_read) {  
            data = p1904_lora_rak811_extract_data(buf1, &len);
            if (data && len <= (size * 2)) {
                printf("%s\n", data);

                p1094_convert_at_format_to_str(data, len, buf);

                return len;
            }
        }
    }
}

void p1904_lora_rak811_fini(p1904_lora_module_t *m)
{
    if (m->fd != -1) {
        close(m->fd);
    }
}
