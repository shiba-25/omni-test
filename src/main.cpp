#include "mbed.h"
#define Radian M_PI/180
BufferedSerial esp(PA_9,PA_10,115200);
BufferedSerial pc(USBTX,USBRX,115200);
char buf[16];
char *token;
int JoystickX,JoystickY,power;
int wheel[4] = {0,0,0,0};
int wheel_angle[4] = {45,135,225,315};
const int motor_max_val = 15000;
int turn;

int main(){
    CAN can(PA_11, PA_12, (int)1e6); //canを出力するピンを指定
    CANMessage msg; //変数「msg」の作成
    
    char buf_index;
    while(1){
        if(esp.readable()){
            char c;
            int len = esp.read(&c,1);
            if(len > 0){
                if(c == '\n'){
                    if(buf_index > 0){
                        buf[buf_index] = '\0';
                        buf_index = 0;

                        if(buf == "R"){
                            turn = 1;
                        } else if(buf == "L"){
                            turn = -1;
                        } else{
                            turn = 0;
                        }
                        if(buf != "R",buf != "L"){
                            char *str = buf; //bufの中身をstrに保存
                            token = strtok(str,","); //","で区切る
                            JoystickX = atoi(token); //文字列を数値に変換
                            token = strtok(NULL,","); //","で区切る
                            JoystickY = atoi(token); //文字列を数値に変換
                        }
                    }
                } else{
                    if(buf_index < sizeof(buf - 1)){
                        buf[buf_index++] = c;
                    }
                }
            }
        }

        // ジョイスティックの倒した強さと角度を計算
        power = sqrt(pow(JoystickX,2.0)+pow(JoystickY,2.0));
        float Joystickdir = atan2(JoystickX,JoystickY);
        if(Joystickdir < 0) Joystickdir += 360.0f * Radian;
        if(JoystickX < 20 && JoystickX > -20 && JoystickY < 20 && JoystickY > -20) power = 0; //デッドゾーンの作成

        // オムニのそれぞれの比率を計算、または旋回
        if(turn != 0){
            for(int i = 0; i < 4; i++){
                wheel[i] = motor_max_val*turn;
            }
        }else {
            for(int i = 0; i < 4; i++){
                wheel[i] = min((int)sin(Joystickdir-wheel_angle[i]*Radian)*power*100,motor_max_val);
            }
        }
        
        CANMessage msg(2, (const uint8_t *)wheel, 8); //特に理由がない限りwhile直下
        can.write(msg); //特に理由がない限りwhile直下
    }
}