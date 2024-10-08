#ifndef Motor_h
#define Motor_h

#include <Arduino.h>

class Motor
{
private:
  int M_A, M_1, M_2;

public:
  Motor(int _M_A, int _M_1, int _M_2) : M_A(_M_A), M_1(_M_1), M_2(_M_2) {};
  // setup
  void setup()
  {
    pinMode(M_A, OUTPUT);
    pinMode(M_1, OUTPUT);
    pinMode(M_2, OUTPUT);
  };
  void turn(int speed)
  {
    if (speed > 0)
    {
      digitalWrite(M_1, HIGH);
      digitalWrite(M_2, LOW);
      analogWrite(M_A, speed);
    }
    else
    {
      digitalWrite(M_1, LOW);
      digitalWrite(M_2, HIGH);
      analogWrite(M_A, speed * -1);
    }
  };
};

#endif