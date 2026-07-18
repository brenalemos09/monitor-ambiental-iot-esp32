#include <WiFi.h>
#include <WebServer.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <DHT.h>

// ======================================================
// PINOS E COMPONENTES
// ======================================================

#define DHTPIN 4
#define DHTTYPE DHT11
#define LDR_PIN 34

#define LED_VERDE 18
#define LED_AZUL 19
#define LED_VERMELHO 21
#define BOTAO_PIN 23

// ======================================================
// WI-FI
// ======================================================

const char* WIFI_NOME = "...";
const char* WIFI_SENHA = "...";

// ======================================================
// SUPABASE
// ======================================================

const char* SUPABASE_URL =
  "https://qwjqkmdupmyidmgegccj.supabase.co/rest/v1/leituras_ambientais";

// Cole somente a chave que começa com sb_publishable_
const char* SUPABASE_KEY =
  "...";

// ======================================================
// OBJETOS
// ======================================================

DHT dht(DHTPIN, DHTTYPE);
WebServer servidor(80);

// ======================================================
// VARIÁVEIS DOS SENSORES
// ======================================================

float temperatura = 0;
float umidade = 0;
int luminosidade = 0;

bool leituraValida = false;

String nivelLuz = "DESCONHECIDO";
String statusAmbiente = "INICIANDO";

// ======================================================
// BOTÃO E ALERTA
// ======================================================

bool alertaReconhecido = false;
bool botaoAnterior = HIGH;

// ======================================================
// CONTROLE DE TEMPO
// ======================================================

unsigned long ultimaLeituraSensores = 0;
unsigned long ultimaExibicaoSerial = 0;
unsigned long ultimoEnvioSupabase = 0;

const unsigned long INTERVALO_SENSORES = 2000;
const unsigned long INTERVALO_SERIAL = 2000;
const unsigned long INTERVALO_SUPABASE = 30000;

// ======================================================
// LEITURA DOS SENSORES
// ======================================================

void atualizarSensores() {
  float novaTemperatura = dht.readTemperature();
  float novaUmidade = dht.readHumidity();

  if (!isnan(novaTemperatura) && !isnan(novaUmidade)) {
    temperatura = novaTemperatura;
    umidade = novaUmidade;
    leituraValida = true;
  } else {
    leituraValida = false;
    Serial.println("Falha ao ler o DHT11.");
  }

  luminosidade = analogRead(LDR_PIN);
}

// ======================================================
// CLASSIFICAÇÃO DA LUMINOSIDADE
// ======================================================

void classificarLuminosidade() {
  if (luminosidade <= 2300) {
    nivelLuz = "MUITO CLARO";
  } else if (luminosidade <= 3300) {
    nivelLuz = "NORMAL";
  } else {
    nivelLuz = "ESCURO";
  }
}

// ======================================================
// STATUS AMBIENTAL E LEDs
// ======================================================

void atualizarStatus() {
  if (temperatura > 30.0 || umidade > 85.0) {
    statusAmbiente = "ALERTA";

    digitalWrite(LED_VERDE, LOW);
    digitalWrite(LED_AZUL, LOW);
    digitalWrite(LED_VERMELHO, HIGH);

  } else if (temperatura > 28.0 || umidade > 75.0) {
    statusAmbiente = "ATENCAO";

    digitalWrite(LED_VERDE, LOW);
    digitalWrite(LED_AZUL, HIGH);
    digitalWrite(LED_VERMELHO, LOW);

  } else {
    statusAmbiente = "NORMAL";

    digitalWrite(LED_VERDE, HIGH);
    digitalWrite(LED_AZUL, LOW);
    digitalWrite(LED_VERMELHO, LOW);

    alertaReconhecido = false;
  }
}

// ======================================================
// CRIAÇÃO DO JSON
// ======================================================

String criarJson() {
  String json = "{";

  json += "\"temperatura\":";
  json += String(temperatura, 2);

  json += ",\"umidade\":";
  json += String(umidade, 2);

  json += ",\"luminosidade\":";
  json += String(luminosidade);

  json += ",\"nivel_luz\":\"";
  json += nivelLuz;
  json += "\"";

  json += ",\"status\":\"";
  json += statusAmbiente;
  json += "\"";

  json += ",\"alerta_reconhecido\":";
  json += alertaReconhecido ? "true" : "false";

  json += "}";

  return json;
}

// ======================================================
// ENVIO PARA O SUPABASE
// ======================================================

