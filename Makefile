all:
	g++ arduino_reader.cpp ArduinoJackMidi.cpp -o ArduinoJackMidi `pkg-config --libs jack`

clean:
	$(RM) ArduinoJackMidi
