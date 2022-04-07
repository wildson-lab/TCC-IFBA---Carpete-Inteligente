/*
Programa de envio de SMS com o GSM Shield, a partir de comando via serial
Escrito por Wildson oliveira, baseado no código de FILIPEFLOP
*/

#include "SIM900.h"
#include <SoftwareSerial.h>
#include "sms.h"
 
SMSGSM sms;
 
int numdata;
boolean started=false;
char smsbuffer[160];
char n[20];
 
void setup(){
  //Inicializa a serial 
  Serial.begin(9600);
  Serial.print("Testando GSM shield... ");
  
  //Inicia a configuracao do Shield
  if (gsm.begin(2400)){
  Serial.print("nstatus=READY ");
  Serial.println(0x1);
  started=true;
  } 
  
  else Serial.print("nstatus=IDLE ");
  Serial.print(">>"); 
}
 
void loop(){
  
int serialRead = 0;  

  if (Serial.available() > 0) {
    serialRead = Serial.read()-48;
    
    if (serialRead == 0x2) {
      if(started) {
           if (
             sms.SendSMS("99108820", "Alerta! Queda detectada! [Teste com o carpete inteligente]") /* &&
             sms.SendSMS("XXXXXXXX", "Alerta! Queda detectada! [Teste com o carpete inteligente]") 
             Adicionar aqui todos os números que receberão o SMS */
           )
           Serial.print("nSMS sent OK ");
      }
    }
   }

}