void enviarParaSupabase() {
  if (!leituraValida) {
    Serial.println("Envio cancelado: leitura inválida.");
    return;
  }

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Envio cancelado: Wi-Fi desconectado.");
    return;
  }

  WiFiClientSecure clienteSeguro;
  clienteSeguro.setInsecure();

  HTTPClient http;

  if (!http.begin(clienteSeguro, SUPABASE_URL)) {
    Serial.println("Erro ao iniciar conexão com o Supabase.");
    return;
  }

  http.addHeader("Content-Type", "application/json");
  http.addHeader("apikey", SUPABASE_KEY);
  http.addHeader("Prefer", "return=minimal");

  String json = criarJson();

  Serial.println();
  Serial.println("Enviando leitura para o Supabase...");
  Serial.println(json);

  int codigoHttp = http.POST(json);

  Serial.print("Código HTTP do Supabase: ");
  Serial.println(codigoHttp);

  if (codigoHttp == 201 || codigoHttp == 200) {
    Serial.println("Leitura salva no Supabase com sucesso!");
  } else {
    Serial.println("Falha ao salvar leitura no Supabase.");

    String resposta = http.getString();

    if (resposta.length() > 0) {
      Serial.print("Resposta do servidor: ");
      Serial.println(resposta);
    }
  }

  http.end();
}

// ======================================================
// API REST LOCAL
// ======================================================

void enviarDados() {
  servidor.send(200, "application/json", criarJson());
}

// ======================================================
// DASHBOARD
// ======================================================

void enviarPaginaInicial() {
  String pagina = R"rawliteral(
<!DOCTYPE html>
<html lang="pt-BR">

<head>
  <meta charset="UTF-8">

  <meta
    name="viewport"
    content="width=device-width, initial-scale=1.0"
  >

  <title>Monitor Ambiental IoT</title>

  <style>
    body {
      font-family: Arial, sans-serif;
      background: #f4f6f8;
      margin: 0;
      padding: 20px;
      color: #222;
    }

    .container {
      max-width: 900px;
      margin: auto;
    }

    h1 {
      text-align: center;
      margin-bottom: 8px;
    }

    .subtitulo {
      text-align: center;
      color: #666;
      margin-bottom: 28px;
    }

    .grade {
      display: grid;
      grid-template-columns: repeat(auto-fit, minmax(220px, 1fr));
      gap: 16px;
    }

    .cartao {
      background: white;
      border-radius: 14px;
      padding: 22px;
      box-shadow: 0 4px 14px rgba(0, 0, 0, 0.08);
    }

    .titulo-cartao {
      font-size: 14px;
      color: #666;
      margin-bottom: 10px;
    }

    .valor {
      font-size: 30px;
      font-weight: bold;
    }

    .status {
      text-align: center;
      margin-top: 20px;
      padding: 18px;
      border-radius: 14px;
      font-size: 24px;
      font-weight: bold;
      background: white;
      box-shadow: 0 4px 14px rgba(0, 0, 0, 0.08);
    }

    .normal {
      border-left: 8px solid green;
    }

    .atencao {
      border-left: 8px solid orange;
    }

    .alerta {
      border-left: 8px solid red;
    }

    .rodape {
      text-align: center;
      margin-top: 24px;
      color: #777;
      font-size: 13px;
    }

    @media (max-width: 600px) {
      body {
        padding: 12px;
      }

      h1 {
        font-size: 26px;
      }

      .valor {
        font-size: 26px;
      }
    }
  </style>
</head>

<body>

  <div class="container">

    <h1>Monitor Ambiental IoT</h1>

    <p class="subtitulo">
      ESP32 com DHT11, LDR, LEDs, botão e Supabase
    </p>

    <div class="grade">

      <div class="cartao">
        <div class="titulo-cartao">Temperatura</div>
        <div class="valor" id="temperatura">-- °C</div>
      </div>

      <div class="cartao">
        <div class="titulo-cartao">Umidade</div>
        <div class="valor" id="umidade">-- %</div>
      </div>

      <div class="cartao">
        <div class="titulo-cartao">Luminosidade</div>
        <div class="valor" id="luminosidade">--</div>
      </div>

      <div class="cartao">
        <div class="titulo-cartao">Nível de luz</div>
        <div class="valor" id="nivelLuz">--</div>
      </div>

      <div class="cartao">
        <div class="titulo-cartao">Alerta reconhecido</div>
        <div class="valor" id="reconhecido">--</div>
      </div>

    </div>

    <div id="statusBox" class="status">
      Status: carregando...
    </div>

    <div class="rodape">
      Dashboard atualizado automaticamente a cada 2 segundos
    </div>

  </div>

  <script>
    async function atualizarDados() {
      try {
        const resposta = await fetch('/dados');
        const dados = await resposta.json();

        document.getElementById('temperatura').textContent =
          dados.temperatura.toFixed(1) + ' °C';

        document.getElementById('umidade').textContent =
          dados.umidade.toFixed(1) + ' %';

        document.getElementById('luminosidade').textContent =
          dados.luminosidade;

        document.getElementById('nivelLuz').textContent =
          dados.nivel_luz;

        document.getElementById('reconhecido').textContent =
          dados.alerta_reconhecido ? 'SIM' : 'NÃO';

        const statusBox =
          document.getElementById('statusBox');

        statusBox.textContent =
          'Status: ' + dados.status;

        statusBox.className = 'status';

        if (dados.status === 'NORMAL') {
          statusBox.classList.add('normal');

        } else if (dados.status === 'ATENCAO') {
          statusBox.classList.add('atencao');

        } else if (dados.status === 'ALERTA') {
          statusBox.classList.add('alerta');
        }

      } catch (erro) {
        document.getElementById('statusBox').textContent =
          'Erro ao carregar os dados';
      }
    }

    atualizarDados();

    setInterval(atualizarDados, 2000);
  </script>

</body>
</html>
)rawliteral";

  servidor.send(200, "text/html", pagina);
}

