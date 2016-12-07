//includes
#include <IRremoteInt.h>
#include <IRremote.h>
#include <LedControl.h>

#define DEBUG true
//matriz 8x8



//sensor de chama
#define pin_chama A0
#define limiar_chama 500 // descobrir quanto ir uma chama manda e colocar aqui

//sensor de queda
#define pin_queda 2
#define tempo_sample 2 //em segundos o tempo de medidas
#define alternacoes  2  //numero de vezes que ele troca durante o tempo de sample
//LedControl lc=LedControl(12,11,10,1); 

bool ler_chama(){
  int ir_sentido = analogRequedaad(pin_chama);
  if(DEBUG) Serial.println(ir_sentido);  
  return ir_sentido >= limiar_chama;
  };

void ler_oscila(){
  
  };

void se_oscilando_muito(){};

void se_muito_ir(){};

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  
}
void loop() {
  // put your main code here, to run repeatedly:
  ler_chama();
  ler_oscila();

  se_oscilando_muito();//se sim, está em terremoto
  se_muito_ir();//tem uma chama no comodo
  

}

