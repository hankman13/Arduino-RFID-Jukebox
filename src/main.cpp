// Add libraries
#include "Arduino.h"
#include <Bounce2.h>
#include "SoftwareSerial.h"
// https://github.com/electronicdrops/RFIDRdm630
#include <RFIDRdm630.h>
//for Piezo Melody
#include <pitches.h>
//DFPlayer Mini
#include "DFRobotDFPlayerMini.h"

// set Data Pin Piezo buzzer
const int buzzer = 9; //buzzer to arduino pin 9

// set Data Pin RDM6300
const int rxPin = 6;  // pin that will receive the data
const int txPin = 7;  // connection not necessary.
RFIDtag  tag;  // RFIDtag object
RFIDRdm630 reader = RFIDRdm630(rxPin,txPin);    // the reader object.

// set Data Pin DFRobotDFPlayerMini
SoftwareSerial mySoftwareSerial(10, 11); // RX, TX
DFRobotDFPlayerMini myDFPlayer;

//  define Cars Numbers
#define elements  2
long validCardNumber[elements] = {13683409, // Folder 1
                                  13683544  // Folder 2
                  };
long lastCardNumber = 0;

// set Buttons
const int buttonNextPin = 4;
const int buttonPrevPin = 5;
int buttonNextState = LOW;
int buttonPrevState = LOW;
bool isplaying = false;

// Instantiate a Bounce object
Bounce debouncer1 = Bounce();

// Instantiate another Bounce object
Bounce debouncer2 = Bounce();

void setup(){
// Initializing serial console

Serial.begin(115200);
 Serial.println();
 Serial.println(F("RFID Arduino Jukebox"));
 Serial.println(F("Initializing ..."));

// Initializing Piezo Buzzer
pinMode(buzzer, OUTPUT); // Set buzzer - pin 9 as an output
 Serial.println(F("... Piezo Buzzer done."));

// Initializing Buttons


pinMode(buttonNextPin, INPUT_PULLUP);
// After setting up the button, setup the Bounce instance :
debouncer1.attach(buttonNextPin);
debouncer1.interval(3); // interval in ms


pinMode(buttonPrevPin, INPUT_PULLUP);
// After setting up the button, setup the Bounce instance :
debouncer2.attach(buttonPrevPin);
debouncer2.interval(3); // interval in ms
// Instantiate a Bounce object


// Initializing RFID RDM6300



// Initializing DFRobotDFPlayerMini

mySoftwareSerial.begin(9600);

if (!myDFPlayer.begin(mySoftwareSerial)) {  //Use softwareSerial to communicate with mp3.

  noTone(buzzer);
  tone(buzzer, 1519,100);
  delay(300);
  tone(buzzer, 1519,100);
  delay(300);
  tone(buzzer, 1519,100);
  delay(300);
  noTone(buzzer);
   Serial.println(F("Unable to begin:"));
   Serial.println(F("1.Please recheck the connection!"));
   Serial.println(F("2.Please insert the SD card!"));
    while(true){
      delay(0); // Code to compatible with ESP8266 watch dog.
    }
  }
 Serial.println(F("DFPlayer Mini online."));
myDFPlayer.setTimeOut(500);
myDFPlayer.volume(20);
 Serial.print(F("DFPlayer Mini set Volume "));
 Serial.println(myDFPlayer.readVolume());
  delay(300);
myDFPlayer.play(1);
//myDFPlayer.next();

// play startupsound via buzzer
 Serial.println(F("Playing startupsound via Buzzer"));
noTone(buzzer);
tone(buzzer, 1519,100);
delay(300);
tone(buzzer, 1519,100);
delay(300);
noTone(buzzer);
Serial.println(F("Waiting for Interaction."));

}

void loop() {
// is Playing
if (isplaying){
// Update the Bounce instances :
debouncer1.update();
debouncer2.update();

// Get the updated value :
// Button 1
if ( debouncer1.fell() ) {

     // Toggle LED state :
     buttonNextState = HIGH;

   }

if (buttonNextState == HIGH) {
    // play next:
    Serial.println("Button Next");
    tone(buzzer,NOTE_C4,100);
    mySoftwareSerial.listen();

    myDFPlayer.next();
    delay(500);
    buttonNextState = LOW;
  }
// Button 2
  if ( debouncer2.fell() ) {

       // Toggle LED state :
       buttonPrevState = HIGH;

     }

  if (buttonPrevState == HIGH) {
      // play next:
      Serial.println("Button Previous");
      tone(buzzer,NOTE_G4,100);
      mySoftwareSerial.listen();

      myDFPlayer.previous();
      delay(500);
      buttonPrevState = LOW;
    }
}

reader.rfidlisten();
delay(300);

/*
if (mySoftwareSerial.isListening()) {
 Serial.println("DFPlayer is listening!");
}else{
 Serial.println("DFPlayer is not listening!");
}
if (reader.rfidisListening()) {
 Serial.println("RFID is listening!");
}else{
 Serial.println("RFID is not listening!");
}
*/

//RDM6300 get Tag
  if (reader.isAvailable()){  // tests if a card was read by the module

     tag = reader.getTag();  // if true, then receives a tag object


// Compare cardNumber
boolean matching = true;
     int j;
      for (j = 0; j < elements; j++) {
        if(validCardNumber[j] == tag.getCardNumber()){
          matching = true;

          break;
        }else{
          matching = false;
      }
      }
      // Check if lastCardNumber
      if (lastCardNumber == tag.getCardNumber()){
       Serial.println("Card Number is Last Card Number");
      }else{
      // Action after comparing cardNumber
      if(matching) {
       Serial.print("Match. Element: ");
       Serial.print(j);
       Serial.print(" Card Number: ");
       Serial.println(tag.getCardNumber());

        lastCardNumber = tag.getCardNumber();
        // Play Fireball sound
        tone(buzzer,NOTE_G4,35);
        delay(35);
        tone(buzzer,NOTE_G5,35);
        delay(35);
        tone(buzzer,NOTE_G6,35);
        delay(35);
        noTone(buzzer);
        Serial.println("Listen to DFPlayer");
        mySoftwareSerial.listen();
        delay(300);
        int folder=j+1;
        Serial.print("Play Song, Folder ");
        Serial.println(folder);
        myDFPlayer.loopFolder(folder);
        delay(300);
        Serial.println("Playing");
        isplaying=true;
      }else{
       Serial.print("No Match. Card Number: ");
       Serial.println(tag.getCardNumber());
        tone(buzzer, 1519,100);
        reader.rfidlisten();

      }
      //Action done
    }
  }
    //getting Tag done


}