// ======================================================
// CONEXÃO WI-FI
// ======================================================

void conectarWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_NOME, WIFI_SENHA);

  Serial.print("Conectando ao Wi-Fi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("Wi-Fi conectado com sucesso!");

  Serial.print("Endereço IP: ");
  Serial.println(WiFi.localIP());

  Serial.print("Força do sinal: ");
  Serial.print(WiFi.RSSI());
  Serial.println(" dBm");
}

// ======================================================
// CONFIGURAÇÃO INICIAL
// ======================================================

void setup() {
  Serial.begin(115200);
  delay(1000);

  dht.begin();

  pinMode(LED_VERDE, OUTPUT);
  pinMode(LED_AZUL, OUTPUT);
  pinMode(LED_VERMELHO, OUTPUT);

  pinMode(BOTAO_PIN, INPUT_PULLUP);

  digitalWrite(LED_VERDE, LOW);
  digitalWrite(LED_AZUL, LOW);
  digitalWrite(LED_VERMELHO, LOW);

  conectarWiFi();

  servidor.on("/", enviarPaginaInicial);
  servidor.on("/dados", enviarDados);

  servidor.begin();

  Serial.println("API REST local iniciada!");
  Serial.println("Dashboard disponível no endereço IP acima.");

  atualizarSensores();
  classificarLuminosidade();
  atualizarStatus();

  enviarParaSupabase();

  ultimaLeituraSensores = millis();
  ultimaExibicaoSerial = millis();
  ultimoEnvioSupabase = millis();
}

// ======================================================
// LOOP PRINCIPAL
// ======================================================

void loop() {
  servidor.handleClient();

  unsigned long agora = millis();

  // Atualiza os sensores a cada 2 segundos
  if (agora - ultimaLeituraSensores >= INTERVALO_SENSORES) {
    ultimaLeituraSensores = agora;

    atualizarSensores();
    classificarLuminosidade();
    atualizarStatus();
  }

  // Detecta o pressionamento do botão
  bool botaoAtual = digitalRead(BOTAO_PIN);

  if (botaoAnterior == HIGH && botaoAtual == LOW) {
    if (
      statusAmbiente == "ATENCAO" ||
      statusAmbiente == "ALERTA"
    ) {
      alertaReconhecido = true;

      Serial.println(
        "Alerta reconhecido pelo usuário."
      );
    }
  }

  botaoAnterior = botaoAtual;

  // Exibe os dados no Monitor Serial
  if (agora - ultimaExibicaoSerial >= INTERVALO_SERIAL) {
    ultimaExibicaoSerial = agora;

    Serial.print("Temperatura: ");
    Serial.print(temperatura);
    Serial.print(" °C | Umidade: ");
    Serial.print(umidade);
    Serial.print(" % | Luminosidade: ");
    Serial.print(luminosidade);
    Serial.print(" | Nível de luz: ");
    Serial.print(nivelLuz);
    Serial.print(" | Status: ");
    Serial.print(statusAmbiente);
    Serial.print(" | Reconhecido: ");

    if (alertaReconhecido) {
      Serial.println("SIM");
    } else {
      Serial.println("NÃO");
    }
  }

  // Envia dados para o Supabase a cada 30 segundos
  if (agora - ultimoEnvioSupabase >= INTERVALO_SUPABASE) {
    ultimoEnvioSupabase = agora;

    enviarParaSupabase();
  }

  delay(10);
}
