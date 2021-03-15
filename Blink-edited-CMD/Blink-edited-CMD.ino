
int LED_BUILTIN_foo = 2; // 5 na placa do BRUNO, 2 na placa do BRENO!
String strint_test;

void setup() {
  pinMode(LED_BUILTIN_foo, OUTPUT);
  Serial.begin(115200);
  delay(100);
  Serial.println("Hello World!");
}

void loop() {

  while(Serial.available())  
  {  
      strint_test = Serial.readString( );  
      
      if (strint_test.equals("A\n")) {
        digitalWrite(LED_BUILTIN_foo, HIGH); 
        Serial.println("CMD_ON");
      } else if (strint_test.equals("B\n")) {
        digitalWrite(LED_BUILTIN_foo, LOW); 
        Serial.println("CMD_OFF"); 
      } else {
        Serial.print("CMD_UNKOWN: " + strint_test); 
      } 
  }  

}
