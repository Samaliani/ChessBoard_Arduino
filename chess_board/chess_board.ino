#include <arduino2.h>

// 75HC595 pins
const int dataOut_Pin = 5;   // green pin 14 on the 75HC595
const int latchOut_Pin = 6;  // green-white pin 12 on the 75HC595
const int clockOut_Pin = 7; // blue pin 11 on the 75HC595

// CD4014 pins
const int dataIn_Pin = 2;  // blue-white pin 3 (Q8) on the CD4014
const int clockIn_Pin = 3; // orange pin 10 (CLOCK) on the CD4014
const int latchIn_Pin = 4; // orange-white pin 9 (P/S Clock) on the CD4014

const int button1_Pin = 12; // button for white
const int button2_Pin = 11; // button for black

const int boardChanged_event = 0xFF;
const int button1Pressed_event = 0xFE;
const int button2Pressed_event = 0xFD;

boolean button1Pressed;
boolean button2Pressed;
byte boardData[8];

void setup(){
  // Pins
  pinMode(dataOut_Pin, OUTPUT);
  pinMode(latchOut_Pin, OUTPUT);
  pinMode(clockOut_Pin, OUTPUT);

  pinMode(dataIn_Pin, INPUT);
  pinMode(clockIn_Pin, OUTPUT);
  pinMode(latchIn_Pin, OUTPUT);

  pinMode(button1_Pin, INPUT_PULLUP);  
  pinMode(button2_Pin, INPUT_PULLUP);  
  
  // Board data
  button1Pressed = false;
  button2Pressed = false;
  for(int i = 0; i < 8; i++)
    boardData[i] = 0;

  setNoLine();
  
  // Serial
  Serial.begin(9600); 
}

void loop(){

  checkButtons();
  checkBoard();
  
  delay(100);
}

void setNoLine() {
  
  digitalWrite(latchOut_Pin, LOW);
  shiftOut(dataOut_Pin, clockOut_Pin, LSBFIRST, 0);
  digitalWrite(latchOut_Pin, HIGH);
}

void setFirstLine(){

  // Push 1
  digitalWrite2(latchOut_Pin, LOW);
  digitalWrite2(dataOut_Pin, HIGH);
  digitalWrite2(clockOut_Pin, HIGH);
  digitalWrite2(clockOut_Pin, LOW);
  digitalWrite2(latchOut_Pin, HIGH);

  // FOR switchLine
  digitalWrite2(dataOut_Pin, LOW);
}

void switchLine() {

  // dataOut_Pin should have LOW = 0
  digitalWrite2(latchOut_Pin, LOW);
  digitalWrite2(clockOut_Pin, HIGH);
  digitalWrite2(clockOut_Pin, LOW);
  digitalWrite2(latchOut_Pin, HIGH);
}

uint8_t shiftIn2(uint8_t dataPin, uint8_t clockPin) {
  uint8_t value = 0;
  uint8_t i;

  for (i = 0; i < 8; ++i) {
    digitalWrite2(clockPin, HIGH);
    value |= digitalRead2(dataPin) << i;
    digitalWrite2(clockPin, LOW);
  }
  return value;
}

byte scanLine2()
{
  //Pulse the latch pin:
  //set it to 1 to collect parallel data
  digitalWrite2(latchIn_Pin, HIGH);
  // For cd4014
  digitalWrite2(clockIn_Pin, HIGH);
  //set it to 1 to collect parallel data, wait
  //delayMicroseconds(20);
  //set it to 0 to transmit data serially  
  digitalWrite2(latchIn_Pin, LOW);

  //while the shift register is in serial mode
  //collect each shift register into a byte
  //the register attached to the chip comes in first 
  return shiftIn2(dataIn_Pin, clockIn_Pin);
}

void setCurrentLine(int index){

  digitalWrite(latchOut_Pin, LOW);
  shiftOut(dataOut_Pin, clockOut_Pin, MSBFIRST, 1 << index);
  digitalWrite(latchOut_Pin, HIGH);
}

byte scanLine()
{
  //Pulse the latch pin:
  //set it to 1 to collect parallel data
  digitalWrite(latchIn_Pin, HIGH);
  // For cd4014
  digitalWrite(clockIn_Pin, HIGH);
  //set it to 1 to collect parallel data, wait
  delayMicroseconds(20);
  //set it to 0 to transmit data serially  
  digitalWrite(latchIn_Pin, LOW);

  //while the shift register is in serial mode
  //collect each shift register into a byte
  //the register attached to the chip comes in first 
  return shiftIn(dataIn_Pin, clockIn_Pin, LSBFIRST);
}

void checkButtons()
{
  if(digitalRead2(button1_Pin) == LOW)
  {
    if(!button1Pressed)
      sendEvent(button1Pressed_event, "");
    button1Pressed = true;
  }
  else
    button1Pressed = false;

  if(digitalRead2(button2_Pin) == LOW)
  {
    if(!button2Pressed)
      sendEvent(button2Pressed_event, "");
    button2Pressed = true;
  }
  else
    button2Pressed = false;
}


// --------------------------------------------
// Check Board Details
// --------------------------------------------

// Use setCurrentLine & scanLine
// Native arduino functions slow (3.4 uS for board scan)

// Use setFirstLine & switchLine
// Native and fast instead of shiftOut whole byte in setCurrentLine (1.4 uS for board scan)

// Use setFirstLine & switchLine & scanLine2 & shiftIn2
// DIO2 library which up to 5 times faster (event more performance is possible with using DigitalWrite2f)
// 0.5 uS


void checkBoard()
{
  byte data[8];

  int t = micros();
  setFirstLine();
  for(int i = 0; i < 8; i++)
  {
    //setCurrentLine(i);
    data[7-i] = scanLine2();
    switchLine();
  }
  int result = micros();

  boolean needSend = false;
  for(int i = 0; i < 8; i++)
    if (data[i] != boardData[i])
    {
      boardData[i] = data[i];
      needSend = true;
    }
   
  if(needSend){
    sendBoardData();
    Serial.println(result - t);
  }
}

// Events
void sendEvent(int eventId, String data)
{
  Serial.println(eventId, HEX);
  if (data != "")
    Serial.println(data);
}

void sendBoardData()
{
  String data;
  for(int i = 0; i < 8; i++){
    String line = String(boardData[i], HEX);
    if (line.length() == 1)
      line = "0" + line;
    data += line; 
  }
    
   sendEvent(boardChanged_event, data);  
}

