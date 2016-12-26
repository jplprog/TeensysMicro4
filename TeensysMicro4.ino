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
int nBaud = 9600;
int nCount = 0;
int nEcho = 0;
int nLoop = 0;
int nState = 0;
int nDataMsb = 0;
int nDataLsb = 0;
int nLedState = 0;

// micro variables
#define MAXMEM 0x10 * 0x10
int nMemory[MAXMEM];
int nPage = 0;
int nPc = 0;

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
void displayRegister(void);
void displayMemory(void);

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
    switch (cChar) {
     case ' ':
      messageClear();
      break;
     case 'z':
     case '<':
      if (nState == OFF) nState = STATEBEGIN;
      if (nEcho == ON) altSerial.print(cChar);
      break; 
     case 'x':
     case '>':
      if (nState == STATEDATALSB) nState = STATEEND;
      if (nEcho == ON) altSerial.print(cChar);    
      break;
     case 10: // lf
      break;
     case 13: // cr
      //altSerial.println(""); altSerial.print('>'); 
      break;
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
      if (nEcho == ON) altSerial.print(cChar);
      break;
    }
  }
  return nState;
}

void loop(void)
{
  int nTest = OFF;
  if (nTest == ON) {
    altSerial.printf("> ");    
    while(1) {
      if (altSerial.available()) {
        cChar = altSerial.read();
        altSerial.print(cChar);
        toggleIn(ledPinIn);
        toggleOut(ledPinOut);
      }
    }
  }
  if (messageBuild() == 5) {
    switch (cCommand) { // Gets the first word as a character
      case 'h': // Help
      case '3':
        help( ); messageClear();
        break;
      case 'd': // display memory
      case '0':
        displayMemory(); displayRegister(); messageClear();
        break;
      case 'e': // Write echo off or on
      case '2': 
        if (nDataLsb == 0) nEcho = OFF;
        else {
          if (nDataLsb == 1) nEcho = ON;
        } 
        messageClear(); altSerial.print('!'); 
        break; 
      case 'p': // page and pc
      case '4':
        nPage = nDataMsb; nPc = nDataLsb; messageClear(); altSerial.print('!');
        break;      
      case 'r': // Read pins (analog or digital)
      case '5':
        readpins( ); messageClear(); // Call the readpins function
        break; // Break from the switch
      case 't': // Test card
      case '6':
        //if (nEcho == 1) messageSendChar('T'); // Echo what is being read
        //while(1) test( );
        messageClear(); altSerial.print('!');
        break; 
      case 'v': // Version
      case '7':
        altSerial.println("1.0"); messageClear(); altSerial.print('!');
        break;      
      case 'w': // Write pin
      case '8':
        //if (nEcho == 1) messageSendChar('W'); // Echo what is being read
        writepin( ); messageClear(); altSerial.print('!'); // Call the writepin function
        break;
      case 'l': // Toogle led
      case '9':
        if (nLedState == OFF) {
          digitalWrite(nPinLed, HIGH); nLedState = ON;
        } 
        else {
          digitalWrite(nPinLed, LOW); nLedState = OFF;
        }  
        messageClear(); altSerial.print('!');    
        break;
      default: break;  
    }
  }
}

void readpins(void)
{ // Read pins (analog or digital)
  char cChr = 0;
  switch (cChr) { // Gets the next word as a character
    case 'a': // READ analog pin
    case '1':
      //messageSendInt(analogRead(nPinAnalog)); // Read pin 14
      //;if (nEcho == 1) messageEnd( ); // Terminate the message being sent
      break;  
    case 'b': // READ button
    case '2':  
      //messageSendInt(digitalRead(nPinButton));
      //if (nEcho == 1) messageEnd( ); // Terminate the message being sent
      break;     
    case 'd': // READ digital pins
    case '3':
      //for (char i = 0; i < 13; i++) {
        //messageSendInt(digitalRead(i)); // Read pins 0 to 13
      //}
      //if (nEcho == 1) messageEnd( ); // Terminate the message being sent
      break;
    case 'l': // READ light
    case '5':
      //messageSendInt(analogRead(nPinLight));         
      //if (nEcho == 1) messageEnd( ); // Terminate the message being sent 
      break;  
    case 's': // READ switch
    case '7':   
      //messageSendInt(digitalRead(nPinSwitch));
      //if (nEcho == 1) messageEnd( ); // Terminate the message being sent
      break;      
    case 't': // READ temperature
    case '8':
      //messageSendInt(analogRead(nPinTemperature));          
      //if (nEcho == 1) messageEnd( ); // Terminate the message being sent
      break;       
    default: break;
  }
}

