/*
  DC MOTOR Control Lab Firmware
  Pinrolinvic Manembu, Modified for DC Motor AND Adapted to wemos D1 mini R2,
  Oct 2018
  SOURCES Historie: 
  Jeffrey Kantor, Initial Version Temperatur Control Labs
  John Hedengren, Modified
  Oct 2017
  
  scans the serial port looking for case-insensitive commands:
  U1        set Driver 1, range 0 to 100% 
  U2        set Driver 2, range 0 to 100% 
  KC1       get Encoder data (kecepatan1), returns reading as string
  KC2       get Encoder data (kecepatan2, returns reading as string
  VER       get firmware version string
  X         stop, enter sleep mode
  LED       set LED brightnes, range 0 to 100%
*/

// constants
const String vers = "1.01-WR2";  // version of this firmware
const int baud = 9600;         // serial baud rate
const char sp = ' ';           // command separator
const char nl = '\n';          // command terminator

// pin numbers corresponding to WEMOS R2 signals on the MDC Lab Kit
const int pinQ1   = 5;         // Q1       / D1
const int pinQ2   = 4;         // Q2       / D2
const int pinE1   = 14;        // Encoder1 / D5
const int pinE2   = 12;        // Encoder2 / D6
const int pinLED  = 13;        // LED      / D7

// global variables
volatile float interruptCo = 0;// additional for motor
char Buffer[64];               // buffer for parsing serial input
String cmd;                    // command 
double pv;                     // pin value
float level;                   // LED level (0-100%)
int rps;                      // SPEED VARIABLE
float Q1 = 0;                  // value written to Q1 pin
float Q2 = 0;                  // value written to Q2 pin
int iwrite = 0;                // integer value for writing
float dwrite = 0;              // float value for writing
int n = 10;                    // number of samples for each temperature measurement

void parseSerial(void) {
  int ByteCount = Serial.readBytesUntil(nl,Buffer,sizeof(Buffer));
  String read_ = String(Buffer);
  memset(Buffer,0,sizeof(Buffer));
   
  // separate command from associated data
  int idx = read_.indexOf(sp);
  cmd = read_.substring(0,idx);
  cmd.trim();
  cmd.toUpperCase();

  // extract data. toInt() returns 0 on error
  String data = read_.substring(idx+1);
  data.trim();
  pv = data.toFloat();
}


// Command handler part
void dispatchCommand(void) {
  if (cmd == "U1") {
    Q1 = max(0.0, min(100.0, pv));
    iwrite = int(Q1 * 10); // 10.0 max
    iwrite = max(0, min(1023, iwrite));    
    analogWrite(pinQ1, iwrite);
    Serial.println(Q1);
  }
  else if (cmd == "U2") {
    Q2 = max(0.0, min(100.0, pv));
    iwrite = int(Q2 * 5); // 2.55 max
    iwrite = max(0, min(1023, iwrite));
    analogWrite(pinQ2, iwrite);
    Serial.println(Q2);
  }
  else if (cmd == "KC1") {
    pinMode(pinE1, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(pinE1), handleInterrupt, FALLING);
    delay(1000);
    rps = (interruptCo/20)*60;
    //rps = interruptCo;
    interruptCo = 0;
    Serial.println(rps);
    detachInterrupt(digitalPinToInterrupt(pinE1));
  }
  else if (cmd == "KC2") {
    pinMode(pinE1, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(pinE2), handleInterrupt, FALLING);
    delay(500);
    rps = (interruptCo/20)*60;
    //rps = interruptCo;
    interruptCo = 0;
    Serial.println(rps);
    detachInterrupt(digitalPinToInterrupt(pinE2));
  }
  else if ((cmd == "V") or (cmd == "VER")) {
    Serial.println("PIN-MDCLab Firmware Version " + vers);
  }
  else if (cmd == "LED") {
    level = max(0.0, min(100.0, pv));
    iwrite = int(level * 5.0);
    iwrite = max(0, min(500, iwrite));    
    analogWrite(pinLED, iwrite);
    Serial.println(level);
  }  
  else if (cmd == "S") {
    analogWrite(pinQ1, 0);
    analogWrite(pinQ2, 0);
    Serial.println("Stop");
  }
}

// wemos startup
void setup() {
  Serial.begin(baud); 
  while (!Serial) {
    ; // wait for serial port to connect.
  }
  analogWrite(pinQ1, 0);
  analogWrite(pinQ2, 0);
}

void handleInterrupt() {
  interruptCo++;
}

// wemos main event loop
void loop() {
  parseSerial();
  dispatchCommand(); 
}
