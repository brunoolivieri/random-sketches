
int LED_BUILTIN_foo = 5; // 5 na placa do BRUNO, 2 na placa do BRENO!
String strint_test;

void setup() {
  pinMode(LED_BUILTIN_foo, OUTPUT);
  Serial.begin(115200);
  delay(100);
  Serial.println("Hello World!  :-)");
}

void loop() {

  Serial.println("Entrando...");

  digitalWrite(LED_BUILTIN_foo, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);                       // wait for a second
  digitalWrite(LED_BUILTIN_foo, LOW);    // turn the LED off by making the voltage LOW
  delay(1000);    
  digitalWrite(LED_BUILTIN_foo, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(500);                        // wait for a second
  digitalWrite(LED_BUILTIN_foo, LOW);    // turn the LED off by making the voltage LOW
  delay(500);    
  digitalWrite(LED_BUILTIN_foo, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);                       // wait for a second
  digitalWrite(LED_BUILTIN_foo, LOW);    // turn the LED off by making the voltage LOW
  delay(1000);    

  Serial.println("Saindo...");
  
}
