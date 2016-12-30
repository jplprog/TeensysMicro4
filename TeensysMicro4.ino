/* 
 ---- Teensy ----
 Control Teensy Micro 4 bits
 Base: Thomas Ouellet Fredericks 
 Additions: Alexandre Quessy and Jean-Pierre Lessard
 Date: 20 march 2015
 */

#include <AltSoftSerial.h>

#define OFF 0
#define ON 1
#define ledPinIn 13 // intern
#define ledPinOut 6 // 8 board
#define STATENULL 0
#define STATEBEGIN 1
#define STATECOMMAND 2
#define STATEDATAMSB 3
#define STATEDATALSB 4
#define STATEEND 5
#define CODEACK '!'
#define CODECLEAR ' '
#define CODEBEGIN '<'
#define CODEEND '>'
#define CODECOMMENTBEGIN '('
#define CODECOMMENTEND ')'
#define CODELF 10
#define CODECR 13
#define CODEPOINT '.'
#define CASEREGX 0xF0
#define CASEREGY 0xF1
#define CASEREGACC 0xF2

// pin definitions
int nLedPins[] = {8, 9, 10, 13};
int nPinTx = 21;
int nPinRx = 20;
int nPinBuzzerN = 5;
int nPinBuzzerP = 6;
int nPinMotor = 7;
int nPinRed = 8;
int nPinBlue = 9;
int nPinGreen = 10;
int nPinButton = 11;
int nPinVcc = 12;
int nPinLed = 13;
int nPinSwitch = 14;
int nPinAnalog = 1;
int nPinTemperature = 1;
int nPinLight = 2;
int nPinOutput1 = 19;
int nPinOutput2 = 19; //20;
int nPinOutput3 = 19; //21;
int nPinOutput4 = 19; //22;
int nPinOutput5 = 23;

// serial variables
char cChar = 0;
char cCommand = 0;
int nBuild = 0;
int nBaud = 9600;
int nCount = 0;
int nEcho = 0;
int nLoop = 0;
int nState = 0;
int nDataMsb = 0;
int nDataLsb = 0;
int nLedState = 0;
int nDirect = OFF;
int nMemoire = OFF;
int nRegistre = ON;
int nStep = 0;

// micro variables
#define MAXMEM 0x10 * 0x10
int nMemory[MAXMEM];
int nPage = 0;
int nPc = 0;
int nX = 0;
int nY = 0;
int nAcc = 0;
int nFlag = 0;

// local variables
byte nPinStateIn = 0;
byte nPinStateOut = 0;
AltSoftSerial altSerial; // rx = 20 (A6), tx = 21 (A7), pwm = 22 (A8)

void setup(void);
int messageBuild(void);
void messageClear(void);
void toggleIn(int nPinNum);
void toggleOut(int nPinNum);
void clearMemory(void);
void displayStep(void);
void displayRegister(void);
void displayMemory(void);
void teston(void);
void runProgram(void);
void execIncrement(void);
void execDecrement(void);

