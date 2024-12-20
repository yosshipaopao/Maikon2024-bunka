#include <Arduino.h>
#include "Wire.h"
#include "SPI.h"
#include "Servo.h"
#include "Motor.h"

#include "sensor.h"
#include "comn.h"


//// ピン番号の設定
// 左モーター
const int LEFT_U1 = 3;
const int LEFT_U2 = 2;
const int LEFT_UA = 4;
const int LEFT_D1 = 6;
const int LEFT_D2 = 5;
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
const int RUN_SP = 50;
// ゆっくり曲がるときの速度
const int HALF_STOP_SP = 25;
// 強く曲がるときの速度
const int STOP_SP = 0;
// 90 turn delay
const int TURN_DELAY = 2000;

const int COLOR_SENSOR_HIST_SIZE = 10;

// クラスを実体化
Motor motor_lu(LEFT_UA, LEFT_U1, LEFT_U2);
Motor motor_ld(LEFT_DA, LEFT_D1, LEFT_D2);
Motor motor_ru(RIGHT_UA, RIGHT_U1, RIGHT_U2);
Motor motor_rd(RIGHT_DA, RIGHT_D1, RIGHT_D2);

Sensor sensor;

Servo servo_0;
Servo servo_1;

void motor_setup()
{
  motor_lu.setup();
  motor_ld.setup();
  motor_ru.setup();
  motor_rd.setup();
  digitalWrite(STANDBY, HIGH);
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


void Comn::motor(int speedl, int speedr) {
  turn(-speedr, -speedl);
}
void Comn::_turn(int angle) {
  const int magic_number = 10;
  if(angle == 0){
    straight(0);
  }
  else if(angle > 0){
    turn(-RUN_SP, RUN_SP);
    delay(angle * magic_number);
    straight(0);
  }else{
    turn(RUN_SP, -RUN_SP);
    delay(-angle * magic_number);
    straight(0);
  }
}
Comn comn;

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("start");
  pinMode(18,INPUT_PULLUP);
  sensor.setup();
  Serial.println("setup");
  motor_setup();
}

// 前回どっち側検出したか
// 1で右-1で左
int before = 0;

unsigned long l_color_last_ditect = 0;
unsigned long r_color_last_ditect = 0;
unsigned long last_ditect = 0;

int states[10];
int state_index = 0;
void new_state(int state)
{
  states[state_index++] = state;
  state_index %= 10;
}
int sum_state = 0;
int get_sum_state()
{
  sum_state = 0;
  rep(i, 10)sum_state += states[i];
  return sum_state;
}


int color_old_states[2][COLOR_SENSOR_HIST_SIZE];
int color_states[2][COLOR_SENSOR_HIST_SIZE];
int color_states_idx =0;
int color_thrs[2] = {300,350};
bool get_color_detect(int lr){ 
  color_old_states[lr][color_states_idx] = color_states[lr][color_states_idx];
  color_states[lr][color_states_idx] = sensor.get_color_data(lr);
  color_states_idx++;
  color_states_idx %= COLOR_SENSOR_HIST_SIZE;
  int avg_old_states = 0;
  rep(i,COLOR_SENSOR_HIST_SIZE)avg_old_states += color_old_states[lr][i];
  avg_old_states /= COLOR_SENSOR_HIST_SIZE;
  int avg_new_states = 0;
  rep(i,COLOR_SENSOR_HIST_SIZE)avg_new_states += color_states[lr][i];
  avg_new_states /= COLOR_SENSOR_HIST_SIZE;
  Serial.print(lr);
  Serial.print(" ");
  Serial.println(abs(avg_old_states - avg_new_states));
  if(avg_new_states < color_thrs[lr] && abs(avg_old_states - avg_new_states) < 30 && 15 < abs(avg_old_states - avg_new_states)){
    if(avg_new_states < avg_old_states){
      return true;
    }
  }
  return false;
}
bool get_color_new_detect(){
  int avg_new_states[2] = {0,0};
  rep(j,2)rep(i,COLOR_SENSOR_HIST_SIZE)avg_new_states[j] += color_states[j][i];
  avg_new_states[0] /= COLOR_SENSOR_HIST_SIZE;
  avg_new_states[1] /= COLOR_SENSOR_HIST_SIZE;
  if(avg_new_states[0] < color_thrs[0] && avg_new_states[1] < color_thrs[1]){
    return true;
  }
  return false;
}
int cal_result(){
  int l=get_color_detect(0);
  int r=get_color_detect(1);
  if(l||r){
    if(l&&r)return 1;
    if(l)return 2;
    else return 3;
  }
  return 0;
}
void reset_color_states(){
  rep(j,2)rep(i,COLOR_SENSOR_HIST_SIZE)color_states[j][i] = 0;
  rep(j,2)rep(i,COLOR_SENSOR_HIST_SIZE)color_old_states[j][i] = 0;
}

