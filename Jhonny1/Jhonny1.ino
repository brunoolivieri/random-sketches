/* ==================================================================================================================*/
//
//      Programa de controle via Arduino do Johnny 1
//
//      Autor: Bruno Olivieri
//      Release: 2013/03/07
//
//      Componentes do prototipo
//        - Arduino Mega (DX.com sku )
//        - Motor Shield LadyADA (*detalhe) (DX.com sku )
//        - Placa com dois Reles, acionados digitalmente (DX.com sku )
//        - Placa Bluetooth, para uso com o arduino (DX.com sku )
//        - Bateria LiPo 3S (11.1v) (DX.com sku )
//        - Dois Servos motores (DX.com sku )
//        - Quatro motores 80RPM 12V (DX.com sku ) (DX.com sku )
//        - Mira Laser XPTO (DX.com sku )
//        - Softgun comprada no Walmart
//        - Rodas de brinquedo reutilizadas
//        - Suportes de cortinas para a armacao
//        - Madeira Balsa 6mm para o corpo [ 2 x (30cm x 30cm)]
//
/* ==================================================================================================================*/



// includes
#include <Servo.h> 
#include <AFMotor.h> // biblioteca dos motores com o shield da LadyADA

// constantes
const byte pino_mira = 9;
const byte pino_gatilho = 10;
const byte pino_pan = 5;
const byte pino_tilt = 6;
const byte max_servo = 210;
const byte med_servo = 110;
const byte min_servo = 0;
const byte passo_motores = 10;  // Para o servo e para o motor tambem, passo de cada movimento
const byte passo_servos = 1;  // Para o servo e para o motor tambem, passo de cada movimento
const byte max_motor = 250;
const byte min_motor = 0;
const byte multi_velo = 25; // max_motor dividida por passo_motores


// variaveis
Servo servo_pan;   // servo do eixo X
Servo servo_tilt;  // servo do eixo Y
byte pos_pan;      // posicao do servo do eixo X
byte pos_tilt;     // posicao do servo do eixo X
byte VME;      //velo dos motores da esquerda
byte VMD;      //velo dos motores da direita
int VAD;  //velo do lado direito
int VAE;  //velo do lado esquerdo
int VAF;  //velo frontal geral
char serial;       // CHAR lida na Serial 
char str_buffer[64]; // para casos onde ha conversao de valores

// Definicoes dos motores referentes a potencia da placa grande
AF_DCMotor motor_esquerdo(1, MOTOR12_64KHZ);
AF_DCMotor motor_direito(2, MOTOR12_64KHZ);







/* ==================================================================================================================*/
// Procedure padrao arduino de inicializacao
//
void setup() { 
  //pinMode(led, OUTPUT);     // pegando o led onboard, pra que mesmo???
  Serial.begin(9600);       // inicializa a Serial
  delay(1000);
  // setando os servos
  servo_pan.attach(pino_pan); 
  servo_tilt.attach(pino_tilt); 
  // posicionando no meio
  pos_pan = med_servo;
  pos_tilt = med_servo;
  servo_pan.write(med_servo);
  servo_tilt.write(med_servo);
  
  //brincando de dar uma volta com eles
  /*servo_pan.write(min_servo);
  servo_tilt.write(min_servo);
  delay(3000);
  servo_pan.write(max_servo);
  servo_tilt.write(max_servo); 
  delay(3000);
  */  

  // velocidades todas zeradas
  VAF = 0;
  VAD = 0; 
  VAE = 0;  
  VMD = 0;
  VME = 0;

  // iniciando os motores parados
  motor_esquerdo.setSpeed(VME);  
  motor_direito.setSpeed(VMD); 
  
  // "setando" os pinos dos relays
  pinMode(pino_mira, OUTPUT);
  pinMode(pino_gatilho, OUTPUT);
  digitalWrite(pino_gatilho,LOW);
  digitalWrite(pino_mira,LOW);
  
  delay(200);
  
}
/* ==================================================================================================================*/






