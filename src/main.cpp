#include <Arduino.h>
#include "Wire.h"
#include "SPI.h"
#include "Motor.h"

#define USE_COLOR_SENSOR

#include "Sensor.h"


//// ピン番号の設定
// 左モーター
const int LEFT_U1 = 3;
const int LEFT_U2 = 2;
const int LEFT_UA = 4;
const int LEFT_D1 = 5;
const int LEFT_D2 = 6;
const int LEFT_DA = 7;
// 右モーター
const int RIGHT_U1 = 12;
const int RIGHT_U2 = 11;
const int RIGHT_UA = 13;
const int RIGHT_D1 = 9;
const int RIGHT_D2 = 10;
const int RIGHT_DA = 8;

// nani
const int STANDBY = 16;

//// 微調整用の変数
// 通常走行の速度
const int RUN_SP = 30;
// ゆっくり曲がるときの速度
const int HALF_STOP_SP = 15;
// 強く曲がるときの速度
const int STOP_SP = -10;
// sensor
const int COLOR_THRESHOLD1 = 10;
const int COLOR_THRESHOLD2 = 10;
// 90 turn delay
const int TURN_DELAY = 2000;


// クラスを実体化
Motor motor_lu(LEFT_UA, LEFT_U1, LEFT_U2);
Motor motor_ld(LEFT_DA, LEFT_D1, LEFT_D2);
Motor motor_ru(RIGHT_UA, RIGHT_U1, RIGHT_U2);
Motor motor_rd(RIGHT_DA, RIGHT_D1, RIGHT_D2);

Sensor sensor;


void motor_setup()
{
  motor_lu.setup();
  motor_ld.setup();
  motor_ru.setup();
  motor_rd.setup();
}

void left_motor(int speed)
{
  motor_lu.turn(speed);
  motor_ld.turn(speed);
}
void right_motor(int speed)
{
  motor_ru.turn(speed);
  motor_rd.turn(speed);
}

// 直進用関数
// spped -100 ~ 100
void straight(int speed)
{
  left_motor(speed * 255 / 100);
  right_motor(speed * 255 / 100);
}
// 回転用、 第一引数でl側、第二引数でr側を設定できます。
// spped -100 ~ 100
void turn(int l_speed, int r_speed)
{
  left_motor(l_speed * 255 / 100);
  right_motor(r_speed * 255 / 100);
}



void setup() {
  Serial.begin(9600);
  sensor.setup();
  //motor_setup();
  //digitalWrite(STANDBY, HIGH);
}

// 前回どっち側検出したか
// 1で右-1で左
int before = 0;

unsigned long l_color_last_ditect = 0;
unsigned long r_color_last_ditect = 0;
unsigned long last_ditect = 0;


void loop() {
  sensor.set(); // センサーの値を読み取ります。
  //sensor.debug();
  sensor.debug_raw();
  //sensor.debug_color();
  delay(100);
  return;

  // 真ん中が同時についたら直進する
  if (sensor.get(2) && !sensor.get(1) && !sensor.get(3))
  {
    straight(RUN_SP);
    before = 0;
    last_ditect = millis();
  }
  // 真ん中の右だけだったら少しだけ右に
  else if (sensor.get(3))
  {
    turn(RUN_SP, HALF_STOP_SP);
    before = 0;
    last_ditect = millis();
  }
  // 真ん中の左だけだったら少しだけ左に
  else if (sensor.get(1))
  {
    turn(HALF_STOP_SP, RUN_SP);
    before = 0;
    last_ditect = millis();
  }
  // 右端だったら強く右に曲がる
  else if (sensor.get(4))
  {
    turn(RUN_SP, STOP_SP);
    before = 1;
    last_ditect = millis();
  }
  // 左端だったら強く左に曲がる
  else if (sensor.get(0))
  {
    turn(STOP_SP, RUN_SP);
    before = -1;
    last_ditect = millis();
  }
  // 5つが検出されてないとき
  else
  {
    // 前回と同じ動きをする
    if (before > 0)
    {
      turn(RUN_SP, STOP_SP);
    }
    else if (before < 0)
    {
      turn(STOP_SP, RUN_SP);
    }
    else
    {
      // どっちでもないときは直進
      straight(RUN_SP);
    }
  }
  delay(10);
  
  ///// line trace end
}