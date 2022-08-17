/* ATtiny85 as an I2C Slave            BroHogan                           1/12/11
 * Example of ATtiny I2C slave receiving and sending data to an Arduino master.
 * Gets data from master, adds 10 to it and sends it back.
 * SETUP:
 * ATtiny Pin 1 = (RESET) N/U                      ATtiny Pin 2 = (D3) N/U
 * ATtiny Pin 3 = (D4) to LED1                     ATtiny Pin 4 = GND
 * ATtiny Pin 5 = I2C SDA on DS1621  & GPIO        ATtiny Pin 6 = (D1) to LED2
 * ATtiny Pin 7 = I2C SCK on DS1621  & GPIO        ATtiny Pin 8 = VCC (2.7-5.5V)
 * NOTE! - It's very important to use pullups on the SDA & SCL lines!
 * Current Rx & Tx buffers set at 32 bytes - see usiTwiSlave.h
 * Credit and thanks to Don Blake for his usiTwiSlave code. 
 * More on TinyWireS usage - see TinyWireS.h
 * 
 * 
 * Dowload libraries from: https://playground.arduino.cc/uploads/Code/TinyWireS/index.zip
 * 
 */



#include "TinyWireS.h"                  // wrapper class for I2C slave routines
#include <Adafruit_NeoPixel.h>

#define I2C_SLAVE_ADDR  0x8 //0x26            // i2c slave address (38)
#define LED1_PIN         4              // ATtiny Pin 3
//#define LED2_PIN         1              // ATtiny Pin 6

#define LED_COUNT 5
#define LED_PIN   1  // ATtiny Pin 6

int brightness;
int val;
int breathe_val = 0;
int BreatheInOut = 0;

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

void setup(){
  pinMode(LED1_PIN,OUTPUT);             // for general DEBUG use
//  pinMode(LED2_PIN,OUTPUT);             // for verification
  Blink(LED1_PIN,2);                    // show it's alive
  Serial.begin(9600);
  TinyWireS.begin(I2C_SLAVE_ADDR);      // init I2C Slave mode

  strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();            // Turn OFF all strip ASAP
  strip.setBrightness(255); // Set BRIGHTNESS to about 1/5 (max = 255)
}


void loop(){

  Serial.println("hello");
  if (TinyWireS.available()){           // got I2C input!
    brightness = TinyWireS.receive();     // get the byte from master
  }

  if(int(brightness) == 0){
    Serial.println("breathe");
    breathe();
    //colorWipe(strip.Color(0, 0, 255));
    //digitalWrite(LED1_PIN, HIGH);

  }

  else{
    //proximity_equaliser();
    //proximity_equaliser_reverse();
    //digitalWrite(LED1_PIN, LOW);
    proximity_brightness_reverse();
    
  }

}


void Blink(byte led, byte times){ // poor man's display
  for (byte i=0; i< times; i++){
    digitalWrite(led,HIGH);
    delay (250);
    digitalWrite(led,LOW);
    delay (175);
  }
}


void colorWipe(uint32_t color) {
  for(int i=0; i<strip.numPixels(); i++) { // For each pixel in strip...
    strip.setPixelColor(i, color);         //  Set pixel's color (in RAM)
    strip.show();                          //  Update strip to match
  }
}

//void breathe(){
//
//  if (BreatheInOut == 0){
//    for(int i=0; i<255; i++){
//      colorWipe(strip.Color(i, 0, 0));
//      delay(1);
//    }
//    //delay(10);
//    BreatheInOut == 1;
//  }
//
//  else{
//    for(int i=255; i>=0; i--){
//      //colorWipe(strip.Color(i, 0, 0));
//      int g = int(i/255*100);
//      int b = int(i/255*100);
//      colorWipe(strip.Color(i, g, b));
//      delay(3);
//    }
//  //delay(30);
//  BreatheInOut == 0;
//  }
//}


void breathe(){

  if (BreatheInOut == 0){
    breathe_val += 5;
    breathe_val = constrain(breathe_val, 0 ,255);
    strip.setBrightness(breathe_val);
    colorWipe(strip.Color(255, 48, 48));
    
    if(breathe_val >= 255){
      BreatheInOut = 1;
      }
    //delay(1);
  }

  else{
    breathe_val -= 2;
    breathe_val = constrain(breathe_val, 0 ,255);
    strip.setBrightness(breathe_val);
    colorWipe(strip.Color(255, 48, 48));
    
    if(breathe_val <= 0){
      BreatheInOut = 0;
      }
    //delay(2);
  }
}


void proximity_equaliser(){
  strip.clear(); // Set all pixel colors to 'off'
  val = brightness;
  //val = weight * brightness + (1 - weight) * val; // exponential filter to smooth
  val = map(val, 0, 255, 0, strip.numPixels());
  val = constrain(val, 0, strip.numPixels());
  Serial.print("N strip ");
  Serial.print(val);
  
  for(int i=0; i<val; i++) { // For each pixel...

    strip.setPixelColor(i, strip.Color(150, 150, 150)); 
  }
  strip.show();   // Send the updated pixel colors to the hardware.
  delay(10); 
}


void proximity_equaliser_reverse(){
  strip.clear(); // Set all pixel colors to 'off'
  val = brightness;
  //val = weight * analogRead(sensorpin) + (1 - weight) * val; // exponential filter to smooth
  val = map(val, 0, 255, 0, strip.numPixels());
  val = constrain(val, 0, strip.numPixels());
  val = strip.numPixels() - val;
  for(int i=0; i<val; i++) { // For each pixel...

    strip.setPixelColor(i, strip.Color(150, 150, 150)); 
  }
  strip.show();   // Send the updated pixel colors to the hardware.
  delay(100); 
}


void scale_brightness(){
    strip.setBrightness(brightness*2);
    colorWipe(strip.Color(  255,   255,  255)); // White
}

void proximity_brightness_reverse(){
  val = 255 - brightness;
  val = constrain(val,0,255);


  for(int i=0; i<strip.numPixels(); i++) { // For each pixel...

    //strip.setPixelColor(i, strip.Color(val_to_colour, val_to_colour, val_to_colour)); 
    //strip.setPixelColor(i, strip.Color(strip.gamma8(val_to_colour), strip.gamma8(val_to_colour), strip.gamma8(val_to_colour))); 
    strip.setPixelColor(i, strip.Color(strip.gamma8(val), strip.gamma8(val), strip.gamma8(val))); 
    
  }

  strip.show();   // Send the updated pixel colors to the hardware.
  
}