void force_turn_90(int to){
  if(to==1)turn(HALF_STOP_SP,-HALF_STOP_SP);
  else turn(-HALF_STOP_SP,HALF_STOP_SP);
  delay(800);
  rep(i,10)states[i] = 1;
   while (true)  
  {
    sensor.set();
    delay(25);
    int state = sensor.get_state();
    if (state == 0b01110 || state == 0b01100 || state == 0b00110 || state == 0b00100 || state == 0b11111)new_state(0);
    else new_state(1);
    get_sum_state();
    if(sum_state < 3)break;
  }
  rep(i,10)states[i] = 0;
}
int sw_hist[100];
int sw_hist_idx = 0;
int sum_sw_hist(){
  int sum = 0;
  rep(i,100)sum += sw_hist[i];
  return sum;
}

// 0 line trace
// 1 comn
int mode = 0;
void loop() {
  if(false){
    sensor.set();
    sensor.color_led(sensor.next_led());
    delay(25);
    sensor.debug_color_raw(true);
    return;
  }
  if(mode == 0){
    sensor.set();
    sensor.color_led(Sensor::LED_STATUS::RED);
    //sensor.color_led(sensor.next_led());
    //sensor.debug_color_raw(true);
    delay(25);

    int color_sensor_result = cal_result();

    sw_hist[sw_hist_idx++]=!digitalRead(18);
    sw_hist_idx %= 100;
    
    if(sum_sw_hist() > 80){
      Serial.println("start syougai");
      straight(-RUN_SP);
      delay(1500);
      turn(-RUN_SP,RUN_SP);
      delay(1100);
      straight(RUN_SP);
      delay(3000);
      turn(RUN_SP,-RUN_SP);
      delay(1100);
      straight(RUN_SP);
      delay(3000);
      turn(RUN_SP,-RUN_SP);
      delay(1100);
      rep(i,10)states[i] = 0;
      rep(i,100)sw_hist[i] = 0;
    }
    
    get_sum_state();

    if((sensor.get_color_state() == 0b01110||sensor.get_color_state() == 0b00100)&&color_sensor_result!=0&&get_color_new_detect()){
      Serial.println("color detect");
      straight(HALF_STOP_SP);
      delay(1000);
      if(color_sensor_result==1){
        Serial.println("180 turn");
        force_turn_90(0);
        Serial.println("90 turn");
        force_turn_90(0);
        Serial.println("180 fin");
      }else if(color_sensor_result == 2){
        Serial.println("90 turn");
        force_turn_90(0);
      }else if(color_sensor_result == 3){
        Serial.println("-90 turn");
        force_turn_90(1);
      }
      reset_color_states();
    }
    // 真ん中が同時についたら直進する
    switch (sensor.get_state())
    {
    case 0b11111:
    case 0b01110:
    case 0b00100:
      straight(RUN_SP);
      new_state(0);
      break;
    case 0b01100:
      turn(RUN_SP, HALF_STOP_SP);
      new_state(1);
      break;
    case 0b00110:
      turn(HALF_STOP_SP, RUN_SP);
      new_state(-1);
      break;
    case 0b10000:
      turn(RUN_SP, STOP_SP);
      new_state(2);
      break;
    case 0b00001:
      turn(STOP_SP, RUN_SP);
      new_state(-2);
      break;
    case 0b11000:
      turn(RUN_SP, STOP_SP);
      new_state(10);
      break;
    case 0b11100:
      turn(RUN_SP, STOP_SP);
      new_state(30);
      break;
    case 0b11110:
      turn(HALF_STOP_SP, RUN_SP);
      new_state(20);
      break;
    case 0b00011:
      turn(STOP_SP, RUN_SP);
      new_state(-10);
      break;
    case 0b00111:
      turn(STOP_SP, RUN_SP);
      new_state(-30);
      break;
    case 0b01111:
      turn(HALF_STOP_SP, RUN_SP);
      new_state(-20);
      break;
    case 0b00000:
      Serial.println("!!!");
      get_sum_state();
      straight(HALF_STOP_SP);
      delay(300);
      if(sum_state >= 20){
        turn(HALF_STOP_SP,-HALF_STOP_SP);
      }else if(sum_state <= -20){
        turn(-HALF_STOP_SP,HALF_STOP_SP);
      }else{
        straight(HALF_STOP_SP);
      }
      rep(i,10)states[i] = 1;
      int cnt = 0;
      while (true)  
      {
        sensor.set();
        delay(25);
        int state = sensor.get_state();
        if (state == 0b01110 || state == 0b01100 || state == 0b00110 || state == 0b00100 
            ||  state == 0b00010 || state == 0b01000 || state == 0b11111)new_state(0);
        else new_state(1);
        get_sum_state();
        if(sum_state < 2)break;
        if(cnt++ > 30){
          mode=1;
          comn.set_mode(1);
          return;
        }
      }
      rep(i,10)states[i] = 0;
      break;
    default:
      straight(RUN_SP);
      break;
    }
  }else{
    sensor.set();
    int a =0;
    while(sensor.get_state()!=0b00000){
      a++;
      delay(10);
      if(a>10){
        mode = 0;
        comn.set_mode(0);
        return;
      }
      sensor.set();
    }
    comn.loop();
  }
  ///// line trace end
}