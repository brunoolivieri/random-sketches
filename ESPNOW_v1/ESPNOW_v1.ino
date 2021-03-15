// First and basic wi-fi mesh based on ESPNOW

#include <esp_now.h>
#include <WiFi.h>

// Addresses
// uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
// uint8_t broadcastAddress[] = {C4;4F;33;56;76;CD};
// uint8_t broadcastAddress[] = {98;F4;AB;39;CE;10};

void setup(){
  Serial.begin(115200);
  WiFi.mode(WIFI_MODE_STA);
  Serial.print("This ESP32 MAC Address: " );
  Serial.println(WiFi.macAddress());
}
 
void loop(){

}
