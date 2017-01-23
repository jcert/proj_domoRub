//includes
#include <LedControl.h>
#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"

#define DEBUG true
enum sensor { IR, OSC };
struct mensagem { sensor tipo; int valor;};


//Armazena os dados enviados
int dados[1];

//Inicializa a placa nos pinos 9 (CE) e 10 (CS) do Arduino
RF24 radio(9,10);

//Define o endereco para comunicacao entre os modulos
const uint64_t pipe = 0xE14BC8C8C8C8;

//Define os pinos dos botoes
int pino_botao1 = 7;
int pino_botao2 = 2;

//matriz 8x8
LedControl lc = LedControl(51,52,53, 1); //3 primeiros sao os pinos do spi e o ultimo eh quantos controladores estao ligados
bool mostra_terremoto;
bool mostra_chama;
#define tempo_display_chama 		10 //tempo, em segundos, após acabar o estado que ele mantem na tela a mensagem de chama 
#define tempo_display_terremoto 	10 //tempo, em segundos, após acabar o estado que ele mantem na tela a mensagem de terremoto 
long stamp_display_chama;
long stamp_display_terremoto;

int buffer_ler_chama = false;

bool display_symbol[2]; 	//[0] se em chama, [1] se em terremoto
char index=0;
volatile char buffer_display[8]; 	//cada um dos pondos do display em matriz

const char display_vazio[8] = 
	{0b11111111,
	 0b10000001,
	 0b10000001,
	 0b10000001,
	 0b10000001,
	 0b10000001,
	 0b10000001,
	 0b11111111}; 

const char display_upper_chama[4] = 
	{0b10111101,
	 0b10100101,
	 0b10111101,
	 0b10000001};

const char display_lower_terre[4] = 
	{0b10011001,
	 0b10011001,
	 0b10011001,
	 0b10011001};

volatile char display_col_index = 0; //qual linha do display deve ser atualizada


//sensor de chama
#define pin_chama A0
#define limiar_chama 500 // descobrir quanto ir uma chama manda e colocar aqui

//sensor de queda
#define pin_queda 2
#define tempo_sample 2 //em segundos o tempo de medidas
#define limiar_trocas  2  //numero de vezes que ele deve trocar durante o tempo de sample
int ultimo_estado; //qual o ultimo estado em que esteve a chave de tilt
long ultimo_stamp;
int trocas;        //

bool ler_chama(int ir_sentido) {
  if (DEBUG) {
    Serial.print("ir: ");
    Serial.println(ir_sentido);
  }
  buffer_ler_chama = ir_sentido < limiar_chama; //global com o ultimo valor lido
  return ir_sentido < limiar_chama;//quanto mais intensa a chama menor o valor do sensor
};

void ler_oscila(int nova_medida) {
  if ((millis() - ultimo_stamp) < (tempo_sample * 1000)) {
    if (ultimo_estado != nova_medida) trocas++;
    ultimo_estado = nova_medida;
  } else {
    if (DEBUG) {
      Serial.print("oscila: ");
      Serial.println(trocas);
    }
    trocas = 0;
    ultimo_stamp = millis();
  }
};

void se_oscilando_muito() {
  if (trocas > limiar_trocas) mostra_terremoto = true;
  else mostra_terremoto = false;
};

void se_muito_ir() {
  if (buffer_ler_chama) mostra_chama = true;
  else mostra_chama = false;
};

void display_chama() {
  if (mostra_chama) {
    stamp_display_chama = millis();
    //mostra o simbolo de chama
    display_symbol[0] = true;
  }
  if ((stamp_display_chama + tempo_display_chama * 1000) < millis()) {
    //apaga o display
    display_symbol[0] = false;
  }
};

void display_terremoto() {
  if (mostra_terremoto) {
    stamp_display_terremoto = millis();
    //mostra o simbolo de terremoto
    display_symbol[1] = true;
  }
  if ((stamp_display_terremoto + tempo_display_terremoto * 1000) < millis()) {
    //apaga o display
    display_symbol[1] = false;
  }
};

void display_refresh(){
	//mostra no display os simbolos necessarios de acordo com o vetor de booleanos, se esta em chama ou/e terremoto
	lc.setRow(0,index,buffer_display[index]);		//escreve na linha e vai para a proxima
	++index/=8;//zera ao atingir 8
};

void modifica_display_buffer(){
	if(display_symbol[0]){
    lc.setRow(0,0,display_upper_chama[0]);
    lc.setRow(0,1,display_upper_chama[1]);
    lc.setRow(0,2,display_upper_chama[2]);
    lc.setRow(0,3,display_upper_chama[3]);
	}else{
    
    lc.setRow(0,0,display_vazio[0]);
    lc.setRow(0,1,display_vazio[1]);
    lc.setRow(0,2,display_vazio[2]);
    lc.setRow(0,3,display_vazio[3]);
	}
	if(display_symbol[1]){
    lc.setRow(0,4,display_lower_terre[0]);
    lc.setRow(0,5,display_lower_terre[1]);
    lc.setRow(0,6,display_lower_terre[2]);
    lc.setRow(0,7,display_lower_terre[3]);
	}else{
    lc.setRow(0,4,display_vazio[4]);
    lc.setRow(0,5,display_vazio[5]);
    lc.setRow(0,6,display_vazio[6]);
    lc.setRow(0,7,display_vazio[7]);
	}
};

void setup() {
  // put your setup code here, to run once:
  //Timer1.initialize(2000);//400Hz
  //Timer1.attachInterrupt(display_refresh); 
  lc.shutdown(0,false);//acorda o driver de led
  lc.setIntensity(0,8);//intensidade do brilho
  Serial.begin(9600);
  trocas = 0;
  ultimo_estado = digitalRead(pin_queda);
  mostra_chama = false;
  mostra_terremoto = false;
  
  //Inicializa a comunicacao
  radio.begin();
  //Entra em modo de transmissao
  radio.openWritingPipe(pipe);
}

void loop() {
  
  while(radio.available()){
    struct mensagem recebido;
    radio.read(&recebido,sizeof(recebido));
    switch(recebido.tipo){
      case IR:
        ler_chama(recebido.valor);
      break;
      case OSC:
        ler_oscila(recebido.valor);      
      break;
      }
    }

  se_oscilando_muito();//se sim, está em terremoto
  se_muito_ir();//tem uma chama no comodo

  display_terremoto();
  display_chama();

  modifica_display_buffer();
  delay(200);
}



