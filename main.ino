/* 
------------------ FIAP --------------------
CHECKPOINT 4 - EDGE COMPUTING
EDGE COMPUTING & COMPUTER SYSTEMS

Participantes:
Davi de Jesus Duarte - RM566316
Jéssica Lais Domingues Flauzino da Silva - RM562973
Kauã Carvalho de Oliveira - RM566371
Leonardo Lopes Pereira - RM561349
Matheus Brasil Borges Sevilha Angelotti - RM561456
--------------------------------------------

Canal ThingSpeak para Processamento dos Dados
https://thingspeak.mathworks.com/channels/3047442

*/

#include <WiFi.h>
#include <HTTPClient.h>
#include <DHT.h>

#define DHTPIN 15 // Pino GPIO15 do ESP32 para o DHT22
#define DHTTYPE DHT22 // Tipo de sensor DHT (DHT22)
DHT dht(DHTPIN, DHTTYPE);

#define POT_PIN 34 // Pino GPIO34 do ESP32 para o Potenciômetro

// Credenciais
const char* ssid = "Wokwi-GUEST"; // Rede Wi-Fi
const char* password = ""; // Senha da rede Wi-Fi
const char* apiKey = "B5OTDEMUTQFA114J"; // Write API Key
const char* server = "http://api.thingspeak.com"; // Servidor ThingSpeak

void setup() {
  Serial.begin(9600);
  dht.begin();

  // Configuração dos pinos
  pinMode(POT_PIN, INPUT);

  // Inicialização e loop de verificação da rede Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Conectando ao WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" conectado!");
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    // Leitura dos sensores
    float h = dht.readHumidity();
    float t = dht.readTemperature();
    int ldrValue = analogRead(POT_PIN); // Leitura do valor do LDR
    float luminosidade = map(ldrValue, 0, 1023, 100, 0); // Mapeamento do valor do sensor de luminosidade

    if (isnan(h) || isnan(t)) {
      Serial.println("Falha ao ler o sensor DHT11!");
      return;
    }

    // Envio de dados para o ThingSpeak
    HTTPClient http;
    String url = String(server) + "/update?api_key=" + apiKey + "&field1=" + String(t) +
                                 "&field2=" + String(h) + "&field3=" + String(luminosidade);
    http.begin(url);

    int httpCode = http.GET();
    if (httpCode > 0) {
      String payload = http.getString(); // Resposta da requisição HTTP
      Serial.println("Dados enviados ao ThingSpeak.");
      Serial.print("Código HTTP: ");
      Serial.println(httpCode);
      Serial.println("Resposta: ");
      Serial.println(payload);
    } else {
      Serial.print("Erro ao enviar dados. Código HTTP: ");
      Serial.println(httpCode);
    }
    
    http.end();
  } else {
    Serial.println("WiFi não conectado. Tentando reconectar...");
  }
  
  // Espera 15 segundos para enviar a requisição novamente
  delay(2500);
}
