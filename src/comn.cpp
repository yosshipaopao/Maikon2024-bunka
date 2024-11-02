#include<comn.h>


void Comn::loop() {
    if (Serial.available() > 0) {
        char c = Serial.read();
        if (c == '\n') {
            _cmd_idx = 0;
            // do something with _cmd
            switch (_cmd[0][0])
            {
            // `m:idx:speed` -> motor(idx, speed)
            case 'm':
                motor(_cmd[1].toInt(), _cmd[2].toInt());
                break;
            // `l:(1/0) -> linetrace(on/off)
            case 'l':
                linetracing = _cmd[1].toInt();
                break;
            case 's':
                Serial.println("sensor ok");
                color_sensor[_cmd[1].toInt()] = _cmd[2].toInt();
                break;
            default:
                break;
            }
            _cmd[0] = "";
            _cmd[1] = "";
            _cmd[2] = "";
        } else if(c == ':'){
            _cmd_idx++;
        } else {
            _cmd[_cmd_idx] += c;
        }
    }
}