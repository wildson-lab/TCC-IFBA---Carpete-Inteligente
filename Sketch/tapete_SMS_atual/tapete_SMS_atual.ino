/* 
Protótipo para tapete sensível à pressão (detecção de queda através da área pressionada)
Criado em 24/12/2014. Atualizado em 05/04/2015
Wildson Oliveira

Montar o circuito da seguinte forma:

Pino   3: Ligar ao buzzer;
Pinos  6, 7, 8 e 9: Ligar cada um a uma fileira de push-butons;
Pino   10: LED verde;
Pino   11: LED amarelo;
Pino   12: LED vermelho;
Pino   A0: mySerial RX;
Pino   A1: mySerial TX;
Pino   A4: Display LCD SDA
Pino   A5: Display LCD SCL
*/


#include <Wire.h>
#include <LCD.h>
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>

// Cria nova comunicação serial com o shield através dos pinos A0 e A1 (RX, TX)
SoftwareSerial mySerial(A0, A1); 

// Localização e quantidade das fileiras de botões. Para modificar a quantidade de fileiras no tapete, alterar estas variáveis..
int pins[4] = {6,7,8,9};
int nPins = 4;

// Limite de fileiras acionados ao mesmo tempo
int limite = 3;                

// Leds de sinalização
int ledOK = 10;                // Led de status normal.
int ledAtencao = 11;           // Led de atenção.
int ledAlerta = 12;            // Led de alerta.

// Delays utilizados no programa
const int mainTimer = 1000;    // Tempo entre as verificações de queda.
const int alertTimer = 2000;   // Tempo que o programa esperará até emitir o alerta, em caso de emergencia.

//Inicia o display LCD no endereço 0x20:
LiquidCrystal_I2C lcd(0x20,4, 5, 6, 0, 1, 2, 3, 7, NEGATIVE);

void setup(){  
  //Inicia a comunicação serial com o PC
  Serial.begin(9600);                                        // Inicia a conexao serial.
  Serial.println("A conexao serial foi bem estabelecida.");  // Informa a serial que a conexao foi bem sucedida.

  //Inicia a comunicação serial com o shield
  mySerial.begin(9600);
  mySerial.println("1");

  // Inicia o LCD, e acende sua luz de fundo
  lcd.begin (16,2);
  lcd.setBacklight(1);  

  // Declara como INPUT todos os pinos declarados em pins[]  
  for (int x = 0; x < nPins; x++){
      pinMode(pins[x], INPUT); 
  }

  // Declara como saída os pinos conectados aos leds  
  pinMode (ledOK, OUTPUT);
  pinMode (ledAtencao, OUTPUT);
  pinMode (ledAlerta, OUTPUT);

  // Aguarda 30 segundos até o carregamento do shield. 
  // Imprime mensagem no display e acende o led amarelo.
  
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Aguardando");
  lcd.setCursor(0,1);
  lcd.print("shield...");
  
  ledManager(ledAtencao);
  delay(30000);

  //   
  ledManager(ledOK);      // Acende o led verde 
  tone(3, 3250, 500);     // Emite um pequeno sinal sonoro
}

void loop(){
  // Se houver detecção de queda (forem acionados simultaneamente mais botões que o limite)
  if ( buttonCheck() > limite){
    atencao();      // Coloca sistema em estado de atenção.
      
    // Se os sensores ainda detectarem a queda         
    if ( buttonCheck() > limite){
      alerta();        // Executa os procedimentos de emergência.
    }
      
    // Ou retorna ao monitoramento, caso o alarme seja falso.      
    else{
      Serial.println("Alarme falso. Retomando o monitoramento..."); 
    }
  }

  // Se não houver detecção, coloca o sistema no estado normal.  
  else{ 
    ok();
  }
  
  delay(mainTimer);    // Intervalo entre os monitoramentos.
}

// Função ok(): Procedimentos executados no estado normal do sistema.

void ok(){
  // Acende o led verde
  ledManager(ledOK);
  
  // Imprime que tudo está normal no LCD
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Monitoramento");
  lcd.setCursor(0,1);
  lcd.print("Status: Normal");   
}


// Função atencao(): Procedimentos a serem executados em caso de suspeita de queda.

void atencao(){
  // Acende a luz amarela
  ledManager(ledAtencao);

  // Imprime no LCD que uma possível queda foi detectada
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Atencao!");
  lcd.setCursor(0,1);
  lcd.print("Possivel queda!"); 

  // Comunica também via serial:
  Serial.println("Possivel queda detectada! Caso o sensor permaneca detectando a queda, um alarme soara em 2 segundos..."); 

  // Aguarda o tempo alertTimer para verificar novamente.  
  delay(alertTimer);  
}


// Função alerta(): Procedimentos executados quando a suspeita de queda for confirmada.

void alerta(){
  ledManager(ledAlerta);                          // Acende a luz vermelha

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Alerta!");
  lcd.setCursor(0,1);
  lcd.print("Queda detectada!"); 

  Serial.println("Alerta de emergencia!");        // Manda um alerta de emergencia via serial.

  // Envia um comendo para o shield enviar o SMS
  if (mySerial.available()){
    mySerial.println(2);
  }
  
  while (buttonCheck() > limite ){          
  tone(3, 440, 1000);                           // Emite um alarme enquanto os sensores detectarem a queda.
  }  
}


// Função ledManager: Acende apenas o led recebido como parâmetro.

void ledManager(int led){
  digitalWrite(ledAtencao, LOW);
  digitalWrite(ledAlerta, LOW);
  digitalWrite(ledOK, LOW);                         // Apaga todos os leds
  
  digitalWrite(led, HIGH);                          // Aciona o led que foi recebido como parâmetro.
}


// Função buttonCheck: Verifica quantos botoes estao acionados simultaneamente (retorna tal quantidade como um inteiro).
int buttonCheck(){
   int y = 0; 
   for (int x = 0; x < nPins; x++){                    // Para todos os índices do array pins[]
       int value = digitalRead(pins[x]);               // Faz a leitura digital de cada um e armazena seu valor no inteiro value. 
       y = (y + value);                                // Soma o resultado da leitura à soma das leituras anteriores.
   }
   return (y);                                         // Retorna a soma dos valores dos botoes, ou seja, a quantidade de botoes acionados.
}

