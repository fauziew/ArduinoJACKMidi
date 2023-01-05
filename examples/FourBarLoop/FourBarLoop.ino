int barcount = 0;
int beatcount = 0;
int prevbar = 4;
//String midiSignal = "";
int playTheSameNote = 0;

unsigned short chords [4][4] = {
  {62,65,69,72},
  {57,60,64,67},
  {55,58,62,65},
  {58,62,65,69}
}; //Dm7 Am7 Gm7 Bbmaj7

void setup() {
  Serial.begin(9600); // this is lower than MIDI standard
}

void midiSend(unsigned short command, unsigned short key, unsigned short velocity) {
  Serial.write(command);
  Serial.write(key);
  Serial.write(velocity);
}

void loop() {
  // ------------ SYNTH CH 1 -------------------
    if (beatcount == 0) {
    for (int i = 0; i<4; i++) { // turn off previous notes
      playTheSameNote = 0;

      for (int j = 0; j<4; j++) {     
        if (chords[prevbar][i] == chords[barcount][j]) {
          playTheSameNote = 1;
        }
      }
      
      if (playTheSameNote == 0) {
        midiSend(128, chords[prevbar][i], 0);
      }
    }

    for(int i = 0; i<4; i++) {
      midiSend(144,chords[barcount][i],64);
    }
  }

  // ------------- BAS CH 2 ----------------------
  switch (beatcount) {
    case 0:
      midiSend(129,chords[prevbar][0],0);
      midiSend(145,chords[barcount][0],64);
      break;
    case 3: case 6:
      midiSend(145,chords[barcount][0],64);
      break;
  }

  // ------------ DRUMS CH 10 --------------------
  if (beatcount == 7) {
    midiSend(153,46,64); // open hi-hat
  } else {
    midiSend(153,42,64); // closed hi-hat
  }

  switch (beatcount) {
    case 0: case 4: case 5:
      midiSend(153,36,80); //kick
      break;
    case 2: case 6:
      midiSend(153,38,80); //snare
      break;
  }
  
  delay(250);

  if (beatcount < 7) {  // 8 beats
    beatcount++;
  } else {
    prevbar = barcount;
    barcount = (barcount < 3)? barcount + 1 : 0; //4 bars loop
    beatcount = 0;
  }
}
