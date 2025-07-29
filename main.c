#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/select.h>

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
	printf("Serial monitor started. Press Ctrl + C to exit.\n");

	memset(buf, 0, 256);
	while (1) {
		fd_set read_fds;
		FD_ZERO(&read_fds);
		FD_SET(STDIN_FILENO, &read_fds);
		FD_SET(fd, &read_fds);

		if (select(fd + 1, &read_fds, NULL, NULL, NULL) < 0) {
			perror("Error from select");
			return EXIT_FAILURE;
		}
		if (FD_ISSET(STDIN_FILENO, &read_fds)) {
			char write_buf[256];
			int len = read(STDIN_FILENO, write_buf, sizeof(write_buf));
			if (len > 0) {
				write(fd, write_buf, len);
			}
		}
		if (FD_ISSET(fd, &read_fds)) {
			char read_buf[256];
			int len = read(fd, read_buf, sizeof(read_buf));
			if (len > 0) {
				write(STDIN_FILENO, read_buf, len);
			}
		}
	}
	close(fd);
	return EXIT_SUCCESS;
}
