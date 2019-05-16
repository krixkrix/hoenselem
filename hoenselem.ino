
const int GreenLED = 11;
const int RedLED = 12;
const int ButtonPin = 13;

const int LightSensorPin = A0;
const int PotentiometerPin = A1;

int lightValue = 0;
int potValue = 0;

const int openAbove = 600;
const int closeBelow = 300;
const int VotesRequired = 10;
int nVotes = 0;

const int DoorClosed = 0;
const int DoorOpen = 1;
const int DoorMoving = -1;
const int DoorUnknown = -2;

int doorState = DoorUnknown;

const int loopSleep = 10;
int ms = 0;

void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(GreenLED, OUTPUT);
  digitalWrite(GreenLED, LOW);
  pinMode(RedLED, OUTPUT);
  digitalWrite(RedLED, LOW);
  Serial.begin(9600);

  // detect doorstate
  doorState = DoorUnknown;
}

// main loop
void loop() {
  delay(loopSleep);
  if (ms%1000 == 0){
  
    lightValue = analogRead(LightSensorPin);
    potValue = analogRead(PotentiometerPin);
    Serial.print("light: ");
    Serial.print(lightValue);
    Serial.print("  pot: ");
    Serial.println(potValue);

    if (doorState != DoorClosed && lightValue < closeBelow) {
      nVotes++;
      if (nVotes == VotesRequired) {
        closeDoor();
        nVotes = 0;
      }
    }
    else if (doorState != DoorOpen && lightValue > openAbove) {
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
  
  if (doorState == DoorUnknown && ms%250 == 0) {
    digitalWrite(GreenLED, ms%500);
    digitalWrite(RedLED, ms%500);
  }

  if (digitalRead(ButtonPin) == 1) {
    if (doorState == DoorOpen) {
      closeDoor();
    }
    else {
      openDoor();
    }
  }
  
  ms += loopSleep;
  if (ms >= 1000){
    ms = 0;
  }
}

void openDoor() {

  Serial.println("opening door");
  doorState = DoorMoving;
  digitalWrite(RedLED, LOW);
  // simulate movement
  for (int i = 0; i < 10; i++) {
    digitalWrite(GreenLED, LOW);
    delay(500);
    digitalWrite(GreenLED, HIGH);
    delay(500);
  }
  doorState = DoorOpen;
  Serial.println("door open");
}

void closeDoor() {
  Serial.println("closing door");
  doorState = DoorMoving;
  digitalWrite(GreenLED, LOW);
  // simulate movement
  for (int i = 0; i < 10; i++) {
    digitalWrite(RedLED, LOW);
    delay(500);
    digitalWrite(RedLED, HIGH);
    delay(500);
  }
  doorState = DoorClosed;
  Serial.println("door closed");
}
