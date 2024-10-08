#ifndef Sensor_h
#define Sensor_h

#include <Arduino.h>
#include<Adafruit_TCS34725.h>

#define rep(i, n) for (int i = 0; i < (int)(n); i++)
// センサー
#define PIN_SIZE 5
#define PINS {A2, A3, A4, A5, A6}
#define PIN_REP_CNT 5
#define SENSOR_THRESHOLDS {220,220,200,180,180}
const int SENSOR_THRESHOLD = 205;

class Sensor
{
private:
    // センサーのpin番号
    int pins[PIN_SIZE] = PINS;
    int thresholds[PIN_SIZE] = SENSOR_THRESHOLDS;
    // pinの判断基準
public:
    // 読み込んだデータの一時保存場所
    int data[PIN_SIZE];
#ifdef USE_COLOR_SENSOR
    Adafruit_TCS34725 c_sensor_1 = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_24MS, TCS34725_GAIN_16X);
    Adafruit_TCS34725 c_sensor_2 = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_24MS, TCS34725_GAIN_16X);
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
#endif
}
// 値の取得(基準を超えてるかチェック)
bool get(int i)
{
    return data[i] > thresholds[i];
};
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
#ifdef USE_COLOR_SENSOR
    Serial.print("sensor1: ");
    Serial.print(sensor1_color[0]);
    Serial.print(" ");
    Serial.print(sensor1_color[1]);
    Serial.print(" ");
    Serial.println(sensor1_color[2]);
    Serial.print("sensor2: ");
    Serial.print(sensor2_color[0]);
    Serial.print(" ");
    Serial.print(sensor2_color[1]);
    Serial.print(" ");
    Serial.println(sensor2_color[2]);
#endif
}
}
;
#endif