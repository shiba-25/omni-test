#include "mbed.h"
#include <cmath>
#include "math.h"
#include "stdio.h"
#include <string.h>
#define Radian M_PI*180
BufferedSerial pc(USBTX, USBRX, 115200);

int main(){

    CAN can(PB_12, PB_13, (int)1e6); // canを出力するピンを指定
    CANMessage msg;// 変数「msg」の作成

    /*変数、配列の宣言*/
    int i;
    float JoyStickX,JoyStickY,power;
    char buf[64];
    char *token;
    float n[4] {0,90*Radian,90*Radian,0};
    float m[4] {90*Radian,0,0,90*Radian};
    int Minus[4] {1,-1,-1,1};
    float wheel[4] {0};//0=LeftUp,1=LeftDown,2=RightUp,3=RightDown
    int16_t pwm[4] {0};

    while(true){
        if (pc.readable()){
            int n = pc.read(buf, sizeof(buf) - 1);
            if (n > 0){
                buf[n] = '\0';
                if (buf[n-1] == '\n'||buf[n-1] == '\r'){
                    buf[n-1] = '\0';
                }
                if(buf[0] != 'L' && buf[0] != 'R'){
                    char *str = buf;
                    token = strtok(str,",");//,で区切る
                    JoyStickX = atof(token)-atof(token)/2;
                    token = strtok(NULL,",");
                    JoyStickY = atof(token)-atof(token)/2;
                }
            }
        }
        if(buf[0] == 'R'){
            /*右旋回の命令*/
            power = 10000;
            pwm[0] = power;
            pwm[1] = power;
            pwm[2] = -power;
            pwm[3] = -power;
        }else if(buf[0] == 'L'){
            /*左旋回の命令*/
            power = 10000;
            pwm[0] = -power;
            pwm[1] = -power;
            pwm[2] = power;
            pwm[3] = power;
        }else{
            /*直進の命令*/
            power = sqrt(pow(JoyStickX,2.0)+pow(JoyStickY,2.0));
            float JoyStickDir = atan2(abs(JoyStickY),abs(JoyStickX));
            for(i=0;i<4;i++){
                if(JoyStickX > 10 && JoyStickY > 10){
                    if(JoyStickDir > 45*Radian){
                        wheel[i] = sin(JoyStickDir+45*Radian+n[i]);
                    } else {
                        wheel[i] = sin(JoyStickDir-45*Radian+m[i])*Minus[i];
                    }
                }else if(JoyStickX < -10 && JoyStickY > 10){
                    if(JoyStickDir > 45*Radian){
                        wheel[i] = sin(JoyStickDir+45*Radian+m[i]);
                    }else {
                        wheel[i] = -sin(JoyStickDir-45*Radian+n[i])*Minus[i];
                    }
                }else if(JoyStickX > 10 && JoyStickY < -10){
                    if(JoyStickDir > 45*Radian){
                        wheel[i] = -sin(JoyStickDir+45*Radian+m[i]);
                    }else {
                        wheel[i] = sin(JoyStickDir-45*Radian+n[i])*Minus[i];
                    }
                }else if(JoyStickX < -10 && JoyStickY < -10){
                    if(JoyStickDir > 45*Radian){
                        wheel[i] = -sin(JoyStickDir+45*Radian+n[i]);
                    }else {
                        wheel[i] = -sin(JoyStickDir-45*Radian+m[i])*Minus[i];
                    }
                }
            
                pwm[i] = power*wheel[i];
                
            }
        }
        CANMessage msg(2, (const uint8_t *)pwm, 8); //特に理由がない限りwhile直下
        can.write(msg); //特に理由がない限りwhile直下
        ThisThread::sleep_for(chrono::milliseconds(10));
    }
}
