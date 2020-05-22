/* Work with USART FT232RL converter
 *
 */
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h> 
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <sys/select.h>


int set_interface_attribs(int fd, int speed, int parity)
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

int set_blocking(int fd, int should_block)
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

ssize_t read_str_from_fd(int fd, char *buf, size_t len)
{
    ssize_t n;

    n = read(fd, buf, len);
    if (n < 0) {
        return -1;
    }

    return n;
}

ssize_t write_str_to_fd(int fd, const char *str)
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


int main(int argc, char *const argv[])
{
    const char *portname; /* like /dev/ttyUSB0 */
    int usart_fd;
    size_t len;
    char buf1[300];
    char buf2[300];

    struct timeval timeout;
    fd_set rfds;
    fd_set wfds;
    int ret;
    int fdmax;
    ssize_t bytes_read;
    ssize_t bytes_wrote;
    int ready_to_write = 0;

    int err;


    if (argc != 2) {
        fprintf(stderr, "%s\n", "Invalid arguments");
        return 1;
    }

    portname = argv[1];

    usart_fd = open(portname, O_RDWR|O_NOCTTY|O_SYNC);
    if (usart_fd < 0) {
        fprintf(stderr, "%s\n", "open() failed");
        return 1;
    }

    err = set_interface_attribs(usart_fd, B115200, '\n');
    if (err != EXIT_SUCCESS) {
        fprintf(stderr, "%s\n", "set_interface_attribs() failed");
        return 1;
    }
    err = set_blocking(usart_fd, 0);
    if (err != EXIT_SUCCESS) {
        fprintf(stderr, "%s\n", "set_blocking() failed");
        return 1;
    }

    while (1) {

        FD_ZERO(&rfds);
        FD_ZERO(&wfds);

        FD_SET(STDIN_FILENO, &rfds);
        if (fdmax < STDIN_FILENO) {
            fdmax = STDIN_FILENO;
        }
        FD_SET(usart_fd, &rfds);
        if (fdmax < usart_fd) {
            fdmax = usart_fd;
        }
        if (ready_to_write) {
            FD_SET(usart_fd, &wfds);
            if (fdmax < usart_fd) {
                fdmax = usart_fd;
            }
        }

        timeout.tv_sec = 30;
        timeout.tv_usec = 0;

        ret = select(fdmax + 1, &rfds, &wfds, NULL, &timeout);
        if (ret == -1) {
            if (errno != EINTR) {
                fprintf(stderr, "select() failed\n");
                return 1;
            }
            else {
                printf("interrupted by signal\n");
            }
        }
        else if (!ret) {
            printf("timeout\n");
        }

        else {
            if (FD_ISSET(STDIN_FILENO, &rfds)) {
                bytes_read = read_str_from_fd(STDIN_FILENO, buf1, sizeof(buf1));
                if (bytes_read < 0) {
                    fprintf(stderr, "%s\n", "Input string is too long");
                }
                buf1[bytes_read - 1] = '\0'; /* Remove line feed */
                ready_to_write = 1;
            }
            if (FD_ISSET(usart_fd, &rfds)) {
                bytes_read = read_str_from_fd(usart_fd, buf2, sizeof(buf2));
                if (bytes_read < 0) {
                    fprintf(stderr, "%s\n", "read() failed");
                }
                printf("%s", buf2);
            }

            if (FD_ISSET(usart_fd, &wfds)) {
                bytes_wrote = write_str_to_fd(usart_fd, buf1);
                if (bytes_wrote < 0) {
                    fprintf(stderr, "%s\n", "write() failed");
                    return 1;
                }
                ready_to_write = 0;
            }
        }
    }

    return 0;
}
