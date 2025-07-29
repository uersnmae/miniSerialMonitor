#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <termios.h>

static int openPort(void) {
	int fd = open("/dev/ttyACM0", O_RDONLY);
	if (fd < 0) {
		perror(NULL);
		return -1;
	} else {
		printf("Open Success!!\n");
		return fd;
	}
}

static int setTerminal(int fd) {
	struct termios tty;

	if (tcgetattr(fd, &tty) != 0) {
		perror("Error from tcgetattr");
		return -1;
	}

	cfsetospeed(&tty, B115200);
	cfsetispeed(&tty, B115200);

	tty.c_cflag &= ~PARENB;
	tty.c_cflag &= ~CSTOPB;
	tty.c_cflag &= ~CSIZE;
	tty.c_cflag |= CS8;
	tty.c_cflag &= ~CRTSCTS;
	tty.c_cflag |= CREAD | CLOCAL;

	tty.c_lflag &= ~ICANON;
	tty.c_lflag &= ~ECHO;
	tty.c_lflag &= ~ECHOE;
	tty.c_lflag &= ~ECHONL;
	tty.c_lflag &= ~ISIG;
	tty.c_lflag &= ~ICRNL;
	tty.c_lflag &= ~OPOST;

	if (tcsetattr(fd, TCSANOW, &tty) != 0) {
		perror("Error from tcsetattr");
		return -1;
	}
	return 0;
}

int main(void) {
	char buf[256];
	int fd = openPort();
	if (fd < 0) {
		return EXIT_FAILURE;
	}

	if (setTerminal(fd)) {
		return EXIT_FAILURE;
	}

	memset(buf, 0, 256);
	while (1) {
		if (read(fd, buf, 255) < 0) {
			perror("Error from read");
			return EXIT_FAILURE;
		} else {
			write(STDOUT_FILENO, buf, strlen(buf));
			memset(buf, 0, 256);
		}
	}
	return EXIT_SUCCESS;
}
