/*-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
PROJECT: POTATO SOUL
OWNERS: Nikita Khutorni, Harald Asmus
DESCRIPTION: 
This is the Arduino code for the Instructables "Potato Speed Contest" for the project "Potato Soul" by Nikita Khutorni and Harald Asmus.
For using this code you will also need the newer library of the Talkie-libraries on the Github page of adafruit (https://github.com/adafruit/Talkie).
The idea is that inside a (sweet) potato is a 3d printed box with electronics. On the outside, connected to the Arduino Nano on the inside of the box through two holes,
there are two PIR sensors acting as eyes of the potato and watching out for infrared motion. Once one of the PIR sensors picks up a signal, the Potato will warn the person 
with words the Inductive Proximity Sensor (ProxSensor) is activated. The ProxSensor is placed upwards, diagonally, along the longer side of the potato and box, because 
they usually take up a lot of place and very conveniently right above where the head of the ProxSensor is, on the outside of the potato there is written an X and "Don't cut 
me here.", which should push the one cutting the potato pyschologically to cut the potato right there, therefore the ProxSensor with its short magnetic field distance will 
sense the incoming metal/knife and the potato will suddenly scream.
-------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

#include "talkie.h"        //automatically reserves the pin 3

#define spkGND 2           // grounding the speaker at pin 2
#define pir1Pow 4          // = Passive Infrared Sensor 1 Power Pin
#define pir1Sig 5          // = Passive Infrared Sensor 1 Signal Pin
#define pir1GND 6          // = Passive Infrared Sensor 1 Grounding Pin
#define pir2Pow 7          // = Passive Infrared Sensor 2 Power Pin
#define pir2Sig 8          // = Passive Infrared Sensor 2 Signal Pin
#define pir2GND 9          // = Passive Infrared Sensor 2 Grounding Pin

#define proxPow 10          // = Inductive Proximity Sensor Power Pin (usually brown cable, while blue is for grounding, which)
#define proxSig A0         // = Inductive Proximity Sensor Signal Pin (usually black cable)

#define xtraGND 11         // we need an extra ground to ground the speaker
#define xtraGND2 10        // we need an extra ground, if we use an Arduino Nano

Talkie voice;

//the hexencoded words we want the speaker to say using the library
const uint8_t spSTOP[] PROGMEM ={0x0E,0xD8,0x5A,0x3D,0x02,0x11,0xF0,0x00,0x40,0x00,0xBA,0x0F,0x5F,0x85,0x4F,0xBD,0x2A,0x91,0xE4,0x16,0xB7,0xBA,0xD5,0x6D,0x6E,0x33,0x3A,0xDB,0xA7,0x94,0xC7,0xAE,0xEF,0x01,0x00,0x04,0xA0,0xBD,0x39,0x06,0xE0,0x01,0x00,0x00};
const uint8_t spNO[] PROGMEM ={0xA1,0xF2,0x0E,0x3E,0xD3,0xA9,0x94,0x3A,0x78,0xB8,0xA8,0xA0,0x93,0x6A,0xEF,0xE0,0xA3,0x9C,0xF1,0xAA,0xA2,0xE7,0x32,0xF3,0x24,0xA7,0xB0,0xDE,0xDB,0xDC,0xDB,0xDC,0xFC,0xE6,0x37,0x5F,0x95,0xF2,0xB9,0x61,0xD4,0x65,0x57,0xBB,0x6E,0x2D,0x8F,0xB3,0x6A,0xD4,0xA5,0x76,0xB5,0x0F,0x3D,0xDD,0x1B,0xAA,0x92,0xC8,0xF7,0x0F,0x00,0x00};
const uint8_t spPLEASE[] PROGMEM ={0x0E,0xC8,0x81,0xD4,0x01,0x59,0x9B,0x8F,0xB0,0x48,0x69,0x57,0x73,0x3B,0xD2,0x24,0x2D,0x3D,0x24,0xCD,0x4A,0xBD,0x33,0x8F,0xF0,0xC7,0x3B,0x6B,0x79,0x4D,0x86,0x9C,0xFE,0x64,0x56,0xA3,0xAA,0x59,0x89,0x3D,0xBE,0xCC,0xA6,0xB7,0xA5,0xCF,0x9D,0x85,0xA5,0xDE,0xE6,0x3E,0xF5,0x29,0xB1,0x98,0xC5,0xE3,0xD2,0x07,0x13,0x10,0x56,0x1D,0xCB,0xF0,0x35,0x87,0x51,0xAE,0x75,0xC0,0x04,0xAC,0x06,0xF8,0x46,0xCD,0x00,0x3F,0x1A,0x19,0xE0,0x7B,0x0B,0x01,0x7C,0xA7,0xF6};
const uint8_t spRETURN[] PROGMEM ={0x00,0x60,0x12,0x15,0xC9,0x27,0x9C,0xF1,0xA8,0xBC,0x93,0xD4,0xD2,0x65,0xAB,0x4A,0x51,0x43,0x25,0x7B,0xDF,0x6A,0x54,0x3E,0x98,0x99,0x76,0x2B,0xD3,0x19,0x19,0x91,0x1A,0x4E,0x00,0x12,0xD0,0x4C,0xB8,0x07,0x14,0xE0,0x9D,0x19,0x02,0x78,0xB5,0x5C,0x91,0x4F,0x2E,0x95,0xEA,0x7A,0x27,0x2B,0x0D,0xA5,0xC9,0x8B,0x3C,0xEF,0x74,0x67,0x23,0x0F,0xDB,0x38,0x55,0x32,0xCF,0x6A,0xD4,0x61,0x3A,0x87,0x8B,0xAF,0xD6,0x84,0x6B,0x66,0x4E,0x9D,0x4A,0x1B,0x0F,0xBB,0x86,0xA4,0x09,0x5D,0x58,0x90,0x66,0xE5,0xCA,0x75,0x2C,0x58,0x9B,0xB4,0x69,0xDF,0xF9,0xEE,0x01};


void setup() {
  
  // OUTPUT = gives power to the component at that pin; INPUT = takes an incoming signal from the component at that pin
  //we want our low-power PIR sensors to be running at all times
  pinMode(pir1Pow, OUTPUT);
  pinMode(pir2Pow, OUTPUT);
  digitalWrite(pir1Pow, HIGH);
  digitalWrite(pir2Pow, HIGH);
  pinMode(pir1Sig, INPUT);
  pinMode(pir2Sig, INPUT);
  
  pinMode(proxPow, OUTPUT);
  //we start the ProxSensor on LOW and only activate it when it might be needed, because it usually takes in a voltage between 6V and 36V, which mean if we constantly run it on HIGH, it will drink our battery like a cup of CocaCola at McDonalds.
  //the PIR sensors will activate the ProxSensor, because there ain't a knife without a hand, except you have ghosts at home or build a robot solely to cut this potoato, at which point I might question your sanity at either possibility.
  digitalWrite(proxPow, LOW);
  //the ProxSensor will return a value of either 0 (metal detected) or 1023 (no metal)
  pinMode(proxSig, INPUT);

  pinMode(xtraGND, OUTPUT);
  digitalWrite(xtraGND, LOW);
  pinMode(xtraGND2, OUTPUT);
  digitalWrite(xtraGND2, LOW);
  
  Serial.begin(9600);
  
}

void loop() {

  if(digitalRead(pir1Sig) == HIGH || digitalRead(pir2Sig) == HIGH){
    digitalRead(pir1Sig) == HIGH ? Serial.println("Left PIR Detected STH") : Serial.println("Right PIR Detected STH");
    voice.say(spSTOP);
//    voice.say(spPLEASE);
//    voice.say(spRETURN);
    delay(1000);
    digitalWrite(proxPow, HIGH);
    if(analogRead(proxSig) == 0){
      Serial.println("Metal detected");
      for(int i = 1; i <= 5; i++){
        voice.say(spNO);
        delay(1000);
      }
    }
  }
  else{
    if(digitalRead(proxPow) == HIGH){digitalWrite(proxPow, LOW);}
  }

}
