// 75HC595 pins
int dataOut_Pin = 8;   //pin 14 on the 75HC595
int latchOut_Pin = 9;  //pin 12 on the 75HC595
int clockOut_Pin = 10; //pin 11 on the 75HC595

// CD4014 pins
int dataIn_Pin = 5;  //pin 3 (Q8) on the CD4014
int clockIn_Pin = 6; //pin 10 (CLOCK) on the CD4014
int latchIn_Pin = 7; //pin 9 (P/S Clock) on the CD4014

int boardChanged_event = 0xFF;

byte boardData[8];

void setup(){
  // Pins
  pinMode(dataOut_Pin, OUTPUT);
  pinMode(latchOut_Pin, OUTPUT);
  pinMode(clockOut_Pin, OUTPUT);

  pinMode(dataIn_Pin, INPUT);
  pinMode(clockIn_Pin, OUTPUT);
  pinMode(latchIn_Pin, OUTPUT);

  // Board data
  for(int i = 0; i < 8; i++)
    boardData[i] = 0;

  // Serial
  Serial.begin(9600); 
}

void loop(){

  // Takes about 20 millis
  checkBoard();
  
  
  delay(100);

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
  return shiftIn(dataIn_Pin, clockIn_Pin, MSBFIRST);
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
  Serial.println(data);
}

void sendBoardData()
{
  String data;
  for(int i = 0; i < 8; i++)
    data += String(boardData[i], HEX);
    
   sendEvent(boardChanged_event, data);  
}



