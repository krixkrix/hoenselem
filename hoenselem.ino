
const int GreenLED = 11;
const int RedLED = 12;

const int ButtonPin = 13;
const int LimitTopPin = 7;
const int LimitBotPin = 8;

const int LightSensorPin = A0;

// motor
const int ControlPin1 = 2;
const int ControlPin2 = 3;
const int EnablePin = 10;

// light sensor
int lightValue = 0;
const int OpenAbove = 600;
const int CloseBelow = 300;
const int VotesRequired = 10;
int nVotes = 0;

// door states
const int DoorClosed = 0;
const int DoorOpen = 1;
const int DoorMoving = -1;
const int DoorUnknown = -2;
const int DoorError = -3;
int doorState = DoorUnknown;

const int DoorTimeoutMs = 30000;
const int Speed = 100;  // Speed 100 is around 8sec/rev which is roughly 1cm/sec with the black rubber roll

// loop resolution
// no action may block without itself providing a check for input with this interval
const int LoopSleep = 10;

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(GreenLED, OUTPUT);
  digitalWrite(GreenLED, LOW);
  pinMode(RedLED, OUTPUT);
  digitalWrite(RedLED, LOW);
  Serial.begin(9600);

  // detect doorstate
  doorState = DoorUnknown;

  pinMode(LimitTopPin, INPUT);
  pinMode(LimitBotPin, INPUT);
  pinMode(ControlPin1, OUTPUT);
  pinMode(ControlPin2, OUTPUT);
  pinMode(EnablePin, OUTPUT);
  analogWrite(EnablePin, 0);
}

// main loop
void loop() {
  static int ms = 0;
  if (ms%1000 == 0){
  
    lightValue = analogRead(LightSensorPin);
    Serial.print("light: ");
    Serial.println(lightValue);
    
    openDoorIfNeeded();
    closeDoorIfNeeded();
  }
 
  if (doorState == DoorUnknown && ms%500 == 0) {
    toggleRedAndGreen();
  }

  if (digitalRead(ButtonPin) == 1) {
    if (doorState == DoorOpen) {
      closeDoor();
    }
    else {
      openDoor();
    }
  }

  delay(LoopSleep);
  ms += LoopSleep;
  if (ms >= 1000){
    ms = 0;
  }
}

void closeDoorIfNeeded() {
  if (doorState != DoorClosed && lightValue < CloseBelow) {
    nVotes++;
    if (nVotes == VotesRequired) {
      closeDoor();
      nVotes = 0;
    }
  }
}

void openDoorIfNeeded() {
  if (doorState != DoorOpen && lightValue > OpenAbove) {
    nVotes++;
    if (nVotes == VotesRequired) {
      openDoor();
      nVotes = 0;
    }
  }
  else {
    nVotes = 0;
  }
}

void openDoor() {
  Serial.println("opening door");
  digitalWrite(RedLED, LOW);
  startMove(1);
  doorState = DoorMoving;
  for (int dt = 0; dt < DoorTimeoutMs; dt+=LoopSleep) {
    if (dt%500 == 0) {
      toggleGreen();
    }

    if (digitalRead(ButtonPin) == 1 && dt > 1000) {
      break;
    }
    delay(LoopSleep);
  }
  stopMove();
  doorState = DoorOpen;
  digitalWrite(GreenLED, HIGH);
  Serial.println("door open");
  delay(1000);
}

void closeDoor() {
  Serial.println("closing door");
  digitalWrite(GreenLED, LOW);
  doorState = DoorMoving;
  startMove(0);
  for (int dt = 0; dt < DoorTimeoutMs; dt+=LoopSleep) {
    if (dt%500 == 0) {
      toggleRed();
    }
    if (digitalRead(ButtonPin) == 1 && dt > 1000) {
      break;
    }
    delay(LoopSleep);
  }
  stopMove();
  doorState = DoorClosed;
  digitalWrite(RedLED, HIGH);
  Serial.println("door closed");
  delay(1000);
}

void startMove(int direction) {
  digitalWrite(ControlPin1, direction);
  digitalWrite(ControlPin2, !direction);
  analogWrite(EnablePin, Speed);
}

void stopMove() {
  analogWrite(EnablePin, 0);
}

void toggleRed() {
  digitalWrite(RedLED, !digitalRead(RedLED));
}

void toggleGreen() {
  digitalWrite(GreenLED, !digitalRead(GreenLED));
}

void toggleRedAndGreen() {
  digitalWrite(RedLED, !digitalRead(GreenLED));
  digitalWrite(GreenLED, !digitalRead(GreenLED));
}
