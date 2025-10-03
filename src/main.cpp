#include "mbed.h"
#define Radian M_PI/180
BufferedSerial esp(PA_9,PA_10,115200);
BufferedSerial pc(USBTX,USBRX,115200);
int16_t pwm[4] {0};

int main(){
    CAN can(PB_12, PB_13, (int)1e6); // canを出力するピンを指定
    CANMessage msg;// 変数「msg」の作成

    char buf[16];
    char *token;
    int JoystickX,JoystickY,power;
    int wheel[4] = {0,0,0,0};
    int wheel_dig[4] = {45,135,225,315};
    while(1){
        if(pc.readable()){
            int n = pc.read(buf, sizeof(buf) - 1);
            if(n > 0){
                buf[n] = '\0';
                if(buf[n-1] == '|'){
                    buf[n-1] = '\0';
                }
                if(buf != "R",buf != "L"){
                    char *str = buf;
                    token = strtok(str,",");
                    JoystickX = atoi(token);
                    token = strtok(NULL,",");
                    JoystickY = atoi(token);
                }
            }
        }
        power = sqrt(pow(JoystickX,2.0)+pow(JoystickY,2.0));
        float Joystickdir = atan2(JoystickX,JoystickY);
        if(Joystickdir < 0){
            Joystickdir += 360.0f * Radian;
        }
        for(int i = 0; i < 4; i++){
            wheel[i] = sin(Joystickdir-wheel_dig[i]*Radian);
            pwm[i] =  wheel[i]*power*100;
        }
        printf("%d,%d",JoystickX,JoystickY);
        CANMessage msg(2, (const uint8_t *)pwm, 8); //特に理由がない限りwhile直下
        can.write(msg); //特に理由がない限りwhile直下
    }
}