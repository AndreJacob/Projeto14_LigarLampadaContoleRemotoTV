#include <WiFi.h>
#include <WebServer.h>
#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRutils.h>

const char* ssid = "Cobertura";
const char* password = "Dev@!2024";
const uint16_t RECV_PIN = 14;
const uint16_t LED_PIN = 2;
const uint32_t IR_CODE = 0x807F02FD;

WebServer server(80);
IRrecv irrecv(RECV_PIN);
decode_results results;
bool ledState = false;

void handleRoot() {
  String html = "<html><body style='text-align: center;'>";
  html += "<h1>Status da Lâmpada</h1>";
  html += "<h2 style='color: " + String(ledState ? "green'>Ligada" : "red'>Desligada") + "</h2>";
  html += "<p>Atualize a página para ver o estado mais recente</p>";
  html += "</body></html>";
  server.send(200, "text/html", html);
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  irrecv.enableIRIn();

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Conectado ao WiFi");

  server.on("/", handleRoot);
  server.begin();
  Serial.println("Servidor iniciado");

  Serial.println("Aguardando sinal IR...");
}

void loop() {
  server.handleClient();

  if (irrecv.decode(&results)) {
    if (results.value == IR_CODE) {
      ledState = !ledState;
      digitalWrite(LED_PIN, ledState);
    }
    irrecv.resume();
  }
}