void setup(void)
{
  // The following command initiates the serial port at 9600 baud. Please note this is VERY SLOW!!!!!! 
  // I suggest you use higher speeds in your own code. You can go up to 115200 with the USB version, that's 12x faster
  pinMode(nPinVcc, INPUT); // set the vcc as an input
  pinMode(nPinSwitch, INPUT); // set the switch as an input
  digitalWrite(nPinSwitch, HIGH); // enable the pull-up resistor
  pinMode(nPinButton, INPUT); // set the button as an input
  digitalWrite(nPinButton, HIGH); // enable the pull-up resistor  
  pinMode(nPinLed, OUTPUT); // led
  digitalWrite(nPinLed, HIGH); // set the led on
  pinMode(nPinBuzzerP, OUTPUT); // buzzer
  digitalWrite(nPinBuzzerP, LOW); // set the buzzer off
  pinMode(nPinBuzzerN, OUTPUT); // buzzer
  digitalWrite(nPinBuzzerN, LOW); // set the buzzer off   
  pinMode(nPinMotor, OUTPUT); // motor
  digitalWrite(nPinMotor, HIGH); // set the motor on  
  pinMode(nPinRed, OUTPUT); // led red
  digitalWrite(nPinRed, LOW); // set the led red off
  pinMode(nPinBlue, OUTPUT); // led blue
  digitalWrite(nPinRed, LOW); // set the led blue off
  pinMode(nPinGreen, OUTPUT); // led green
  digitalWrite(nPinGreen, LOW); // set the led green off 
  pinMode(nPinOutput1, OUTPUT); // output 1
  digitalWrite(nPinOutput1, HIGH); // set the output 1 on 
  pinMode(nPinOutput2, OUTPUT); // output 2
  digitalWrite(nPinOutput2, HIGH); // set the output 2 on
  pinMode(nPinOutput3, OUTPUT); // output 3
  digitalWrite(nPinOutput3, HIGH); // set the output 3 on
  pinMode(nPinOutput4, OUTPUT); // output 4
  digitalWrite(nPinOutput4, HIGH); // set the output 4 on
  pinMode(nPinOutput5, OUTPUT); // output 5
  digitalWrite(nPinOutput5, HIGH); // set the output 5 on 
  digitalWrite(nPinRed, LOW); // set the led red off
  // Switch
  pinMode(nPinSwitch, INPUT); // set the switch as an input
  digitalWrite(nPinSwitch, HIGH); // enable the pull-up resistor
  // Button
  pinMode(nPinButton, INPUT); // set the pin as an input
  digitalWrite(nPinButton, HIGH); // enable the pull-up resistor
  // Buzzer
  pinMode(nPinBuzzerP, OUTPUT); // set both buzzer pins as outputs
  pinMode(nPinBuzzerN, OUTPUT);  
  pinMode(ledPinIn, OUTPUT);
  pinMode(ledPinOut, OUTPUT);
  pinMode(nPinRx, INPUT);
  pinMode(nPinTx, OUTPUT);  
  digitalWrite(ledPinIn, HIGH);
  delay(1000);
  digitalWrite(ledPinIn, LOW);
  delay(1000);
  digitalWrite(ledPinIn, HIGH);
  nCount = 0; nEcho = OFF; 
  messageClear(); clearMemory();
  altSerial.begin(9600);
  altSerial.println("Welcome TeensyMicro4");
}

void messageClear(void)
{
  nState = STATENULL; cCommand = 0; 
}

int messageBuild(void) 
{
  if (altSerial.available()) {
    cChar = (char)altSerial.read();
    altSerial.flush();
    if (cChar == CODECOMMENTBEGIN) {
      nBuild = 1; return 0;
    }
    else if (cChar == CODECOMMENTEND) {
      nBuild = 0; return 0;
    }
    if (nBuild == 1) return 0;
    switch (cChar) {
     case CODECLEAR:
      messageClear(); break;
     case CODEBEGIN:
      if (nState == OFF) nState = STATEBEGIN;
      if (nEcho == ON) altSerial.print(cChar);
      break; 
     case CODEEND:
      if (nState == STATEDATALSB) nState = STATEEND;
      if (nEcho == ON) altSerial.print(cChar);    
      break;
     case CODEPOINT:
      displayStep(); break;
     case CODECR: 
     case CODELF: break;
     default: // another
      if (nState == STATEBEGIN) {
        cCommand = cChar; nState = STATECOMMAND;
      }
      else {
        if (nState == STATECOMMAND) {
          switch (cChar) {
            case '0': nDataMsb = 0; break;
            case '1': nDataMsb = 1; break;
            case '2': nDataMsb = 2; break;
            case '3': nDataMsb = 3; break;            
            case '4': nDataMsb = 4; break;
            case '5': nDataMsb = 5; break;
            case '6': nDataMsb = 6; break;
            case '7': nDataMsb = 7; break;
            case '8': nDataMsb = 8; break;
            case '9': nDataMsb = 9; break;
            case 'a': nDataMsb = 10; break; 
            case 'b': nDataMsb = 11; break; 
            case 'c': nDataMsb = 12; break; 
            case 'd': nDataMsb = 13; break; 
            case 'e': nDataMsb = 14; break;
            case 'f': nDataMsb = 15; break;                                                                                                                         
            default: nDataMsb = 20; break;
          }
          nState = STATEDATAMSB;
        }
        else if (nState == STATEDATAMSB) {
          switch (cChar) {
            case '0': nDataLsb = 0; break;
            case '1': nDataLsb = 1; break;
            case '2': nDataLsb = 2; break;
            case '3': nDataLsb = 3; break;
            case '4': nDataLsb = 4; break;
            case '5': nDataLsb = 5; break;
            case '6': nDataLsb = 6; break;
            case '7': nDataLsb = 7; break;
            case '8': nDataLsb = 8; break; 
            case '9': nDataLsb = 9; break;
            case 'a': nDataLsb = 10; break;
            case 'b': nDataLsb = 11; break;
            case 'c': nDataLsb = 12; break; 
            case 'd': nDataLsb = 13; break;
            case 'e': nDataLsb = 14; break;
            case 'f': nDataLsb = 15; break;                                                                                                                                             
            default: nDataLsb = 20; break;
          }
          nState = STATEDATALSB;
        }
      }
      if (nEcho == ON) altSerial.print(cChar); break;
    }
  }
  return nState;
}

