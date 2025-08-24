#include <PS4Controller.h>
char buf[64];

void setup() {
  Serial.begin(115200);
  PS4.begin("88:13:BF:82:4A:6A");
  Serial.println("Ready.");
}

void loop() {
  // Below has all accessible outputs from the controller
  if (PS4.isConnected()) {
    if (PS4.L2()) {
      Serial.println("L");
    }else if (PS4.R2()) {
      Serial.println("R");
    }else {
      Serial.println(String(PS4.LStickX()) + "," + String(PS4.LStickY()));
    }
  }
}
