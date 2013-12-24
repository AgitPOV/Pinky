#include <EEPROM.h>


// Use lillypad Arduino w/ atmega168

/*  
POV PIN CONFIGURTATION
Pinky mapping : ATMEGA168 : Arduino
LED0-LED5 : PB0-PB5 : 8,9,10,11,12,13
LED6-LED11 : PC0-PC5 : AN0, AN1, AN2, AN3, AN4, AN5
*/
unsigned char povPins[]={A5,A4,A3,A2,A1,A0,13,12,11,10,9,8};

/* 
POV INTERRUPT CONFIGURATION
what : interrupt : ATMEGA168 : Arduino
hall : 0 : PD2 : 2
*/
#define HALL_INTERRUPT 0
#define HALL_PIN 2

/*
MODES
*/
#define PLAYING 0
#define WRITING 1
#define WAITING 2
#define PLAYBACK 3
byte mode = PLAYING;



#define SWITCH_PIN 0
#define SCLK_PIN 3
#define SDAT_PIN 4

byte characterIndex = 0;
byte  bitIndex = 0;
char message[27]; 
byte messageLength;


unsigned long timeStamp;



void setup() {                

  pinMode(SWITCH_PIN,INPUT);
  digitalWrite(SWITCH_PIN,HIGH);
  
  
  for ( int i =0; i < 27; i++ ) message[i] = 0;
  
  povSetup();

  // A 3s fade out animation  
   timeStamp = millis();
   unsigned long microTimeStamp = micros();
  while ( (millis() - timeStamp) < 3000) {
    
      unsigned long lapsed = (micros() - microTimeStamp); // 0-3000000 
      
      unsigned long dim = lapsed / 300; // 0-10000
      unsigned long frame = lapsed % 10000; //0-10000
      
      if ( frame >= dim ) {
        for ( int i=0; i < 12 ; i++ ) digitalWrite( povPins[i],  LOW ); //ON
      } else {
        for ( int i=0; i < 12 ; i++ ) digitalWrite( povPins[i],  HIGH ); //OFF
      }
      
      
    
  }
  
  // Turn LEDS off
   for (unsigned char k=0;k<12;k++) {
    digitalWrite(povPins[k],HIGH);
  }


  // Check switch for mode
  // Go to WAITING to WRITING mode if switch is pressed  
  if ( digitalRead(SWITCH_PIN) == LOW ) mode = WAITING;


  // SETUP MODE


  if ( mode == PLAYING ) {

    readMessage();
   


  }  else { // mode == WRITING OR WAITING

    pinMode(SCLK_PIN,INPUT);
    pinMode(SDAT_PIN,INPUT);

  }
  
  
  timeStamp = millis();

}

// the loop routine runs over and over again forever:
void loop() {

  if ( mode == PLAYING ) {

     povWaitAndDisplay();
    //povDisplay();

  } 
  else if ( mode == WAITING) {
    
      
  
    unsigned long lighted = ((millis() - timeStamp) / 125) % 12;
    for ( int i=0; i < 12 ; i++ ) {
      digitalWrite( povPins[i],  (lighted != i ));
    }
    
    
    if ( digitalRead(SWITCH_PIN) == HIGH &&  digitalRead(SCLK_PIN) == LOW && digitalRead(SDAT_PIN) == LOW ) { // Button released and nothing is detected
        
        for ( int i=0; i < 12 ; i++ ) {
          digitalWrite( povPins[i],  HIGH);
        }
        
        
        //while( digitalRead(SCLK_PIN) == HIGH || digitalRead(SDAT_PIN) == HIGH) ; // Wait till nothing is detected
        timeStamp = millis();
        
        attachInterrupt(1,SCLKInterrupt, FALLING);
        mode = WRITING;
         
    }
    
    
  } else if ( mode == PLAYBACK) {
    
    povDisplay();
    
    
  } else { // mode == WRITING
  
   digitalWrite( povPins[0],  (millis() / 125) % 2 );
      digitalWrite(povPins[1], !digitalRead(SCLK_PIN) );
   
   digitalWrite(povPins[2], !digitalRead(SDAT_PIN) );
    
  }
}



void SCLKInterrupt() {
  
  if ( millis() - timeStamp > 500 ) {
  int sdatState = digitalRead(SDAT_PIN);
  

  
  message[characterIndex] = message[characterIndex] | ( sdatState << bitIndex );
  bitIndex++;
  
  if ( bitIndex == 8 ) {
    bitIndex=0;


    if ( characterIndex == 26 ) {
      message[characterIndex] = 0;     
    }

    if ( message[characterIndex] == 0 )  {
      saveMessage();
    }
    else {
      characterIndex=characterIndex+1; 
    }


  }
  }

}

void saveMessage() {
  detachInterrupt(1);

  messageLength = characterIndex + 1;


  for ( byte i = 0; i < messageLength; i++ ) {
    EEPROM.write(i,message[i]);
    if ( message[i] == 0 ) break;
  }

  mode = PLAYBACK;

  povSetMessage(&message[0]);

}

void readMessage() {
  boolean ok = true;

  for ( byte i = 0; i < 26; i++ ) {
    byte b = EEPROM.read(i);
    if ( b > 127 ) {
      ok = false;
      break;
    }
    message[i] = b;
    if ( b == 0 ) {
      if ( i == 0 ) ok = false;
      break;
    }
  }
  message[26] = 0; // failsafe
  
  if ( ok ) {
     povSetMessage(&message[0]);
  } else {
     povSetMessage("PINKY");
  }
   
}