/* ==================================================================================================================*/
/* Bloco de controle do PANTILT - Codigo feio... tinha que ser melhor reaproveitado...  */
//
void eixo_pan_esquerda()
{
    if (pos_pan < max_servo) {
     pos_pan = pos_pan + passo_servos;
     servo_pan.write(pos_pan);
     sprintf(str_buffer, "Pan para ESQUERDA %d", pos_pan);
     Serial.println(str_buffer);
     } 
     else { 
     servo_pan.write(max_servo); 
     Serial.println("Jah esta no maximo da esquerda");
     }
} /////////////////////////////////////////////////

void eixo_pan_direita()
{
   if (pos_pan > min_servo) {
       pos_pan = pos_pan - passo_servos;
       servo_pan.write(pos_pan);
       sprintf(str_buffer, "Pan para DIREITA %d", pos_pan);
       Serial.println(str_buffer);     
    } 
    else {
        servo_pan.write(min_servo);
        Serial.println("Jah esta no minimo da direita");
    }
}/////////////////////////////////////////////////

void eixo_tilt_cima()
{
    if (pos_tilt < max_servo) {
        pos_tilt = pos_tilt + passo_servos;
        servo_tilt.write(pos_tilt);
        sprintf(str_buffer, "TILT para CIMA %d", pos_tilt);
        Serial.println(str_buffer);
     } 
     else { 
        servo_tilt.write(max_servo); 
        Serial.println("Jah esta no maximo da CIMA");
     }
} /////////////////////////////////////////////////

void eixo_tilt_baixo()
{
    if (pos_tilt > min_servo) {
      pos_tilt = pos_tilt - passo_servos;
      servo_tilt.write(pos_tilt);
      sprintf(str_buffer, "TILT para BAIXO %d", pos_tilt);
      Serial.println(str_buffer);     
    } 
    else {
        servo_tilt.write(min_servo);
        Serial.println("Jah esta no minimo pra BAIXO");
    }
}/////////////////////////////////////////////////

/* ==================================================================================================================*/









/* ==================================================================================================================*/
// Bloco de controle dos motores
//
/* ==================================================================================================================*/


// avanca, reduz e inverte direcao dos motores da direita
void mov_direita(int avanco) {
 
 // variavel de controle de velocidade do lado direito, ficando entre [-multi_velo..multi_velo]
 if ((avanco > 0) && (VAD < multi_velo)) {  
  VAD++;
   } else {
    if ((avanco < 0) && (VAD > -multi_velo)) {  
     VAD--;    
    }
  }  
   // aumentando a velocidade no eixo POSITIVO (indo para a frente)
   if (VAD > 0) {      
     // verificando se nova velocidade vai ficar esta no range [min_motor..max_motor] 
     if ((VMD + avanco) >= min_motor && (VMD + avanco) <= max_motor) {                
        VMD = VMD + avanco;
        motor_direito.setSpeed(VMD); 
        motor_direito.run(FORWARD); 
     }
   }            
   // aumentando a velocidade no eixo NEGATIVO (indo para tras)
   if ((VAD < 0)){  
     avanco = - avanco; // lembrando que o controlador do motor so trabalha com valores positivos        
     // verificando se nova velocidade vai ficar esta no range [min_motor..max_motor] 
     if ((VMD + avanco) >= min_motor && (VMD + avanco) <= max_motor) {        
        VMD = VMD + avanco;
        motor_direito.setSpeed(VMD); 
        motor_direito.run(BACKWARD);         
     }
    }   
 Serial.print("VAD: "); Serial.println(VAD);
 Serial.print("VMD: "); Serial.println(VMD); 
}/////////////////////////////////////////////////




