#include <WiFi.h>
#include <WebServer.h>
#include <BluetoothSerial.h>
BluetoothSerial SerialBT;

#define BUZZER_PIN 13

// Frequências padrão
int freqX = 262;
int freqS = 294;
int freqC = 330;

// Buffer de gravação
char musica[100];
int indice = 0;
bool gravando = false;

// Cria o servidor web
WebServer server(80);

// Inicia como Access Point
const char* ssid = "BateriaESP32";
const char* password = "12345678";

void tocarNota(char nota) {
  switch (nota) {
    case 'X':
      tone(BUZZER_PIN, freqX, 200);
      break;
    case 'S':
      tone(BUZZER_PIN, freqS, 200);
      break;
    case 'C':
      tone(BUZZER_PIN, freqC, 200);
      break;
  }
}

// Página principal com form
void handleRoot() {
  String html = "<html><body><h2>Configurar Frequências</h2>"
                "<form action='/set' method='GET'>"
                "X: <input type='number' name='x' value='" + String(freqX) + "'><br>"
                "S: <input type='number' name='s' value='" + String(freqS) + "'><br>"
                "C: <input type='number' name='c' value='" + String(freqC) + "'><br><br>"
                "<input type='submit' value='Salvar'>"
                "</form></body></html>";
  server.send(200, "text/html", html);
}

// Tratamento do formulário
void handleSet() {
  if (server.hasArg("x")) freqX = server.arg("x").toInt();
  if (server.hasArg("s")) freqS = server.arg("s").toInt();
  if (server.hasArg("c")) freqC = server.arg("c").toInt();
  server.send(200, "text/html", "<html><body><h2>Frequências atualizadas!</h2><a href='/'>Voltar</a></body></html>");
}

void setup() {
  Serial.begin(115200);
  SerialBT.begin("BateriaESP32");

  pinMode(BUZZER_PIN, OUTPUT);

  // Iniciar Wi-Fi AP
  WiFi.softAP(ssid, password);
  Serial.println("WiFi AP iniciado. Acesse: 192.168.4.1");

  // Configurar rotas
  server.on("/", handleRoot);
  server.on("/set", handleSet);
  server.begin();
}

void loop() {
  server.handleClient();

  if (SerialBT.available()) {
    char comando = SerialBT.read();
    Serial.println(comando);

    if (comando == 'F') {
      gravando = true;
      indice = 0;
      Serial.println("Gravando...");
    } else if (comando == 'L') {
      gravando = false;
      Serial.println("Gravação parada.");
    } else if (comando == 'R') {
      Serial.println("Reproduzindo música...");
      for (int i = 0; i < indice; i++) {
        tocarNota(musica[i]);
        delay(300);
      }
    } else if (comando == 'X' || comando == 'S' || comando == 'C') {
      tocarNota(comando);

      if (gravando && indice < 100) {
        musica[indice++] = comando;
      }
    }
  }
}