#ifndef _comn_h_
#define _comn_h_

#include <Arduino.h>

class Comn {
    public:
        void loop();
        void motor(int speedl, int speedr);
        bool linetracing = false;
        bool color_sensor[2] = {false,false};
        void set_mode(int mode){
            Serial.print("s:");
            Serial.println(mode);
        }
    private:
        int _cmd_idx = 0;
        String _cmd[3] = {"", "", ""};
};

#endif