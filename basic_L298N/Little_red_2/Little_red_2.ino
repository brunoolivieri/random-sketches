//See Low Level for Command Definitions
 
//Define Pins
int enableA = 10;
int pinA1 = 9;
int pinA2 = 8;
 
int enableB = 5;
int pinB1 = 6;
int pinB2 = 7;

char serial;
 
//Define Run variable
boolean run;


void setup() {
 
 Serial.begin(9600);       // inicializa a Serial
 delay(1000);              // esperar a serial subir o mini firmware externo
  
 pinMode(enableA, OUTPUT);
 pinMode(pinA1, OUTPUT);
 pinMode(pinA2, OUTPUT);
 
 pinMode(enableB, OUTPUT);
 pinMode(pinB1, OUTPUT);
 pinMode(pinB2, OUTPUT);
 
 enableMotors();
  run = false;
  
}
 
//command sequence
void loop() {
   if (Serial.available()) {    
    serial = Serial.read();
    Serial.println(serial);  
            switch (serial) {
              //segmento do controle de locomocao
              case ' ': // parar motores
                brake(0);
                break;    
              case 'W': // ir para frente              
                forward(0);              
                break;
              case 'S': // ir para tras           
                backward(0);
                break;
              case 'D': // ir para direita
                turnRight(0);
                break;
              case 'A': // ir para esquerda
                turnLeft(0);
                break;
              case 'P': // Play the Demo mode
                run = true;
                break;
//////////////////////////////////////////////////////////////////////////////
// TRECO DE ICMU  //
              case 'V': // ir para esquerda
                Serial.println("#v_receipt"); 
                break;
              case 'B': // ir para esquerda
                Serial.println("#b_receipt"); 
                break;
              case 'N': // ir para esquerda
                Serial.println("#n_receipt"); 
                break;
              case 'M': // ir para esquerda
                Serial.println("#m_receipt"); 
                break;
//////////////////////////////////////////////////////////////////////////////                
                
              default: // comando fora do protocolo 
                    Serial.println("Comando desconhecido");          
            }        
     Serial.flush();// preciso mesmo disso? sera que se perde pacotes nisso? sinais que tenham chego no processamento dos comendos... Bem, so uma ideia   
  }// final do if de serial disponivel
  
    
   if(run)
   {
     //delay(2000);
     //enableMotors();
     forward(1000);
     coast(500);
     backward(1500);
     coast(500);
     forward(500);
     brake(500);
     turnLeft(500);
     turnRight(500);
     //disableMotors(); 
     run = false;
     brake(0);
   }
 
}
 
//Define Low Level H-Bridge Commands
 
//enable motors
void motorAOn()
{
 digitalWrite(enableA, HIGH);
}
 
void motorBOn()
{
 digitalWrite(enableB, HIGH);
}
 
 //disable motors
void motorAOff()
{
 digitalWrite(enableB, LOW);
}
 
void motorBOff()
{
 digitalWrite(enableA, LOW);
}
 
 //motor A controls
void motorAForward()
{
 digitalWrite(pinA1, HIGH);
 digitalWrite(pinA2, LOW);
}
 
void motorABackward()
{
 digitalWrite(pinA1, LOW);
 digitalWrite(pinA2, HIGH);
}
 
//motor B contorls
void motorBForward()
{
 digitalWrite(pinB1, HIGH);
 digitalWrite(pinB2, LOW);
}
 
void motorBBackward()
{
 digitalWrite(pinB1, LOW);
 digitalWrite(pinB2, HIGH);
}
 
//coasting and braking
void motorACoast()
{
 digitalWrite(pinA1, LOW);
 digitalWrite(pinA2, LOW);
}
 
void motorABrake()
{
 digitalWrite(pinA1, HIGH);
 digitalWrite(pinA2, HIGH);
}
 
void motorBCoast()
{
 digitalWrite(pinB1, LOW);
 digitalWrite(pinB2, LOW);
}
 
void motorBBrake()
{
 digitalWrite(pinB1, HIGH);
 digitalWrite(pinB2, HIGH);
}
 
//Define High Level Commands
 
void enableMotors()
{
 motorAOn();
 motorBOn();
}
 
void disableMotors()
{
 motorAOff();
 motorBOff();
}
 
void forward(int time)
{
 motorAForward();
 motorBForward();
 delay(time);
}
 
void backward(int time)
{
 motorABackward();
 motorBBackward();
 delay(time);
}
 
void turnLeft(int time)
{
 motorABackward();
 motorBForward();
 delay(time);
}
 
void turnRight(int time)
{
 motorAForward();
 motorBBackward();
 delay(time);
}
 
void coast(int time)
{
 motorACoast();
 motorBCoast();
 delay(time);
}
 
void brake(int time)
{
 motorABrake();
 motorBBrake();
 delay(time);
}
