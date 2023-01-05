#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <unistd.h>

#ifndef ARDUINOREADER_H
#define ARDUINOREADER_H
#endif

class arduino {
	public:
		const char* device;
		int serial_port;
		struct termios tty;
		//unsigned short read_buf[1];
		unsigned short read_buf;
		int dataExists;
		
		arduino(const char* _device);
		void openPort();
		void flush(); 
		void readData();
		void writeData(unsigned short message[]);
		void closePort();
};
