#include <PS4Controller.h>

void setup() {
  Serial.begin(115200);
  PS4.begin("88:13:BF:82:4A:6A");
  Serial.println("Ready.");
}

void loop() {
  if (PS4.isConnected()) {
    int x = PS4.LStickX();
    int y = PS4.LStickY();
    Serial.print(String(x) + "," + String(y) + "\n");
    
    if (PS4.RStickX() < -30){
      Serial.print("L\n");
    } else if(PS4.RStickX() > 30){
      Serial.print("R\n");
    }
    delay(20);
  }
}