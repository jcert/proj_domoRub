//includes
#include <LedControl.h>

#define DEBUG true
//matriz 8x8
LedControl lc=LedControl(12,11,10,1); 


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
	}else{
		trocas = 0;
	}
  };

void se_oscilando_muito(){
	if(trocas > alternacoes) mostra_terremoto();
};

void se_muito_ir(){
	if(ler_chama()) mostra_chama();
};

void mostra_terremoto(){
//usa a matriz 8x8 para mostrar simbolo de terremoto
}

void mostra_chama(){
//usa a matriz 8x8 para mostrar simbolo de chama
};

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  trocas = 0;
  ultimo_estado = digitalRead(pin_queda);
}
void loop() {
  // put your main code here, to run repeatedly:
  ler_chama();
  ler_oscila();

  se_oscilando_muito();//se sim, está em terremoto
  se_muito_ir();//tem uma chama no comodo
  delay(300);

}

