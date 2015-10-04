// 75HC595 pins
int dataOut_Pin = 8;   //pin 14 on the 75HC595
int latchOut_Pin = 9;  //pin 12 on the 75HC595
int clockOut_Pin = 10; //pin 11 on the 75HC595

// CD4014 pins
int dataIn_Pin = 5;  //pin 3 (Q8) on the CD4014
int clockIn_Pin = 6; //pin 10 (CLOCK) on the CD4014
int latchIn_Pin = 7; //pin 9 (P/S Clock) on the CD4014

void setup(){
  pinMode(dataOut_Pin, OUTPUT);
  pinMode(latchOut_Pin, OUTPUT);
  pinMode(clockOut_Pin, OUTPUT);

  pinMode(dataIn_Pin, INPUT);
  pinMode(clockIn_Pin, OUTPUT);
  pinMode(latchIn_Pin, OUTPUT);


  Serial.begin(9600); 
}


void setCurrentLine(int index){

  digitalWrite(latchOut_Pin, LOW);
  shiftOut(dataOut_Pin, clockOut_Pin, MSBFIRST, 1 << index);
  digitalWrite(latchOut_Pin, HIGH);
}

void scanLine()
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
  int switchVar1 = shiftIn(dataIn_Pin, clockIn_Pin, MSBFIRST);

  //Print out the results.
  //leading 0's at the top of the byte 
  //(7, 6, 5, etc) will be dropped before 
  //the first pin that has a high input
  //reading  
  Serial.println(switchVar1, BIN);

  //white space
  Serial.println("-------------------");
  //delay so all these print satements can keep up. 

}


int index = 0;

void loop(){

  //scanBoard();
  
  if (index == 8)
    index = 0;

  Serial.println(index);
  setCurrentLine(index);
  
  scanLine();
  
  index++;

  delay(500);

}

