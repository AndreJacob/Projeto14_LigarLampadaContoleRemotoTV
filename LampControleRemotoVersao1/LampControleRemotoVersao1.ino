#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRutils.h>

const uint16_t RECV_PIN = 14;       // Pino do receptor IR
const uint16_t LED_PIN = 2;         // Pino do LED
const uint32_t IR_CODE = 0x807F02FD;  // Código IR do botão

IRrecv irrecv(RECV_PIN);
decode_results results;
bool ledState = false;  // Estado inicial do LED

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  irrecv.enableIRIn();
  Serial.println("Aguardando sinal IR...");
}

void loop() {
  if (irrecv.decode(&results)) {
    Serial.println(resultToHumanReadableBasic(&results));

    // Verifica se o código IR recebido corresponde ao botão desejado
    if (results.value == IR_CODE) {
      ledState = !ledState;               // Alterna o estado do LED
      digitalWrite(LED_PIN, ledState);    // Atualiza o LED com o novo estado
    }

    irrecv.resume();  // Aguarda o próximo sinal IR
  }
}