void writepin(void) 
{ // Write pin
  char cChr = 0;
  int pin = 0;
  int state = 0;
  switch (cChr) { // Gets the next word as a character
    case 'c': // WRITE count
    case '0':
      nCount++;
      if (nCount > 9) nCount = 1;  
      digitalWrite(nPinLed, LOW);
      delay(500);
      digitalWrite(nPinLed, HIGH);
      delay(500);  
      digitalWrite(nPinLed, LOW);     
      break;
    case 'a': // WRITE an analog pin
    case '1':
      //if (nEcho == 1) messageSendChar('A'); // Echo what is being read    
      //pin = messageGetInt( ); // Gets the next word as an integer
      //state = messageGetInt( ); // Gets the next word as an integer
      pinMode(pin, OUTPUT); // Sets the state of the pin to an output
      analogWrite(pin, state); // Sets the PWM of the pin 
      //if (nEcho == 1) messageSendInt(state);
      break; // Break from the switch 
    case 'b': // WRITE a buzzer pin
    case '2':
      //if (nEcho == 1) messageSendChar('B'); // Echo what is being read    
      //state = messageGetInt( ); // Gets the next word as an integer
      pinMode(nPinBuzzerP, OUTPUT); // Sets the state of the pin to an output
      digitalWrite(nPinBuzzerP, !state); // Sets the state of the pin HIGH (1) or LOW (0)
      //if (nEcho == 1) messageSendInt(state);
      break;     
    case 'd': // WRITE a digital pin
    case '3':
      //if (nEcho == 1) messageSendChar('D'); // Echo what is being read    
      //pin = messageGetInt( ); // Gets the next word as an integer
      //state = messageGetInt( ); // Gets the next word as an integer
      pinMode(pin, OUTPUT); // Sets the state of the pin to an output
      digitalWrite(pin, state); // Sets the state of the pin HIGH (1) or LOW (0)
      //if (nEcho == 1) messageSendInt(state);
      break;
    case 'g': // WRITE a led green pin
    case '4':
      //if (nEcho == 1) messageSendChar('G'); // Echo what is being read    
      //state = messageGetInt( ); // Gets the next word as an integer
      pinMode(nPinGreen, OUTPUT); // Sets the state of the pin to an output
      digitalWrite(nPinGreen, !state); // Sets the state of the pin HIGH (1) or LOW (0)
      //if (nEcho == 1) messageSendInt(state);
      break;      
    case 'l': // WRITE a led pin
    case '5':
      //if (nEcho == 1) messageSendChar('L'); // Echo what is being read    
      //state = messageGetInt( ); // Gets the next word as an integer
      pinMode(nPinLed, OUTPUT); // Sets the state of the pin to an output
      digitalWrite(nPinLed, state); // Sets the state of the pin HIGH (1) or LOW (0)
      //if (nEcho == 1) messageSendInt(state);    
      break;
    case 'm': // WRITE a motor pin
    case '6':
      //if (nEcho == 1) messageSendChar('M'); // Echo what is being read    
      //state = messageGetInt( ); // Gets the next word as an integer
      pinMode(nPinMotor, OUTPUT); // Sets the state of the pin to an output
      digitalWrite(nPinMotor, state); // Sets the state of the pin HIGH (1) or LOW (0)
      //if (nEcho == 1) messageSendInt(state);
      break; 
    case 'r': // WRITE a led red pin
    case '7':
      //if (nEcho == 1) messageSendChar('R'); // Echo what is being read    
      //state = messageGetInt( ); // Gets the next word as an integer
      pinMode(nPinRed, OUTPUT); // Sets the state of the pin to an output
      digitalWrite(nPinRed, !state); // Sets the state of the pin HIGH (1) or LOW (0)
      //if (nEcho == 1) messageSendInt(state);
      break;    
    case 'u': // WRITE a led blue pin
    case '8':
      //if (nEcho == 1) messageSendChar('U'); // Echo what is being read    
      //state = messageGetInt( ); // Gets the next word as an integer
      pinMode(nPinBlue, OUTPUT); // Sets the state of the pin to an output
      digitalWrite(nPinBlue, !state); // Sets the state of the pin HIGH (1) or LOW (0)
      //if (nEcho == 1) messageSendInt(state);
      break;      
    case 'o': // WRITE a output pin
    case '9':
      //if (nEcho == 1) messageSendChar('O'); // Echo what is being read    
      //pin = messageGetInt( ); // Gets the next word as an integer
      //state = messageGetInt( ); // Gets the next word as an integer
      pin = 0;
      switch (pin) {
        case 1:
          pinMode(nPinOutput1, OUTPUT); // Sets the state of the pin to an output
          digitalWrite(nPinOutput1, state); // Sets the state of the pin HIGH (1) or LOW (0)        
          break;
        case 2:
          pinMode(nPinOutput2, OUTPUT); // Sets the state of the pin to an output
          digitalWrite(nPinOutput2, state); // Sets the state of the pin HIGH (1) or LOW (0)        
          break;          
        case 3:
          pinMode(nPinOutput3, OUTPUT); // Sets the state of the pin to an output
          digitalWrite(nPinOutput3, state); // Sets the state of the pin HIGH (1) or LOW (0)        
          break;          
        case 4:
          pinMode(nPinOutput4, OUTPUT); // Sets the state of the pin to an output
          digitalWrite(nPinOutput4, state); // Sets the state of the pin HIGH (1) or LOW (0)        
          break;          
        case 5:
          pinMode(nPinOutput5, OUTPUT); // Sets the state of the pin to an output
          digitalWrite(nPinOutput5, state); // Sets the state of the pin HIGH (1) or LOW (0)        
          break; 
        case 6:
          pinMode(nPinLed, OUTPUT);
          if (nLedState == OFF) {
            digitalWrite(nPinLed, HIGH); nLedState = ON;
          } 
          else {
            digitalWrite(nPinLed, LOW); nLedState = OFF;
          }       
        default:
          break;
      }    
      break;            
    default: break;  
  }
}

