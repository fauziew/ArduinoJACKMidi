This program is a connector between an Arduino board connected via USB to a JACK soundserver running in a desktop computer. 
While MIDIUSB library for Arduino exists, many popular boards are not supported. This program allows these boards to act as controllers, with the desktop computer as the sound module.

--------------------------------------

Tested on
Linux distributions:
- Ubuntu Studio 22.04
- Manjaro

Arduino Boards:
- Arduino Nano
- Arduino Leonardo

Contributor:
Fauzie Wiriadisastra

License: GNU General Public License v3.0
https://gnu.org/license/gpl-3.0.en.html

----------------------------------------

Compiling:
1. Install JACK development library. This command will be different according to your Linux distribution. 
in Ubuntu: 
	\> apt-get install libjack-jackd2-dev

2. Extract the file and go to the directory
	\> tar -xvzf ArduinoJackMidi.tar.gz
	\> cd ArduinoJackMidi

3. Compile the program
	\> make

4. If you are lucky it will work in the first try. If it doesn't, don't worry as it will clean your karma.
Install any missing dependencies.

-----------------------------------------

Using the program:
1. Create the Arduino program
Use one of the example, or make your own.
The important part is for the Arduino board to send MIDI signals using Serial.write() function.
Create a midiSend function as follow:

	void midiSend(unsigned short command, unsigned short key, unsigned short velocity) {
		Serial.write(command);
		Serial.write(key);
		Serial.write(velocity);
	}

In the loop() section, you can then send the midiSignal, for example to play the note C on channel 1:

	midiSend(144, 60, 64);

Compile and upload the sketch (using Arduino IDE)

2. IMPORTANT: Exit the Arduino IDE. Make sure the associated tty device is free.

3. Run the Jackserver. You can use qjackctl for this. Make sure you can get Jack up and running.
Make sure you can get sound to work with Jack.
	\> ./qjackctl

4. Run the program in the terminal (it depends on the board and its location)
eg: \> ./ArduinoJackMidi /dev/ttyACM0

If everything is working you should be able to see:
- MIDI messages printed on the terminal
- Arduino visible in qjackctl's graph

5. Run your favorite JACK-aware software synthesizer. You can also use a DAW such as Ardour as a host for different synths.

6. In qjackctl's graph, connect MIDI out from Arduino to your Synth midi input.

7. Have fun.

----------------------------------------
resources:
Arduino		https://arduino.cc
JACK		https://jackaudio.org
MIDI spec	https://www.cs.cmu.edu/~music/cmsip/readings/davids-midi-spec.htm
Ardour		https://ardour.org