// avanca, reduz e inverte direcao dos motores da esquerda
void mov_esquerda(int avanco) {
 
 // variavel de controle de velocidade do lado direito, ficando entre [-multi_velo..multi_velo]
 if ((avanco > 0) && (VAE < multi_velo)) {  
  VAE++;
   } else {
    if ((avanco < 0) && (VAE > -multi_velo)) {  
     VAE--;    
    }
  }  
   // aumentando a velocidade no eixo POSITIVO (indo para a frente)
   if (VAE > 0) {      
     // verificando se nova velocidade vai ficar esta no range [min_motor..max_motor] 
     if ((VME + avanco) >= min_motor && (VME + avanco) <= max_motor) {                
        VME = VME + avanco;
        motor_esquerdo.setSpeed(VME); 
        motor_esquerdo.run(FORWARD); 
     }
   }            
   // aumentando a velocidade no eixo NEGATIVO (indo para tras)
   if ((VAE < 0)){  
     avanco = - avanco; // lembrando que o controlador do motor so trabalha com valores positivos        
     // verificando se nova velocidade vai ficar esta no range [min_motor..max_motor] 
     if ((VME + avanco) >= min_motor && (VME + avanco) <= max_motor) {        
        VME = VME + avanco;
        motor_esquerdo.setSpeed(VME); 
        motor_esquerdo.run(BACKWARD);         
     }
    }   
 Serial.print("VAE: "); Serial.println(VAE);
 Serial.print("VME: "); Serial.println(VME); 
}/////////////////////////////////////////////////



// caso esteja saindo de uma curva, alinha a velocidade da reta pelo maiorr
void alinha_motores() {
  if (VAD != VAE) {
    if (VAE > VAD) { // o motor mais rapido segue o motor mais lento
      VAD = VAE;
    } else {
      VAD = VAE;
    }
  } 
}/////////////////////////////////////////////////

// :-)
void parada_total() {
  Serial.println("RELEASE nos motores");
  motor_esquerdo.run(RELEASE);      
  motor_direito.run(RELEASE);
  VAF = 0;
  VAD = 0;
  VAE = 0;
  VME = 0;
  VMD = 0;
  
}/////////////////////////////////////////////////
/* ==================================================================================================================*/









/* ==================================================================================================================*/
// Bloco padrao do Arduino sendo executavo indefinidamente
//
void loop() {
  
  if (Serial.available()) {    
    serial = Serial.read();
    Serial.println(serial);  
            switch (serial) {
            //segmento do controle de locomocao
            case ' ': // parar motores
              parada_total();
              break;    
            case 'W': // ir para frente              
              if (VAF < multi_velo){
                VAF++;
                alinha_motores();
                mov_direita(passo_motores);
                mov_esquerda(passo_motores);
              }
              Serial.print("VAF: "); Serial.println(VAF);              
              break;
            case 'S': // ir para tras           
              if (VAF > -multi_velo){
                VAF--;
                alinha_motores();
                mov_direita(-passo_motores);
                mov_esquerda(-passo_motores);
                Serial.print("VAF: "); Serial.println(VAF);
              }              
              break;
            case 'D': // ir para direita
              mov_direita(-passo_motores);
              mov_esquerda(passo_motores);               
              break;
            case 'A': // ir para esquerda
              mov_direita(passo_motores);
              mov_esquerda(-passo_motores);              
              break;
              
              
              
              
             
            // segmento de controle da visao
            case 'L': // pedindo para virar o pantilt para a direita
              eixo_pan_direita();
              break;
            case 'J': // pedindo para virar o pantilt para a esquerda
              eixo_pan_esquerda();
              break;
            
            
            case 'I': // pedindo para virar o pantilt para cima
              eixo_tilt_cima();
              break;
            case 'K': // pedindo para virar o pantilt para baixo
              eixo_tilt_baixo();
              break;
               
               
            case '4': // pedido para ligar a mira laser
              digitalWrite(pino_mira,HIGH);
              break;
                              
            case '5': // pedido para ligar a mira laser
              digitalWrite(pino_mira,LOW);
              break;
 
               
            case '6': // pedido para atirar
              digitalWrite(pino_gatilho,HIGH);
              break;
                              
            case '7': // pedido para parar de atirar
              digitalWrite(pino_gatilho,LOW);
              break;
            
              
               
            default: // comando fora do protocolo 
                  Serial.println("Comando desconhecido");
          
            }
            
     Serial.flush();// preciso mesmo disso? sera que se perde pacotes nisso? sinais que tenham chego no processamento dos comendos... Bem, so uma ideia
    
  }// final do if de serial disponivel
  
  
  
}
