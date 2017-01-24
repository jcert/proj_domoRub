//Com Pedro, Lucas Delapria, Jair, Mario Catanio

//LED 8x8  DIN=7, CS=5, CLK=6

#include <nRF24L01.h>
#include <printf.h>
#include <RF24.h>
#include <RF24_config.h>

#define DEBUG true

long stamp_display_chama;
long stamp_display_terremoto;

RF24 radio(9, 10);
const uint64_t pipe = 0xE14BC8C8C8C8;

enum sensor { IR, OSC};
struct mensagem {
  sensor tipo; int valor;
};

//sensor de chama
#define pin_chama A0

//sensor de queda
#define pin_queda 2
#define tempo_sample 2 //em segundos o tempo de medidas
int ultimo_estado; //qual o ultimo estado em que esteve a chave de tilt
long ultimo_stamp;

int ler_chama() {
  int ir_sentido = analogRead(pin_chama);
  return ir_sentido ;//quanto mais intensa a chama menor o valor do sensor
};

int ler_oscila() {
  int nova_medida = digitalRead(pin_queda);
  return nova_medida;
};

void setup() {
  Serial.begin(57600);
  radio.begin();
  radio.openWritingPipe(pipe);
  ultimo_estado = digitalRead(pin_queda);
}
void loop() {
  struct mensagem enviado;
	
  enviado.valor = ler_chama();
  enviado.tipo = IR;
  radio.write(&enviado, sizeof(enviado));

  enviado.valor = ler_oscila();
  enviado.tipo = OSC;
  radio.write(&enviado, sizeof(enviado));

  delay(200);
}