void loop(void)
{
  int nTest = OFF;
  if (nTest == ON) teston();
  if (messageBuild() == 5) {
    switch (cCommand) { // Gets the first word as a character
      case 'h': // Help
        help( ); messageClear(); altSerial.print(CODEACK); break;
      case 'd': // Display memory
        displayMemory(); displayRegister(); messageClear(); break;
      case 'c': // Config
        switch (nDataLsb) {
          case 0: nEcho = OFF; break;
          case 1: nEcho = ON; break;
          case 2: nStep = OFF; break;
          case 3: nStep = ON; break;
          case 4: nDirect = OFF; break;
          case 5: nDirect = ON; break;
          default: break;  
        }
        messageClear(); altSerial.print(CODEACK); break; 
      case 'n': // New program
        clearMemory();
        if (nDataMsb < 0x0F) nPage = nDataMsb;
        else nPage = 0;
        nPc = nDataLsb; messageClear(); altSerial.print(CODEACK); break;
      case 'p': // Page and pc
        if (nDataMsb < 0x0F) nPage = nDataMsb;
        else nPage = 0;
        nPc = nDataLsb; messageClear(); altSerial.print(CODEACK); break;   
      case 's': // Store data
        switch (nPage) {
          case 0:
            nMemory[0x00 + nDataMsb] = nDataLsb; break;
          case 1:
            nMemory[0x10 + nDataMsb]= nDataLsb; break;
          case 2:
            nMemory[0x20 + nDataMsb] = nDataLsb; break;  
          case 3:
            nMemory[0x30 + nDataMsb] = nDataLsb; break; 
          case 4:
            nMemory[0x40 + nDataMsb] = nDataLsb; break;
          case 5:
            nMemory[0x50 + nDataMsb] = nDataLsb; break;
          case 6:
            nMemory[0x60 + nDataMsb] = nDataLsb; break;
          case 7:
            nMemory[0x70 + nDataMsb] = nDataLsb; break;
          case 8:
            nMemory[0x80 + nDataMsb] = nDataLsb; break; 
          case 9:
            nMemory[0x90 + nDataMsb] = nDataLsb; break;   
          case 10:
            nMemory[0xA0 + nDataMsb] = nDataLsb; break; 
          case 11:
            nMemory[0xB0 + nDataMsb] = nDataLsb; break; 
          case 12:
            nMemory[0xC0 + nDataMsb] = nDataLsb; break; 
          case 13:
            nMemory[0xD0 + nDataMsb] = nDataLsb; break; 
          case 14:
            nMemory[0xE0 + nDataMsb] = nDataLsb; break;                                                                    
          default: break;                     
        }
        messageClear(); altSerial.print(CODEACK); break;
      case 'r': // run
        runProgram(); messageClear(); altSerial.print(CODEACK); break;      
      case 'u': // Read pins (analog or digital)
        readpins( ); messageClear(); break;
      case 't': // Test card
        messageClear(); altSerial.print(CODEACK); teston(); break; 
      case 'v': // Version
        altSerial.println("0.7"); messageClear(); altSerial.print(CODEACK); break;      
      case 'w': // Write pin
        writepin( ); messageClear(); altSerial.print(CODEACK); break;
      case 'l': // Toogle led
        if (nLedState == OFF) {
          digitalWrite(nPinLed, HIGH); nLedState = ON;
        } 
        else {
          digitalWrite(nPinLed, LOW); nLedState = OFF;
        }  
        messageClear(); altSerial.print(CODEACK); break;
      default: break;  
    }
  }
}

