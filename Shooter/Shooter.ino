#include <Servo.h> 

// Constants
String prompt = "devil@shooter:~# ";
byte PAN_pin = 7;
byte TILT_pin = 9;
byte max_TILT = 150;
byte min_TILT = 65;
              
// Vars          
int pos = 0;    
String stringBuffer = "";
Servo srv_PAN;  
Servo srv_TILT;  
int count_coord_parts = 0 ;

// loop string stackoverflow
char intBuffer[12];
String intRxDataBuffer = "";
int delimiter = '#';

// loop novo
String inputString = "";        
boolean stringComplete = false; 

void setup() 
{ 
  Serial.begin(57600);
  srv_PAN.attach(PAN_pin); 
  srv_TILT.attach(TILT_pin); 
  String stringBuffer = "";
  anwserBack("System online.");
} 
 
void anwserBack(String anwser){
  String temp = "";
  temp.concat(prompt);
  temp.concat(anwser);
  Serial.println(temp);
  String stringBuffer = "";
}

void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read(); 
    // add it to the inputString:
    inputString += inChar;
    // if the incoming character is a newline, set a flag
    // so the main loop can do something about it:
    if (inChar == delimiter) {
      stringComplete = true;
    } 
  }
}
 
void teste(){
  for(pos = min_TILT; pos < max_TILT; pos += 1)  // goes from 0 degrees to 180 degrees 
  {                                  // in steps of 1 degree 
    srv_TILT.write(pos);              // tell servo to go to position in variable 'pos' 
    srv_PAN.write(pos);    
    delay(15);                       // waits 15ms for the servo to reach the position 
  } 
  for(pos = max_TILT; pos>=min_TILT; pos-=1)     // goes from 180 degrees to 0 degrees 
  {                                
    srv_TILT.write(pos);              // tell servo to go to position in variable 'pos' 
    srv_PAN.write(pos);    
    delay(15);                       // waits 15ms for the servo to reach the position 
  } 
  srv_TILT.write(90);  
  srv_PAN.write(90);    
} 
 
void parseCMDs(String CMD){
  if (CMD.startsWith("$ECHO$")) {
    CMD.concat("   <<<  ECHO command receipt");
    anwserBack(CMD); 
  }
  if (CMD.startsWith("$TEST$")) {
    anwserBack("Testing ..."); 
    teste();
  }
  if (CMD.startsWith("$LEDON$")) {
    anwserBack("LED on ..."); 
    digitalWrite(13, HIGH);
  }
  if (CMD.startsWith("$LEDOFF$")) {
    anwserBack("LED off  ..."); 
    digitalWrite(13, LOW);
  }
  if (CMD.startsWith("$COORD$")) {
    char charBuf[50];
    CMD.toCharArray(charBuf, 50) ;
    char *p = charBuf;
    char *str;
    count_coord_parts = 0;
    while ((str = strtok_r(p, ";", &p)) != NULL){ // delimiter is the semicolon
      count_coord_parts++;      
      if (count_coord_parts == 2) { // PAN position
        anwserBack(String(count_coord_parts)); 
        int temp = atoi(str);
        anwserBack(str); 
        srv_PAN.write(temp);  
      }
      if (count_coord_parts == 3) { // TILT position
        anwserBack(String(count_coord_parts)); 
        int temp = atoi(str);
        anwserBack(str); 
        if (temp > max_TILT) {
           srv_TILT.write(max_TILT);  
        }
        else if (temp < min_TILT) {
               srv_TILT.write(min_TILT);  
        } else {
               srv_TILT.write(temp);  
        }
      }
                                                                            
    }
  }


} 
 
 
void loop() 
{ 
  // print the string when a newline arrives:
  if (stringComplete) {
    parseCMDs(inputString);
    // clear the string:
    inputString = "";
    stringComplete = false;
  }
  
  
  
/*
  while (Serial.available()) {
        int ch = Serial.read();
        if (ch == -1) {
            anwserBack("Deu pau na entrada de dados.");
        }
        else if (ch == delimiter) {
            break;
        }
        else {
            intRxDataBuffer += (char) ch;
        }
    }
    // Copy read data into a char array for use by atoi
    // Include room for the null terminator
    int intLength = intRxDataBuffer.length() + 1;
    intRxDataBuffer.toCharArray(intBuffer, intLength);
    // Reinitialize intRxDataBuffer for use next time around the loop
    intRxDataBuffer = "";
    // Convert ASCII-encoded integer to an int
    int i = atoi(intBuffer);
 
 */
 
  
  
} 
