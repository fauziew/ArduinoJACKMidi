// blog.mbedded.ninja/programming/operating-systems/linux/linux-serial-ports-using-c-cpp/

#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <unistd.h>

#include "arduino_reader.h"

arduino::arduino(const char* _device) {
	this->device = _device;
}

void arduino::openPort() {
	this->serial_port = open(this->device, O_RDWR | O_NONBLOCK | O_EXCL);
	// Check for errors
	if (this->serial_port < 0) {
		printf("Error %i from open: %s\n", errno, strerror(errno));
	}

	// Configure tty
	if (tcgetattr(this->serial_port, &tty) != 0) {
		printf("Error %i from tcgetattr: %s\n", errno, strerror(errno));
	}

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

	tty.c_oflag &= ~OPOST;
	tty.c_oflag &= ~ONLCR;

	tty.c_cc[VTIME] = 1;
	tty.c_cc[VMIN] = 1;

	cfsetospeed(&tty, B9600);

	if (tcsetattr(this->serial_port, TCSANOW, &tty) != 0) {
		printf("Error %i from tcsetattr: %s\n", errno, strerror(errno));
	}
}

void arduino::flush() {
	tcflush(this->serial_port, TCIFLUSH);
}

void arduino::readData() {
	this->dataExists = read(this->serial_port, &read_buf, 1);
}

void arduino::writeData(unsigned short message[]) {
	write(this->serial_port, message, 1);
}

void arduino::closePort() {
	close(this->serial_port);
}
