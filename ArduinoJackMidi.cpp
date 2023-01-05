#include <iostream>
#include <jack/jack.h>
#include <jack/midiport.h>
#include <signal.h>
#ifndef WIN32
#include <unistd.h>
#endif
#include <string>

#include "arduino_reader.h"

using namespace std;

// declare variables for communicating with JACK
jack_port_t *output_port1;
jack_client_t *client;
const char **ports;
const char *client_name = "Arduino";
const char *server_name = NULL;
jack_options_t options;
jack_status_t status;
jack_nframes_t nframes;

// Global variables to connect Arduino signal to JACK MIDI
unsigned short midiSignal[100][3]; // 100 events, 3 bytes each.
int dataCount = 0;				// this is to count the number of bytes (3 bytes per MIDI command)
int signalBuffer = 0;			// this is to count the number of MIDI command in the buffer
int sendMidiSignal = 0;			// this is changed to 1 if the buffer is ready to be sent

// when JACK shutdowns
void jack_shutdown (void *arg) {
	exit (1);
}

// exit signal handler
static void signal_handler(int sig) {
	jack_client_close(client);
	fprintf(stderr, "signal received, exiting ...\n");
	exit(0);
}

static int process(jack_nframes_t nframes, void *arg)
{
    void* port_buf = jack_port_get_buffer(output_port1, nframes);
    unsigned char* buffer;
    jack_midi_clear_buffer(port_buf);

	int i = 0, j=0, Length=0, midiCounter=0;
	int midiWord;

	if (sendMidiSignal == 1) {
		for (int i = 0 ; i < signalBuffer; i++) {
			fprintf(stderr,"%d %d %d;",midiSignal[i][0],midiSignal[i][1],midiSignal[i][2]);
			buffer = jack_midi_event_reserve(port_buf, 0, 3);
			buffer[0] = midiSignal[i][0];
			buffer[1] = midiSignal[i][1];
			buffer[2] = midiSignal[i][2];
		}
		sendMidiSignal = 0;
		signalBuffer = 0;
		fprintf(stderr,"\n");
	}

    return 0;
}

void printHelp() {
	fprintf(stderr, "Usage: ./ArduinoJackMidi <device>\n");
	fprintf(stderr, "eg: ./ArduinoJackMidi /dev/ttyACM0\n");
}

int main (int argc, char *argv[]) {
	if (argc != 2) {
		printHelp();
		exit(0);
	}

	options = JackNullOption;

	// open JACK client

	client = jack_client_open (client_name, options, &status, server_name);
	if (client == NULL) {
		fprintf (stderr, "jack_client_open() failed, "
			"status = 0x%2.0x\n", status);
		if (status & JackServerFailed) {
			fprintf (stderr, "Unable to connect to JACK server\n");
		}
		exit (1);
	}
	if (status & JackServerStarted) {
		fprintf (stderr, "JACK server started\n");
	}
	if (status & JackNameNotUnique) {
		client_name = jack_get_client_name(client);
		fprintf (stderr, "unique name `%s' assigned\n", client_name);
	}

	// call this if JACK shuts down

	jack_on_shutdown (client, jack_shutdown, 0);

	// create ports
	
	output_port1 = jack_port_register (client, "midi_out_1",
					JACK_DEFAULT_MIDI_TYPE,
					JackPortIsOutput, 0);
	
	if (output_port1 == NULL) {
		fprintf(stderr, "no more JACK ports available\n");
		exit (1);
	}

	// Signal handlers
#ifdef WIN32
	signal(SIGINT, signal_handler);
	signal(SIGABRT, signal_handler);
	signal(SIGTERM, signal_handler);
#else
	signal(SIGQUIT, signal_handler);
	signal(SIGTERM, signal_handler);
	signal(SIGHUP, signal_handler);
	signal(SIGINT, signal_handler);
#endif 

	// Point jack callback to function process (defined above)
	nframes = jack_get_buffer_size(client);
	jack_set_process_callback (client, process, 0);

	// Tell JACK server we are ready
	if (jack_activate (client)) {
		fprintf (stderr, "cannot activate client");
		exit (1);
	}

	// set up Arduino MIDI device
	arduino myArduino(argv[1]);
    	myArduino.openPort();
    	sleep(1);
    	myArduino.flush();

	//unsigned short operationMode[] = {0};

	//keep running until Ctrl+C
	while (1) {
		//myArduino.writeData(operationMode);
		myArduino.readData(); 
		if (myArduino.dataExists == 1) {
			midiSignal[signalBuffer][dataCount] = myArduino.read_buf;
			if (dataCount == 2) {
				dataCount = 0;
				signalBuffer = (signalBuffer>98)? 0: signalBuffer+1; // buffer overflow
			} else {
				dataCount++;
			}
		} else if (signalBuffer > 0) {
			sendMidiSignal = 1;
		}
	}

	jack_client_close (client);
	exit (0);
}
