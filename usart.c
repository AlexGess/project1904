/* Work with USART FT232RL converter
 *
 */
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <errno.h>
#include <fcntl.h> 
#include <string.h>
#include <termios.h>
#include <unistd.h>


int set_interface_attribs(int fd, int speed, int parity)
{
    struct termios tty;
    if (tcgetattr(fd, &tty) != 0) {
        return 1;
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
    // tty.c_cflag |= parity;
    tty.c_cflag &= ~CSTOPB;
    tty.c_cflag &= ~CRTSCTS;
    tty.c_cflag &= ~PARENB;

    if (tcsetattr(fd, TCSANOW, &tty) != 0) {
        return 1;
    }
    return 0;
}

int set_blocking(int fd, int should_block)
{
    struct termios tty;
    memset(&tty, 0, sizeof tty);
    if (tcgetattr(fd, &tty) != 0) {
        return 1;
    }

    tty.c_cc[VMIN] = should_block ? 1 : 0;
    tty.c_cc[VTIME] = 5;

    if (tcsetattr(fd, TCSANOW, &tty) != 0) {
        return 1;
    }

    return 0;
}

int main(int argc, char *const argv[])
{
    const char *portname; /* like /dev/ttyUSB0 */
    int fd;
    int n;
    size_t len;
    char buf1[300];
    char buf2[300];

    if (argc != 2) {
        fprintf(stderr, "%s\n", "Invalid arguments");
        return 1;
    }

    portname = argv[1];

    printf("try to open %s\n", portname);

    fd = open(portname, O_RDWR|O_NOCTTY|O_SYNC);
    if (fd < 0) {
        fprintf(stderr, "%s\n", "open() failed");
        return 1;
    }

    if (set_interface_attribs(fd, B115200, '\n') != 0) {
        fprintf(stderr, "%s\n", "set_interface_attribs() failed");
        return 1;
    }
    if (set_blocking(fd, 0) != 0) {
        fprintf(stderr, "%s\n", "set_blocking() failed");
        return 1;
    }

    while (1) {
        printf(">> ");
        fgets(buf1, sizeof(buf1), stdin);
        len = strlen(buf1);
        if (len + 2 < sizeof(buf1)) {
            buf1[len-1] = '\r';
            buf1[len+0] = '\n';
            buf1[len+1] = '\0';
        }
        else {
            fprintf(stderr, "%s\n", "Input string is too long");
            continue;
        }

        if (write(fd, buf1, strlen(buf1)) < 0) {
            fprintf(stderr, "%s\n", "write() failed");
        }

        for (size_t i = 0; i < 5; i++) {
            n = read(fd, buf2, sizeof(buf2));
            buf2[n] = '\0';
            printf("%s", buf2, n);
        }
    }

    return 0;
}
