#include <IRremoteESP8266.h>      // Biblioteca para controle infravermelho (IR)
#include <IRrecv.h>               // Biblioteca para receber sinais IR
#include <IRutils.h>              // Biblioteca utilitária para IR
#include <WiFi.h>                 // Biblioteca para conectividade Wi-Fi
#include <ESPAsyncWebServer.h>    // Biblioteca para servidor web assíncrono

const char* ssid = "Cobertura";          // Nome da rede Wi-Fi
const char* password = "Dev@!2024";      // Senha da rede Wi-Fi

const uint16_t RECV_PIN = 14;            // Pino de entrada do receptor IR
const uint16_t LED_PIN = 2;              // Pino do LED que representa a lâmpada
const uint32_t IR_CODE = 0x807F02FD;     // Código IR do botão que controla a lâmpada

IRrecv irrecv(RECV_PIN);                 // Instancia o receptor IR no pino especificado
decode_results results;                  // Armazena os resultados da decodificação IR
bool ledState = false;                   // Estado inicial da lâmpada (desligada)

AsyncWebServer server(80);               // Instancia um servidor web na porta 80

void setup() {
  Serial.begin(115200);                  // Inicializa a comunicação serial para depuração
  pinMode(LED_PIN, OUTPUT);              // Configura o pino do LED como saída
  irrecv.enableIRIn();                   // Habilita o receptor IR para começar a receber sinais

  WiFi.begin(ssid, password);            // Conecta ao Wi-Fi com o SSID e senha especificados
  while (WiFi.status() != WL_CONNECTED) { // Espera até a conexão Wi-Fi ser estabelecida
    delay(1000);
    Serial.println("Conectando ao WiFi...");
  }
  Serial.println("Conectado ao WiFi");   // Imprime uma mensagem de sucesso na conexão
  Serial.print("Endereço IP: ");
  Serial.println(WiFi.localIP());        // Imprime o endereço IP do ESP32 na rede

  // Define a página principal da web com HTML e estilo
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    String html = R"rawliteral(
    <!DOCTYPE html>
    <html lang='en'>
    <head>
      <meta charset='UTF-8'>
      <title>Status da Lâmpada</title>
      <style>
        body {
          background-color: #1e1e2e;
          color: #f3f3f3;
          display: flex;
          justify-content: center;
          align-items: center;
          height: 100vh;
          font-family: Arial, sans-serif;
          margin: 0;
        }
        .container {
          text-align: center;
          padding: 20px;
          border-radius: 15px;
          background-color: #333;
          box-shadow: 0 4px 8px rgba(0, 0, 0, 0.3);
        }
        .lamp-status {
          font-size: 3em;
          margin-top: 20px;
        }
      </style>
      <script>
        setInterval(async () => {
          const response = await fetch('/lampada-status'); // Faz uma requisição para o status da lâmpada
          const data = await response.json();
          document.getElementById('lamp-status').innerText = data.status; // Atualiza o texto com o status recebido
        }, 1000);
      </script>
    </head>
    <body>
      <div class='container'>
        <h1>Status da Lâmpada</h1>
        <div class='lamp-status' id='lamp-status'>Desligada</div>
      </div>
    </body>
    </html>)rawliteral";
    request->send(200, "text/html", html);  // Envia a página HTML ao navegador do cliente
  });

  // Define a rota para obter o estado atual da lâmpada em formato JSON
  server.on("/lampada-status", HTTP_GET, [](AsyncWebServerRequest *request){
    String status = ledState ? "Ligada" : "Desligada";   // Determina o estado atual da lâmpada
    request->send(200, "application/json", "{\"status\": \"" + status + "\"}"); // Envia o estado como JSON
  });

  server.begin();                      // Inicia o servidor web
}

void loop() {
  if (irrecv.decode(&results)) {       // Verifica se há um sinal IR recebido
    if (results.value == IR_CODE) {    // Verifica se o código IR é o esperado
      ledState = !ledState;            // Alterna o estado da lâmpada
      digitalWrite(LED_PIN, ledState); // Atualiza o LED com o novo estado
    }
    irrecv.resume();                   // Aguarda o próximo sinal IR
  }
}
