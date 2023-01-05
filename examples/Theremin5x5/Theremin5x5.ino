#include <NewPing.h>

const unsigned short sensorQty = 5;
const unsigned short modeQty = 5;

const unsigned short TRIG_PIN[sensorQty]={52,50, 48, 46, 44};
const unsigned short ECHO_PIN[sensorQty]={53,51, 49, 47, 45};
const unsigned int BAUD_RATE=9600;
const unsigned short LED[modeQty] = {6, 5, 4, 3, 2};
const unsigned short indicatorLED[sensorQty] = {22,24,26,28,30};
const unsigned short pushButtonPin = 13;

const unsigned int lowestEcho[sensorQty] = {300, 300, 300, 300, 300}, highestEcho[sensorQty] = {10000, 10000, 10000, 10000, 10000};
const unsigned int threshold[sensorQty] = {1000, 1000, 1000, 1000, 1000};
const unsigned int thresholdMin[sensorQty] = {100, 100, 100, 100, 100};
unsigned short selectedSensor = 0, previousSensor = 0;
unsigned short lowestVelocity = 40, highestVelocity = 124;
const unsigned short notePerChord = 4;  
unsigned short noteToPlay = notePerChord;

unsigned short selectedMode = 0, previousMode = 0;
unsigned short pushButtonState = 0;
unsigned short pushButtonPrevState = pushButtonState;

unsigned long duration[sensorQty];
NewPing* sonar = (NewPing*)malloc(sizeof(NewPing)*sensorQty);

unsigned int rangeEcho[sensorQty];
unsigned short rangeVelocity = highestVelocity - lowestVelocity;
unsigned short outputVelocity = 64, previousVelocity = 64;

const unsigned short chords[sensorQty][notePerChord] = {{57,60,64,67},
                                              {56,60,63,67},
                                              {65,69,72,76},
                                              {64,67,71,74},
                                              {62,65,69,72}};

// --------------------- SETUP FUNCTION ----------------------------------

void setup() {
  // Ultrasonic sensors
  for (int i = 0; i < sensorQty; i++) {
    rangeEcho[i] = highestEcho[i] - lowestEcho[i];
    sonar[i] = NewPing(TRIG_PIN[i], ECHO_PIN[i]);
    pinMode(indicatorLED[i], OUTPUT);
  }

  // LED
  for (int i=0; i < modeQty; i++) {
    pinMode(LED[i], OUTPUT);
  }
  turnOnLED(0);
  
  // Push Button
  pinMode(pushButtonPin, INPUT);
  Serial.begin(BAUD_RATE);
}

// -------------------- SEND MIDI SIGNAL TO SERIAL PORT --------------------

void midiSend(unsigned short command, unsigned short key, unsigned short velocity) {
  Serial.write(command);
  Serial.write(key);
  Serial.write(velocity);
}

// -------------------- TURN OFF CHORD ----------------------------

void turnOffChord(unsigned short sensorNumber, unsigned short channel = 1) {
  for (int i = 0; i < notePerChord; i++) {
    midiSend((127+channel),chords[sensorNumber][i],0);
  }
}

// ---------------------- TURN ON CHORD ------------------------------------

void turnOnChord(unsigned short sensorNumber, unsigned short velocity = 64, unsigned short channel = 1) {
  for (int i = 0; i < noteToPlay; i++) {
    midiSend((143+channel),chords[sensorNumber][i],velocity);
  }
}

// ----------------------- TURN ON LEDS UP TO numberOfLED ------------------

void turnOnLED(unsigned int numberOfLED) {
  for (int i=0; i < numberOfLED; i++) {
    digitalWrite(LED[i], HIGH);
  }
  for (int i=numberOfLED; i<modeQty; i++) {
    digitalWrite(LED[i], LOW);
  }
}

// ---------------- RETURN ADJUSTED DIFFERENCE -------------------

unsigned short calculateDiff(unsigned short largeNum, unsigned short smallNum) {
  unsigned short difference = largeNum - smallNum;
  if (difference > 20) {
    return 5;
  } else if (difference > 10) {
    return 3;
  } else if (difference > 0) {
    return 1;
  } else {
    return 0;
  }
}

// ---------------------- MAIN FUNCTION ----------------------------------

void loop() {
  
  //---- get ping distances from all sensors ----
  for (int i = 0; i < sensorQty; i++) {
    duration[i] = sonar[i].ping();

    // check for error
    if (duration[i] < thresholdMin[i]) {
      digitalWrite(indicatorLED[i], HIGH); // no ping received
    } else {
      digitalWrite(indicatorLED[i], LOW);
      // check for shortest distance
      if (duration[i] < threshold[i] && duration[i]>thresholdMin[i] ) {
        selectedSensor = i;
      }
    }
  }

  // ---- the number of note to play -----
  switch (selectedMode) {
    case 0:
      noteToPlay = 0;
      break;
    case 1:
      noteToPlay = 4;
      break;
    case 2:
      noteToPlay = 3;
      break;
    default:
      noteToPlay = 1;
      break;
  }

  // ---- determine if there is any change of sensor selection ----
  if (selectedSensor != previousSensor) {
    if (selectedMode == 4) {
      midiSend(128+previousSensor, 60, 64);
      midiSend(144+selectedSensor, 60, 64);
      midiSend(128+previousSensor, 60, 64);
      midiSend(144+selectedSensor, 60, 64);
    } else {
      if (selectedMode != 5) {
        turnOffChord(previousSensor);
        turnOffChord(previousSensor);
      }
      turnOnChord(selectedSensor);
    }
    previousSensor = selectedSensor;
  }

  // ---- calculate velocity ----
  if (duration[selectedSensor] < lowestEcho[selectedSensor]) {
      outputVelocity = highestVelocity;
  } else if (duration[selectedSensor] > highestEcho[selectedSensor]) {
      outputVelocity = lowestVelocity;
  } else {
      outputVelocity = highestVelocity - ((duration[selectedSensor]-lowestEcho[selectedSensor])*rangeVelocity)/rangeEcho[selectedSensor];
  }

  // ---- turn up / down volume ----
  
  if (outputVelocity > previousVelocity) {
    outputVelocity = (previousVelocity == highestVelocity)? highestVelocity: previousVelocity+calculateDiff(outputVelocity, previousVelocity);
  } else {
    outputVelocity = (previousVelocity == lowestVelocity)? lowestVelocity : previousVelocity-calculateDiff(previousVelocity, outputVelocity);
  }

  // ----------- check Push Button state -----------
  pushButtonState = digitalRead(pushButtonPin);
  if (pushButtonState == LOW && pushButtonPrevState == HIGH) {
    /*if (selectedMode == 4) {
      turnOffChord(0,selectedSensor+1);
    } else {*/
      turnOffChord(selectedSensor);
    //}
    selectedMode = (selectedMode == 5)? 0 : selectedMode + 1;
    turnOnLED(selectedMode);
    selectedSensor = 2; // workarround to make sure first sensor responds (except no 3)
    previousSensor = 2;
  }
  pushButtonPrevState = pushButtonState;

  // ------------ send appropriate MIDI volume signal -------

  switch (selectedMode) {
    case 1:
      midiSend(176,7,outputVelocity);
      break;
    case 2: case 3:
      midiSend(176,1,outputVelocity);
      break;
    case 4:
      midiSend(176+selectedSensor, 7, outputVelocity);
      break;

  }
  previousVelocity = outputVelocity;
 
  delay(50);
 }
