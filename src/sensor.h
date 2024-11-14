#ifndef sensor_h
#define sensor_h

#include <Arduino.h>
#include <Arduino.h>

#define rep(i, n) for (int i = 0; i < (int)(n); i++)

#define PIN_SIZE 5
#define PINS {A1, A2, A3, A4, A5}
#define SENSOR_THRESHOLDS {200,190,190,185,200}
#define L_COLOR_PIN 0
#define R_COLOR_PIN 6
#define RED_LED_PIN 50
#define GRE_LED_PIN 48


class Sensor
{
private:
    // センサーのpin番号
    int pins[PIN_SIZE] = PINS;
    int thresholds[PIN_SIZE] = SENSOR_THRESHOLDS;
    enum LED_STATUS {
        GREEN,
        RED
    };
    LED_STATUS led_status = GREEN;
public:
    void color_led(LED_STATUS to){
        led_status = to;
        if(to==LED_STATUS::GREEN){
            digitalWrite(GRE_LED_PIN,HIGH);
            digitalWrite(RED_LED_PIN,LOW);
        }else{
            digitalWrite(GRE_LED_PIN,LOW);
            digitalWrite(RED_LED_PIN,HIGH);
        }
    }
    LED_STATUS next_led(){
        if(led_status==LED_STATUS::GREEN) return LED_STATUS::RED;
        else return LED_STATUS::GREEN;
    }
    void setup(){
        rep(i, PIN_SIZE)pinMode(pins[i], INPUT);
        pinMode(L_COLOR_PIN,INPUT);
        pinMode(R_COLOR_PIN,INPUT);
        color_led(LED_STATUS::GREEN);
    }
};


#endif