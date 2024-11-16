#ifndef sensor_h
#define sensor_h

#include <Arduino.h>
#include <Arduino.h>

#define rep(i, n) for (int i = 0; i < (int)(n); i++)

#define PIN_SIZE 5
#define PINS {A2, A3, A4, A5, A6}
#define SENSOR_THRESHOLDS {800,600,250,400,700}

#define PIN_REP_CNT 5

#define L_COLOR_PIN A1
#define R_COLOR_PIN A7
#define RED_LED_PIN 50
#define GRE_LED_PIN 48
#define L_COLOR_THRESHOLDS {250,250,200,150}
#define R_COLOR_THRESHOLDS {170,150,170,97}


class Sensor
{
private:
    // センサーのpin番号
    int pins[PIN_SIZE] = PINS;
    int thresholds[PIN_SIZE] = SENSOR_THRESHOLDS;
    int color_thresholds[2][4] = {L_COLOR_THRESHOLDS,R_COLOR_THRESHOLDS};
public:
    enum LED_STATUS {
        GREEN,
        RED
    };
    LED_STATUS led_status = GREEN;
    int data[PIN_SIZE];
    int color_data[2][2];

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
        pinMode(RED_LED_PIN,OUTPUT);
        pinMode(GRE_LED_PIN,OUTPUT);
        rep(i, PIN_SIZE)pinMode(pins[i], INPUT);
        pinMode(L_COLOR_PIN,INPUT);
        pinMode(R_COLOR_PIN,INPUT);
        color_led(LED_STATUS::RED);
    }
    void set(){
        rep(i, PIN_SIZE) data[i] = 0;
        color_data[led_status][0] =0;
        color_data[led_status][1] =0;
        rep(_,PIN_REP_CNT){
            rep(i, PIN_SIZE) data[i] += analogRead(pins[i]);
            color_data[led_status][0] += analogRead(L_COLOR_PIN);
            color_data[led_status][1] += analogRead(R_COLOR_PIN);
        }
        rep(i, PIN_SIZE) data[i] /= PIN_REP_CNT;
        color_data[led_status][0] /= PIN_REP_CNT;
        color_data[led_status][1] /= PIN_REP_CNT;
    }
    bool get(int i){
        return data[i] > thresholds[i];
    }

    unsigned int get_state(){
        unsigned int state = 0;
        rep(i, PIN_SIZE)state |= get(i) << i;
        return state;   
    }
    
    int get_color_data(int LorR){
        return color_data[LED_STATUS::RED][LorR];
    }
    // 0 -> white
    // 1 -> black
    // 2 -> green
    int detect_color(int LorR){
        /*
        if(color_data[LED_STATUS::GREEN][LorR] > color_thresholds[LorR][0] && color_data[LED_STATUS::RED][LorR] > color_thresholds[LorR][1])return 0;
        if(color_data[LED_STATUS::GREEN][LorR] > color_thresholds[LorR][2] && color_data[LED_STATUS::RED][LorR] > color_thresholds[LorR][3])return 2;
        return 1;
        */
        if(color_data[LED_STATUS::RED][LorR] > color_thresholds[LorR][1])return 0;
        if(color_data[LED_STATUS::RED][LorR] > color_thresholds[LorR][3])return 1;
        return 2;
    }
    // 0 others
    // 1 black black
    // 2 white black
    // 3 black white
    // 4 green white
    // 5 white green
    // 6 green green
    int get_color_state(){
        int now_state = 0;
        int c_1 = detect_color(0);
        int c_2 = detect_color(1);
        if(c_1 == 1 && c_2 == 1)now_state = 1;
        else if(c_1 == 0 && c_2 == 1)now_state = 2;
        else if(c_1 == 1 && c_2 == 0)now_state = 3;
        else if(c_1 == 2 && c_2 == 0)now_state = 4;
        else if(c_1 == 0 && c_2 == 2)now_state = 5;
        else if(c_1 == 2 && c_2 == 2)now_state = 6;
        return now_state;
    }

    void debug_raw(){
        rep(i, PIN_SIZE){
            Serial.print(data[i]);
            Serial.print(" ");
        }
        Serial.println();
    }
    void debug_color_raw(bool LorR){
        /*
        if(LorR){
            Serial.print(color_data[0][0]);
            Serial.print(" ");
            Serial.println(color_data[1][0]);
        }else{
            Serial.print(color_data[0][1]);
            Serial.print(" ");
            Serial.println(color_data[1][1]);
        }*/
        Serial.print(color_data[LED_STATUS::RED][0]);
        Serial.print(" ");
        Serial.println(color_data[LED_STATUS::RED][1]);
    }
    void debug_color(){
        Serial.print(detect_color(0));
        Serial.print(" ");
        Serial.println(detect_color(1));
    }
    void debug_color_state(){
        Serial.print("now color state: ");
        switch (get_color_state())
        {
        case 0:
            Serial.println("others");
            break;
        case 1:
            Serial.println("black black");
            break;
        case 2:
            Serial.println("white black");
            break;
        case 3:
            Serial.println("black white");
            break;
        case 4:
            Serial.println("green white");
            break;
        case 5:
            Serial.println("white green");
            break;
        case 6:
            Serial.println("green green");
            break;
        default:
            break;
        }
    }
};


#endif