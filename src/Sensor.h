#ifndef Sensor_h
#define Sensor_h

#include <Arduino.h>
#include<Adafruit_TCS34725.h>

#define rep(i, n) for (int i = 0; i < (int)(n); i++)
// センサー
#define PIN_SIZE 5
#define PINS {A3, A4, A5, A6, A7}
#define PIN_REP_CNT 5
#define SENSOR_THRESHOLDS {185,170,175,175,185}
#define COLOR_SENSOR_THRESHOLD_WHITE 50
#define COLOR_SENSOR_THRESHOLDS {55,55}
#define COLOR_SENSOR_HIST_SIZE 7
#define COLOR_SENSOR_HIST_THRESHOLD 7

class Sensor
{
private:
    // センサーのpin番号
    int pins[PIN_SIZE] = PINS;
    int thresholds[PIN_SIZE] = SENSOR_THRESHOLDS;
    int color_thresholds[2] = COLOR_SENSOR_THRESHOLDS;

    int color_raw_hist_idx = 0;
    int color_raw_hist[2][2][COLOR_SENSOR_HIST_SIZE];

    void new_color_raw_hist(int i,int j,int k,int l){
        color_raw_hist[0][0][color_raw_hist_idx] = i;
        color_raw_hist[0][1][color_raw_hist_idx] = j;
        color_raw_hist[1][0][color_raw_hist_idx] = k;
        color_raw_hist[1][1][color_raw_hist_idx] = l;
        color_raw_hist_idx++;
        color_raw_hist_idx %= COLOR_SENSOR_HIST_SIZE;
    }

    // pinの判断基準
public:
    // 読み込んだデータの一時保存場所
    int data[PIN_SIZE];
#ifdef USE_COLOR_SENSOR
    Adafruit_TCS34725 c_sensor_1 = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_2_4MS, TCS34725_GAIN_16X);
    Adafruit_TCS34725 c_sensor_2 = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_2_4MS, TCS34725_GAIN_16X);
    uint16_t sensor1_color[4];
    uint16_t sensor2_color[4];
#endif
    // setup
    void setup(){
        rep(i, PIN_SIZE){
            pinMode(pins[i], INPUT);
        }
#ifdef USE_COLOR_SENSOR
        while(!c_sensor_1.begin(0x29, &Wire)) {
            Serial.println("sensor1 No TCS34725 found ... check your connections");
            delay(1000);
        }
        while(!c_sensor_2.begin(0x6D, &Wire)) {
            Serial.println("sensor2 No TCS34725 found ... check your connections");
            delay(1000);
        }
#endif
    };
// 値を読んで一時保存場所にセット
void set()
{   
    rep(i, PIN_SIZE) data[i] = 0;
    rep(_,PIN_REP_CNT)rep(i, PIN_SIZE) data[i] += analogRead(pins[i]);
    rep(i, PIN_SIZE) data[i] /= PIN_REP_CNT;
#ifdef USE_COLOR_SENSOR
    c_sensor_1.getRawData(&sensor1_color[0], &sensor1_color[1], &sensor1_color[2], &sensor1_color[3]);
    c_sensor_2.getRawData(&sensor2_color[0], &sensor2_color[1], &sensor2_color[2], &sensor2_color[3]);
    new_color_raw_hist(sensor1_color[0],sensor1_color[1],sensor2_color[0],sensor2_color[1]);
#endif
}
// 値の取得(基準を超えてるかチェック)
bool get(int i)
{
    return data[i] > thresholds[i];
};

unsigned int get_state(){
    unsigned int state = 0;
    rep(i, PIN_SIZE){
        state |= get(i) << i;
    }
    return state;   
}
    int detect_color(int s){
        int sumr = 0, sumg = 0;
        rep(i, COLOR_SENSOR_HIST_SIZE)sumr += color_raw_hist[s][0][i];
        rep(i, COLOR_SENSOR_HIST_SIZE)sumg += color_raw_hist[s][1][i];
        if(true){
            Serial.print("sumr: ");
            Serial.print(sumr);
            Serial.print(" sumg: ");
            Serial.println(sumg);
        }
        if(sumr + sumg > 1000)return 0;// white
        if(sumg - 20 > sumr)return 1;// green
        return 2; // black
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
        if(c_1 == 2 && c_2 == 2)now_state = 1;
        else if(c_1 == 0 && c_2 == 2)now_state = 2;
        else if(c_1 == 2 && c_2 == 0)now_state = 3;
        else if(c_1 == 1 && c_2 == 0)now_state = 4;
        else if(c_1 == 0 && c_2 == 1)now_state = 5;
        else if(c_1 == 1 && c_2 == 1)now_state = 6;
        return now_state;
    }
// Serialで値（上位二けたを表示)
void debug()
{
    rep(i, PIN_SIZE)
    {
        Serial.print(data[i] > thresholds[i]);
        Serial.write(" ");
    }
    Serial.println();
};
void debug_raw(){
    rep(i, PIN_SIZE)
    {
        Serial.print(data[i]);
        Serial.write(" ");
    }
    Serial.println();
}
void debug_color(){
    Serial.print("now color state: ");
    Serial.println(get_color_state());
}
void debug_color_raw(int mode=0){
#ifdef USE_COLOR_SENSOR
    if(mode == 0 || mode == 1){
        Serial.print("sensor1: ");
        Serial.print(sensor1_color[0]);
        Serial.print(" ");
        Serial.print(sensor1_color[1]);
        Serial.print(" ");
        Serial.print(sensor1_color[2]);
        Serial.print(" ");
        Serial.println(sensor1_color[3]);
    }
    if(mode == 0 || mode == 2){
        Serial.print("sensor2: ");
        Serial.print(sensor2_color[0]);
        Serial.print(" ");
        Serial.print(sensor2_color[1]);
        Serial.print(" ");
        Serial.print(sensor2_color[2]);
        Serial.print(" ");
        Serial.println(sensor2_color[3]);
    }
#endif
}
}
;
#endif