FourBarLoop:

This program will send a four bar loop from Arduino. It does not require any additional component.

1. Upload the sketch to your Arduino board, then run the ArduinoJackMidi from your computer. 

2. Open Ardour and create an empty MIDI track with no instrument. Name it 'Midi in'.

3. Using qjackctl's graph, connect Arduino's midi out to Ardour's 'Midi in'. 
You should see the graph in the midi in responding by moving up and down.

4. In Ardour, create 3 empty MIDI buses, with no instrument. 
Call them 'Channel 1', 'Channel 2', and 'Channel 10'
Using the Routing Grid dialog from the output section  at the bottom of the mixer, 
connect the Midi in channel output towards all 3 MIDI buses.
Now you should see all 3 channels responding.

5. Add channel filter plugins to the 3 channels above, opening only associated channels.
If it is not available, install x-42 plugins.

6. Add MIDI chromatic transpose to Channel 2. Set it to -12.

7. Create another 3 channels. Call them 'synth', 'bas', and 'drums'

8. Route the output of channel 1 to 'synth', channel 2 to 'bas', and channel 3 to 'drums'

9. Add appropriate synths to each channels. 
eg: Surge for 'synth', fluidsynth with Ch 2 set to bas for 'bas', Black Pearl Drums to 'drums'

-----------------------------------------------------------------------------------------------

Theremin

This program use 2 Ultrasonic sensors (can be easily expanded) to control chords and volume.

1. Connect 2 Ultrasonic sensors to Arduino. Read the sourcecode for pin numbers.

2. Upload the sketch to your Arduino board, then run the ArduinoJackMidi from your computer. 

3. Use Ardour or Carla as a host, load a synthesizer.

4. Using qjackctl's graph, connect Arduino's midi out to Ardour's 'Midi in'. 

5. Play music

-----------------------------------------------------------------------------------------------