void runProgram(void)
{
  int nOpcode = 0;
  int nParam = 0;
  int nLoop = 1;
  while (nLoop == 1) {
    nOpcode = nMemory[nPc]; nPc++;
    nParam = nMemory[nPc]; nPc++;
    switch (nPc) {
      case 0x00: nPage = 0x00; break;
      case 0x10: nPage = 0x01; break;
      case 0x20: nPage = 0x02; break;
      case 0x30: nPage = 0x03; break;
      case 0x40: nPage = 0x04; break;
      case 0x50: nPage = 0x05; break;
      case 0x60: nPage = 0x06; break;
      case 0x70: nPage = 0x07; break;
      case 0x80: nPage = 0x08; break;
      case 0x90: nPage = 0x09; break;
      case 0xA0: nPage = 0x0A; break; 
      case 0xB0: nPage = 0x0B; break;
      case 0xC0: nPage = 0x0C; break;
      case 0xD0: nPage = 0x0D; break;
      case 0xE0: nPage = 0x0E; break;
      case 0xF0: nPage = 0x00; altSerial.print('?'); break;                                                                             
    }
    // test nPc -> nPage + 1
    switch (nOpcode) {
      case 0x00: // code config
        switch (nParam) {
          case 0x00: break; // nop
          case 0x01: execIncrement(); break; // increment
          case 0x02: execDecrement(); break; // decrement
          case 0x03: altSerial.print('C'); break; // call
          case 0x04: altSerial.print('R'); break; // return
          case 0x05: altSerial.print('Z'); break; // reset
          case 0x06: nStep = OFF; break; // step off
          case 0x07: nStep = ON; break; // step on
          case 0x08: nDirect = ON; nMemoire = OFF; nRegistre = OFF; break; // mode direct
          case 0x09: nMemoire = ON; nDirect = ON; nRegistre = OFF; break; // mode memory
          case 0x0A: nRegistre = ON; nDirect = OFF; nMemoire = OFF; break; // mode register
          case 0x0B: altSerial.print('?'); break;
          case 0x0C: altSerial.print('?'); break;
          case 0x0D: altSerial.print('?'); break;
          case 0x0E: altSerial.print('?'); break;
          case 0x0F: nLoop = 0; // end program
          default: break;
        }
        break;
      case 0x01: break; // addition
      case 0x02: break; // substraction
      default: break;
    }
    if (nStep == ON) displayStep();
  }
}

void execIncrement(void)
{
  if (nRegistre == ON) {
    nAcc++; nMemory[CASEREGACC] = nAcc;
  }
}

void execDecrement(void)
{
  if (nRegistre == ON) {
    nAcc--; nMemory[CASEREGACC] = nAcc;
  }
}

void help(void)
{
  altSerial.println("");
  altSerial.println("Teensy Micro4 V0.7");
  altSerial.println("");  
  altSerial.println("<h00> -> help");
  altSerial.println("<cxx> -> echo off or on"); 
  altSerial.println("<rxx> -> read input");
  altSerial.println("<txx> -> test board");
  altSerial.println("<v00> -> version firmware");
  altSerial.println("<wxx> -> write output");
  altSerial.println("<lxx> -> led board toggle"); 
  altSerial.println("");  altSerial.print('>');
}

void toggleIn(int nPinNum)
{
  digitalWrite(nPinNum, nPinStateIn);
  nPinStateIn = !nPinStateIn;
}

void toggleOut(int nPinNum)
{
  digitalWrite(nPinNum, nPinStateOut);
  nPinStateOut = !nPinStateOut;
}

void clearMemory(void)
{
  int i = 0;
  for (i = 0; i < MAXMEM; i++) {
    nMemory[i] = 0 & 0x0F;
  }
}

