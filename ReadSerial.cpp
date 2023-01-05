// blog.mbedded.ninja/programming/operating-systems/linux/linux-serial-ports-using-c-cpp/

#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <unistd.h>

#include "arduino_midi_device.h"

int main() {
	midiDevice myDevice("/dev/ttyACM0");
	myDevice.openPort();
	sleep(1);
	myDevice.flush();

	while (1) {
		myDevice.readData();
		if (myDevice.dataExists == 1) {
			fprintf(stderr,"%s",myDevice.read_buf);
		}
	}

	myDevice.closePort();
	return 0;
}
