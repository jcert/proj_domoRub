//includes
#include <LedControl.h>

#define DEBUG true
//matriz 8x8
LedControl lc=LedControl(12,11,10,1);//3 primeiros sao os pinos do spi e o ultimo eh quantos controladores estao ligados 
bool mostra_terremoto;
bool mostra_chama;
#define tempo_display 10 //tempo, em segundos, após acabar o estado que ele mantem a a mensagem exibindo na tela 
long stamp_display_chama;
long stamp_display_terremoto;


 
//sensor de chama
#define pin_chama A0
#define limiar_chama 500 // descobrir quanto ir uma chama manda e colocar aqui

//sensor de queda
#define pin_queda 2
#define tempo_sample 2 //em segundos o tempo de medidas
#define alternacoes  2  //numero de vezes que ele deve trocar durante o tempo de sample
int ultimo_estado; //qual o ultimo estado em que esteve a chave de tilt
long ultimo_stamp;
int trocas;        //

bool ler_chama(){
  int ir_sentido = analogRead(pin_chama);
  if(DEBUG) Serial.println(ir_sentido);  
  return ir_sentido < limiar_chama;//quanto mais intensa a chama menor o valor do sensor
  };

void ler_oscila(){
	if((millis()-ultimo_stamp)<(tempo_sample*1000)){
		int nova_medida = digitalRead(pin_queda);
		if (ultimo_estado != nova_medida) trocas++;
   	ultimo_estado = nova_medida;
  	}else{
		trocas = 0;		
		ultimo_stamp = millis();
	}
  };

void se_oscilando_muito(){
  if(trocas > alternacoes) mostra_terremoto = true;
  else mostra_terremoto = false;
};

void se_muito_ir(){
  if(ler_chama()) mostra_chama = true;
  else mostra_chama = false;
};

void display_chama(){
  if(mostra_chama){
    stamp_display_chama = millis();
    //mostra o simbolo de chama
    }
  if((stamp_display_chama+tempo_display*1000)<millis()){
    //apaga o display   
  }
};

void display_terremoto(){
  if(mostra_terremoto){
    stamp_display_terremoto = millis();
    //mostra o simbolo de chama
    }
  if((stamp_display_terremoto+tempo_display*1000)<millis()){
    //apaga o display   
  }
};
  
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  trocas = 0;
  ultimo_estado = digitalRead(pin_queda);
  mostra_chama = false;
  mostra_terremoto = false;
}
void loop() {
  ler_chama();
  ler_oscila();

  se_oscilando_muito();//se sim, está em terremoto
  se_muito_ir();//tem uma chama no comodo

  display_terremoto();
  display_chama();
  delay(300);

}

