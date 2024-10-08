#include<Arduino.h>
#include<SPI.h>
#include<Adafruit_TCS34725.h>

// 0x6D

Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_16X);
Adafruit_TCS34725 tcs2 = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_16X);
void setup(){
  Serial.begin(9600);
  tcs.begin(0x29, &Wire);
  tcs2.begin(0x6D, &Wire);
}



void loop(void)
{
  uint16_t r, g, b, c;
  
  tcs.getRawData(&r, &g, &b, &c);

  Serial.print("tcs1:");
  Serial.print("R: "); Serial.print(r, DEC); Serial.print(" ");
  Serial.print("G: "); Serial.print(g, DEC); Serial.print(" ");
  Serial.print("B: "); Serial.print(b, DEC); Serial.print(" ");
  Serial.println(" ");

  tcs2.getRawData(&r, &g, &b, &c);

  Serial.print("tcs2:");
  Serial.print("R: "); Serial.print(r, DEC); Serial.print(" ");
  Serial.print("G: "); Serial.print(g, DEC); Serial.print(" ");
  Serial.print("B: "); Serial.print(b, DEC); Serial.print(" ");
  Serial.println(" ");

  delay(1000);
}