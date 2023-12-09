#define BLYNK_TEMPLATE_ID 
#define BLYNK_DEVICE_NAME 
#define BLYNK_AUTH_TOKEN 
 
#include <blynk.h>      //include the blynk library
 
char auth[] = BLYNK_AUTH_TOKEN;     //some more blynk stuff and the wifi information 
 
BlynkTimer timer;     //Blynk also uses a timer
 
#include "Particle.h"
#include "neopixel.h"
 
#define DELAY 250
 
int inputReading;
int SFH_PIN = A1;
 
int ButtonPIN = D2;     //button circuit input
 
int PIXEL_PIN = D4;     // outputs to neopixels
int PIXEL_COUNT = 3;
int PIXEL_TYPE = WS2812;
Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXEL_COUNT, PIXEL_PIN, PIXEL_TYPE);
 
bool ButtonNow = FALSE;    //button initializations
bool ButtonLast = FALSE;
bool ButtonPressed = FALSE;
 
int clearColor = strip.Color(0, 0, 0);      //color that sets LEDS off
int store1 = clearColor;      //LED color for the second LED
int store2 = clearColor;      //LED color for the third LED

int store1val, store2val, x = 0;      //the values sent to Blynk from the sensor 

unsigned long int timeToPress;      //timer for button press
 
 void myTimerEvent()      //needs to be included for Blynk to work
 {
 
 }

 BLYNK_WRITE(V5)      //Function for when the "CLEAR" button is pressed on the Blynk app
 {
  int pinValue = param.asInt();
  if(pinValue == 1) {     //When the button is pressed it returns a value of 1 so this function executes
    store1 = clearColor;      //set the LED color to clear (off)
    store2 = clearColor;

    strip.setPixelColor(0, clearColor);     //send "off" color to LEDS 
    strip.setPixelColor(1, store1);
    strip.setPixelColor(2, store2);
    strip.show();     //turn the LEDS off

    x = 0;      //Set the sensor values to 0
    store1val = 0;   
    store2val = 0;

    Blynk.virtualWrite(V2, x);      //Tell the app that the new values are 0
    Blynk.virtualWrite(V3, store1val);
    Blynk.virtualWrite(V4, store2val);
  }
 }

void setup() {
  Serial.begin(115200);     //debugging and startup functions for Blynk
  delay(5000);
  Blynk.begin(auth);      
  timer.setInterval(1000L, myTimerEvent);     
 
  pinMode (ButtonPIN, INPUT_PULLDOWN);    //uses pulldown resistor for button input
  pinMode (SFH_PIN, INPUT);
  Serial.begin(9600);
  strip.begin();

  timeToPress = millis() + 1000;      //initialize the timer for button press

}
 
void loop() {
 
  Blynk.run();      //start Blynk
  timer.run();
 
  inputReading = analogRead(SFH_PIN);     //read value from light sensor
  unsigned long int currentTime = millis();     //get the current time
 
  ButtonNow = digitalRead(ButtonPIN);    
  int PixelColorGreen = strip.Color(126, 0, 0);     //various neopixel states corresponding to brightness of observed light by LDR
  int PixelColorRed = strip.Color(0, 126, 0);
  int PixelColorBlue = strip.Color(0, 0, 126);
  int PixelColorWhite = strip.Color(65, 65, 65);
  int SFH_val = analogRead(SFH_PIN);              //LDR inputs a DAC reading to tell neopixels which color to output
 
  if (ButtonNow == HIGH && ButtonLast == LOW ){     //if the button is pressed, then the button is pressed
      ButtonPressed = TRUE;
  }

    if(ButtonPressed) {     //if the button is pressed...
      if(currentTime > timeToPress) {     //if the time is right
        SFH_val = analogRead(SFH_PIN);                  //LDR input is read from LDR input
        Blynk.virtualWrite(V2, SFH_val);
    
        if (SFH_val < 1024){                           //if a high brightness is observed, output white lights
        strip.setPixelColor(0, PixelColorWhite);
        x = PixelColorWhite;
        } else if (SFH_val >= 1024 && SFH_val < 2048){         //if a medium brightness is observed by LDR, output blue lights
        strip.setPixelColor(0, PixelColorBlue);
        x = PixelColorBlue;
        } else if (SFH_val >= 2048 && SFH_val < 3072){         //if a low brightness is observed by LDR, output green lights
        strip.setPixelColor(0, PixelColorGreen);
        x = PixelColorGreen;
        } else if (SFH_val >= 3072 && SFH_val < 4096){         // if darkness is observed by LDR, output red lights
        strip.setPixelColor(0, PixelColorRed);
        x = PixelColorRed;
        }
        strip.setPixelColor(1, store1);     //set the lights of the other two LEDS
        strip.setPixelColor(2, store2);
    
        strip.show();      //display the LED colors
    
        Blynk.virtualWrite(V3, store1val);     //send the values of the LEDs to Blynk
        Blynk.virtualWrite(V4, store2val);
    
        store2 = store1;     //transfer one down every time a new value is read for the lights
        store1 = x;
    
        store2val = store1val;     //transfer the Blynk-displayed values
        store1val = SFH_val;
    
        ButtonLast = HIGH;      //"the button was just pressed"
        ButtonPressed = FALSE;      //it's not pressed anymore

        timeToPress += 500;

      }

    }
   else {
      ButtonLast = LOW;
  }

}

 