void displayStep(void)
{
  int j = 0, l = 0;
  altSerial.println("X Y A F I O 0 1 2 3 4 5 6 7 8 9 M N P");  
  for (j = 0; j < 0x10; j++) {
    l = nMemory[0xF0 + j];
    if (l < 0x0A) altSerial.print(l);
    else {
      switch (l) {
        case 10: altSerial.print('A'); break;
        case 11: altSerial.print('B'); break;
        case 12: altSerial.print('C'); break;
        case 13: altSerial.print('D'); break;
        case 14: altSerial.print('E'); break;
        case 15: altSerial.print('F'); break;
        default: break;
      }         
    }
    altSerial.print(' ');
  }
  if (nDirect == ON) altSerial.print('d');
  if (nMemoire == ON) altSerial.print('m');
  if (nRegistre == ON) altSerial.print('r');
  altSerial.print(' ');
  if (nPage < 0x0A) altSerial.print(nPage);
  else {
    switch (nPage) {
        case 10: altSerial.print('A'); break;
        case 11: altSerial.print('B'); break;
        case 12: altSerial.print('C'); break;
        case 13: altSerial.print('D'); break;
        case 14: altSerial.print('E'); break;
        case 15: altSerial.print('F'); break;
        default: break;
    }
  }  
  altSerial.print(" ");
  if (nPc < 0x0A) altSerial.print(nPc);
  else {
    switch (nPc) {
        case 10: altSerial.print('A'); break;
        case 11: altSerial.print('B'); break;
        case 12: altSerial.print('C'); break;
        case 13: altSerial.print('D'); break;
        case 14: altSerial.print('E'); break;
        case 15: altSerial.print('F'); break;
        default: break;
    }
  }
  altSerial.println("");
}

void displayRegister(void)
{
  altSerial.print("Mode: ");  
  if (nDirect == ON) altSerial.print('d');
  if (nMemoire == ON) altSerial.print('m');
  if (nRegistre == ON) altSerial.print('r');
  altSerial.print(" Page: "); 
  if (nPage < 0x0A) altSerial.print(nPage);
  else {
    switch (nPage) {
        case 10: altSerial.print('A'); break;
        case 11: altSerial.print('B'); break;
        case 12: altSerial.print('C'); break;
        case 13: altSerial.print('D'); break;
        case 14: altSerial.print('E'); break;
        case 15: altSerial.print('F'); break;
        default: break;
    }
  }
  altSerial.print(" Pc: "); 
  if (nPc < 0x0A) altSerial.print(nPc);
  else {
    switch (nPc) {
        case 10: altSerial.print('A'); break;
        case 11: altSerial.print('B'); break;
        case 12: altSerial.print('C'); break;
        case 13: altSerial.print('D'); break;
        case 14: altSerial.print('E'); break;
        case 15: altSerial.print('F'); break;
        default: break;
    }
  }  
  altSerial.println("");  
}

void displayMemory(void)
{
  int i = 0, j = 0, k = 0, l = 0;
  altSerial.println(""); altSerial.println("   0 1 2 3 4 5 6 7 8 9 A B C D E F");
  for (i = 0; i < 0x10; i++) {
    altSerial.print('#'); 
    if (i < 0x0A) altSerial.print(i);
    else {
      switch (i) {
        case 10: altSerial.print('A'); break;
        case 11: altSerial.print('B'); break;
        case 12: altSerial.print('C'); break;
        case 13: altSerial.print('D'); break;
        case 14: altSerial.print('E'); break;
        case 15: altSerial.print('F'); break;
        default: break;
      }
    }
    altSerial.print(' ');
    k = i * 0x10;
    for (j = 0; j < 0x10; j++) {
      l = nMemory[k + j];
      if (l < 0x0A) altSerial.print(l);
      else {
        switch (l) {
          case 10: altSerial.print('A'); break;
          case 11: altSerial.print('B'); break;
          case 12: altSerial.print('C'); break;
          case 13: altSerial.print('D'); break;
          case 14: altSerial.print('E'); break;
          case 15: altSerial.print('F'); break;
          default: break;
        }         
      }
      altSerial.print(' ');
    }
    altSerial.println("");
  }
  altSerial.println("   X Y A F O I 0 1 2 3 4 5 6 7 8 9");
}

void readpins(void)
{ // Read pins (analog or digital)
  //messageSendInt(analogRead(nPinAnalog));
  //messageSendInt(digitalRead(nPinButton));
}

void writepin(void) 
{ // Write pin
  //digitalWrite(nPinLed, LOW);
  //digitalWrite(nPinLed, HIGH); 
  //pinMode(pin, OUTPUT);
  //analogWrite(pin, state);
}

void teston(void)
{ 
  while(1) {
    if (altSerial.available()) {
      cChar = altSerial.read();
      altSerial.print(cChar);
      toggleIn(ledPinIn);
      toggleOut(ledPinOut);
    }
  }  
}