void help(void)
{
  altSerial.println("");
  altSerial.println("Teensy Micro4");
  altSerial.println("");  
  altSerial.println("h . -> help (3 .)");
  altSerial.println("e [value] . -> echo off or on (2 v .)"); 
  altSerial.println("p . -> prompt (4 .)");
  altSerial.println("r a . -> read analog pin 14 (5 1 .)");
  altSerial.println("r b . -> read button pin 11 (5 2 .)");
  altSerial.println("r d . -> read digital pins (5 3 .)");
  //Serial.println("r l . -> read light sensor pin 16 (5 5 .)");
  altSerial.println("r s . -> read switch pin 12 (5 7 .)");
  //Serial.println("r t . -> read temperature sensor pin 15 (5 8 .)");
  //Serial.println("t . -> test board (6 .)");
  altSerial.println("v . -> version firmware (7 .)");
  altSerial.println("w c . -> write count 1 to 9 (8 0 .)");
  //Serial.println("w a [pin] [value] . -> write analog pin 14 (8 1 v .)");  
  altSerial.println("w b [value] . -> write buzzer pin 6 (8 2 v .)");
  altSerial.println("w d [pin] [value] . -> write digital pin (8 3 p v .)");
  altSerial.println("w g [value] . -> write led green pin 10 (8 4 v .)");
  altSerial.println("w l [value] . -> write led board pin 13 (8 5 v .)");
  //Serial.println("w m [value] . -> write motor pin 7 (8 6 v .)");
  altSerial.println("w r [value] . -> write led red pin 8 (8 7 v .)");
  altSerial.println("w u [value] . -> write led blue pin 9 (8 8 v .)");
  altSerial.println("w o [out] [value] . -> write output pin 19-23 (8 9 o v .)");
  altSerial.println("l . -> led board toggle pin 13 (9 .)"); 
  altSerial.println("");  
  altSerial.println("p or pin = 1-15, o or out = 1-6, v or value = 0 or 1");
  //altSerial.print("baud rate = "); messageSendInt(nBaud);
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
    nMemory[i] = i & 0x0F;
  }
}

void displayRegister(void)
{
  altSerial.print("Page: "); 
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
}

