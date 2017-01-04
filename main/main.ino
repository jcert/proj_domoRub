//includes
#include <LedControl.h>

#define DEBUG true
//matriz 8x8
LedControl lc = LedControl(12, 11, 10, 1); //3 primeiros sao os pinos do spi e o ultimo eh quantos controladores estao ligados
bool mostra_terremoto;
bool mostra_chama;
#define tempo_display_chama 		10 //tempo, em segundos, após acabar o estado que ele mantem na tela a mensagem de chama 
#define tempo_display_terremoto 	10 //tempo, em segundos, após acabar o estado que ele mantem na tela a mensagem de terremoto 
long stamp_display_chama;
long stamp_display_terremoto;

bool display_symbol[2]; 	//[0] se em chama, [1] se em terremoto

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

ISR(TIMER2_COMPA_vect){
	display_refresh();
}

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

bool ler_chama() {
  int ir_sentido = analogRead(pin_chama);
  if (DEBUG) {
    Serial.print("ir: ");
    Serial.println(ir_sentido);
  }
  return ir_sentido < limiar_chama;//quanto mais intensa a chama menor o valor do sensor
};

void ler_oscila() {
  if ((millis() - ultimo_stamp) < (tempo_sample * 1000)) {
    int nova_medida = digitalRead(pin_queda);
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
  if (ler_chama()) mostra_chama = true;
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
	setRow(1,i++,buffer_display[i]);		//escreve na linha e vai para a proxima
	i/=8;//zera ao atingir 8
};

void modifica_display_buffer(){
	if(display_symbol[0]){
		buffer_display[0] = display_upper_chama[0];
		buffer_display[1] = display_upper_chama[1];
		buffer_display[2] = display_upper_chama[2];
		buffer_display[3] = display_upper_chama[3];
	}else{
		buffer_display[0] = display_vazio[0];
		buffer_display[1] = display_vazio[1];
		buffer_display[2] = display_vazio[2];
		buffer_display[3] = display_vazio[3];
	}
	if(display_symbol[1]){
		buffer_display[4] = display_lower_terre[0];
		buffer_display[5] = display_lower_terre[1];
		buffer_display[6] = display_lower_terre[2];
		buffer_display[7] = display_lower_terre[3];
	}else{
		buffer_display[4] = display_vazio[4];
		buffer_display[5] = display_vazio[5];
		buffer_display[6] = display_vazio[6];
		buffer_display[7] = display_vazio[7];
	}
};

void setup() {
  // put your setup code here, to run once:
  lc.shutdown(0,false);//acorda o driver de led
  lc.setIntensity(0,8);//intensidade do brilho
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

  modifica_display_buffer();
  //display_refresh();//quase por interrupcao,,,,talvez colocar por interrupcao
  delay(300);

}

