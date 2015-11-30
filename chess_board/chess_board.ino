// 75HC595 pins
int dataOut_Pin = 5;   // green pin 14 on the 75HC595
int latchOut_Pin = 6;  // green-white pin 12 on the 75HC595
int clockOut_Pin = 7; // blue pin 11 on the 75HC595

// CD4014 pins
int dataIn_Pin = 2;  // blue-white pin 3 (Q8) on the CD4014
int clockIn_Pin = 3; // orange pin 10 (CLOCK) on the CD4014
int latchIn_Pin = 4; // orange-white pin 9 (P/S Clock) on the CD4014

int button1_Pin = 12;
int button2_Pin = 11;

int boardChanged_event = 0xFF;
int button1Pressed_event = 0xFE;
int button2Pressed_event = 0xFD;

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

  // Serial
  Serial.begin(9600); 
}

void loop(){

  checkButtons();
  
  // Takes about 20 millis
  checkBoard();
  
  
  delay(100);

}

void setCurrentLine(int index){

  digitalWrite(latchOut_Pin, LOW);
  shiftOut(dataOut_Pin, clockOut_Pin, LSBFIRST, 1 << index);
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
  if(digitalRead(button1_Pin) == LOW)
  {
    if(!button1Pressed)
      sendEvent(button1Pressed_event, "");
    button1Pressed = true;
  }
  else
    button1Pressed = false;

  if(digitalRead(button2_Pin) == LOW)
  {
    if(!button2Pressed)
      sendEvent(button2Pressed_event, "");
    button2Pressed = true;
  }
  else
    button2Pressed = false;
}

void checkBoard()
{
  byte data[8];
  for(int i = 0; i < 8; i++)
  {
    setCurrentLine(i);
    data[i] = scanLine();
  }
  
  boolean needSend = false;
  for(int i = 0; i < 8; i++)
    if (data[i] != boardData[i])
    {
      boardData[i] = data[i];
      needSend = true;
    }
   
  if(needSend)
    sendBoardData();
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

