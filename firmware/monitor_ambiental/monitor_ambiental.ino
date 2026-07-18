#include <WiFi.h>
#include <WebServer.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <DHT.h>

#include "secrets.h"

// =====================================================
// CONFIGURAÇÃO DOS PINOS
// =====================================================

#define PINO_DHT 4
#define TIPO_DHT DHT11

#define PINO_LDR 34

#define LED_VERDE 18
#define LED_AZUL 19
#define LED_VERMELHO 21

#define PINO_BOTAO 23

// =====================================================
// INTERVALOS DE EXECUÇÃO
// =====================================================

// Atualização dos sensores e do estado local.
const unsigned long INTERVALO_LEITURA = 2000;

// Envio das leituras ao Supabase.
const unsigned long INTERVALO_SUPABASE = 30000;

// =====================================================
// OBJETOS DO SISTEMA
// =====================================================

DHT dht(PINO_DHT, TIPO_DHT);
WebServer servidor(80);

// =====================================================
// VARIÁVEIS DOS SENSORES
// =====================================================

float temperatura = 0.0;
float umidade = 0.0;
int luminosidade = 0;

String nivelLuz = "SEM LEITURA";
String statusAmbiente = "INICIALIZANDO";

bool alertaReconhecido = false;

// Controle do botão.
bool estadoAnteriorBotao = HIGH;
unsigned long ultimoDebounce = 0;
const unsigned long TEMPO_DEBOUNCE = 50;

// Controle de tempo.
unsigned long ultimaLeitura = 0;
unsigned long ultimoEnvioSupabase = 0;

// =====================================================
// CLASSIFICAÇÃO DA LUMINOSIDADE
// =====================================================

String classificarLuminosidade(int valorLdr) {
  if (valorLdr <= 2300) {
    return "MUITO CLARO";
  }

  if (valorLdr <= 3300) {
    return "NORMAL";
  }

  return "ESCURO";
}

// =====================================================
// CLASSIFICAÇÃO DO AMBIENTE
// =====================================================

String classificarAmbiente(float temp, float umid) {
  // A condição de alerta deve ser verificada primeiro.
  if (temp > 30.0 || umid > 85.0) {
    return "ALERTA";
  }

  if (temp > 28.0 || umid > 75.0) {
    return "ATENCAO";
  }

  return "NORMAL";
}

// =====================================================
// CONTROLE DOS LEDS
// =====================================================

void atualizarLeds() {
  digitalWrite(LED_VERDE, LOW);
  digitalWrite(LED_AZUL, LOW);
  digitalWrite(LED_VERMELHO, LOW);

  if (statusAmbiente == "NORMAL") {
    digitalWrite(LED_VERDE, HIGH);
  } else if (statusAmbiente == "ATENCAO") {
    digitalWrite(LED_AZUL, HIGH);
  } else if (statusAmbiente == "ALERTA") {
    digitalWrite(LED_VERMELHO, HIGH);
  }
}

// =====================================================
// LEITURA DOS SENSORES
// =====================================================

void atualizarSensores() {
  float novaTemperatura = dht.readTemperature();
  float novaUmidade = dht.readHumidity();

  if (isnan(novaTemperatura) || isnan(novaUmidade)) {
    Serial.println("Falha ao realizar leitura do DHT11.");
    return;
  }

  String statusAnterior = statusAmbiente;

  temperatura = novaTemperatura;
  umidade = novaUmidade;
  luminosidade = analogRead(PINO_LDR);

  nivelLuz = classificarLuminosidade(luminosidade);
  statusAmbiente = classificarAmbiente(temperatura, umidade);

  /*
    Quando o ambiente retorna ao estado normal,
    o reconhecimento anterior é apagado.

    Também consideramos uma mudança entre ATENCAO e ALERTA
    como uma nova condição que ainda precisa ser reconhecida.
  */
  if (
    statusAmbiente == "NORMAL" ||
    statusAmbiente != statusAnterior
  ) {
    alertaReconhecido = false;
  }

  atualizarLeds();

  Serial.println("----------------------------------");
  Serial.print("Temperatura: ");
  Serial.print(temperatura, 1);
  Serial.println(" °C");

  Serial.print("Umidade: ");
  Serial.print(umidade, 1);
  Serial.println(" %");

  Serial.print("Luminosidade: ");
  Serial.println(luminosidade);

  Serial.print("Nivel de luz: ");
  Serial.println(nivelLuz);

  Serial.print("Status: ");
  Serial.println(statusAmbiente);

  Serial.print("Alerta reconhecido: ");
  Serial.println(alertaReconhecido ? "SIM" : "NAO");
}

// =====================================================
// LEITURA DO BOTÃO
// =====================================================

void verificarBotao() {
  bool estadoAtualBotao = digitalRead(PINO_BOTAO);

  if (estadoAtualBotao != estadoAnteriorBotao) {
    ultimoDebounce = millis();
  }

  if (millis() - ultimoDebounce > TEMPO_DEBOUNCE) {
    if (
      estadoAnteriorBotao == HIGH &&
      estadoAtualBotao == LOW &&
      (statusAmbiente == "ATENCAO" || statusAmbiente == "ALERTA")
    ) {
      alertaReconhecido = true;

      Serial.println("Condicao reconhecida pelo usuario.");
    }
  }

  estadoAnteriorBotao = estadoAtualBotao;
}

// =====================================================
// CRIAÇÃO DO JSON
// =====================================================

String criarJson() {
  String json = "{";

  json += "\"temperatura\":";
  json += String(temperatura, 1);
  json += ",";

  json += "\"umidade\":";
  json += String(umidade, 1);
  json += ",";

  json += "\"luminosidade\":";
  json += String(luminosidade);
  json += ",";

  json += "\"nivel_luz\":\"";
  json += nivelLuz;
  json += "\",";

  json += "\"status\":\"";
  json += statusAmbiente;
  json += "\",";

  json += "\"alerta_reconhecido\":";
  json += alertaReconhecido ? "true" : "false";

  json += "}";

  return json;
}

// =====================================================
// DASHBOARD WEB
// =====================================================

String criarPaginaDashboard() {
  return R"rawliteral(
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
    * {
      box-sizing: border-box;
    }

    body {
      margin: 0;
      padding: 24px;
      font-family: Arial, Helvetica, sans-serif;
      background: #f4f7f9;
      color: #1f2937;
    }

    .container {
      width: 100%;
      max-width: 900px;
      margin: 0 auto;
    }

    header {
      text-align: center;
      margin-bottom: 28px;
    }

    h1 {
      margin-bottom: 8px;
    }

    .subtitulo {
      color: #64748b;
    }

    .grade {
      display: grid;
      grid-template-columns: repeat(auto-fit, minmax(220px, 1fr));
      gap: 16px;
    }

    .card {
      background: white;
      border-radius: 14px;
      padding: 22px;
      box-shadow: 0 4px 16px rgba(0, 0, 0, 0.08);
    }

    .card h2 {
      margin-top: 0;
      margin-bottom: 12px;
      font-size: 16px;
      color: #64748b;
    }

    .valor {
      font-size: 30px;
      font-weight: bold;
    }

    .status {
      margin-top: 18px;
      padding: 20px;
      border-radius: 14px;
      text-align: center;
      font-size: 24px;
      font-weight: bold;
      background: white;
      box-shadow: 0 4px 16px rgba(0, 0, 0, 0.08);
    }

    .normal {
      border-left: 8px solid #22c55e;
    }

    .atencao {
      border-left: 8px solid #3b82f6;
    }

    .alerta {
      border-left: 8px solid #ef4444;
    }

    .rodape {
      margin-top: 24px;
      text-align: center;
      color: #64748b;
      font-size: 14px;
    }

    .erro {
      color: #ef4444;
    }
  </style>
</head>

<body>
  <main class="container">
    <header>
      <h1>Monitor Ambiental IoT</h1>

      <div class="subtitulo">
        ESP32, sensores e integração com Supabase
      </div>
    </header>

    <section class="grade">
      <article class="card">
        <h2>Temperatura</h2>
        <div class="valor" id="temperatura">--</div>
      </article>

      <article class="card">
        <h2>Umidade</h2>
        <div class="valor" id="umidade">--</div>
      </article>

      <article class="card">
        <h2>Luminosidade</h2>
        <div class="valor" id="luminosidade">--</div>
      </article>

      <article class="card">
        <h2>Nível de luz</h2>
        <div class="valor" id="nivelLuz">--</div>
      </article>

      <article class="card">
        <h2>Reconhecimento</h2>
        <div class="valor" id="reconhecimento">--</div>
      </article>
    </section>

    <section class="status" id="cartaoStatus">
      Status: <span id="status">CARREGANDO</span>
    </section>

    <div class="rodape" id="mensagem">
      Atualização automática a cada 2 segundos
    </div>
  </main>

  <script>
    async function atualizarDashboard() {
      const mensagem = document.getElementById("mensagem");

      try {
        const resposta = await fetch("/dados");

        if (!resposta.ok) {
          throw new Error("Não foi possível consultar a API.");
        }

        const dados = await resposta.json();

        document.getElementById("temperatura").textContent =
          dados.temperatura.toFixed(1) + " °C";

        document.getElementById("umidade").textContent =
          dados.umidade.toFixed(1) + " %";

        document.getElementById("luminosidade").textContent =
          dados.luminosidade;

        document.getElementById("nivelLuz").textContent =
          dados.nivel_luz;

        document.getElementById("status").textContent =
          dados.status;

        document.getElementById("reconhecimento").textContent =
          dados.alerta_reconhecido ? "SIM" : "NÃO";

        const cartaoStatus =
          document.getElementById("cartaoStatus");

        cartaoStatus.classList.remove(
          "normal",
          "atencao",
          "alerta"
        );

        if (dados.status === "NORMAL") {
          cartaoStatus.classList.add("normal");
        } else if (dados.status === "ATENCAO") {
          cartaoStatus.classList.add("atencao");
        } else if (dados.status === "ALERTA") {
          cartaoStatus.classList.add("alerta");
        }

        mensagem.textContent =
          "Atualização automática a cada 2 segundos";

        mensagem.classList.remove("erro");
      } catch (erro) {
        mensagem.textContent =
          "Erro ao atualizar os dados do sistema.";

        mensagem.classList.add("erro");

        console.error(erro);
      }
    }

    atualizarDashboard();

    setInterval(atualizarDashboard, 2000);
  </script>
</body>
</html>
)rawliteral";
}

// =====================================================
// ROTAS DO SERVIDOR WEB
// =====================================================

void configurarRotas() {
  servidor.on("/", HTTP_GET, []() {
    servidor.send(
      200,
      "text/html; charset=utf-8",
      criarPaginaDashboard()
    );
  });

  servidor.on("/dados", HTTP_GET, []() {
    servidor.send(
      200,
      "application/json; charset=utf-8",
      criarJson()
    );
  });

  servidor.onNotFound([]() {
    servidor.send(
      404,
      "application/json",
      "{\"erro\":\"Rota nao encontrada\"}"
    );
  });
}

// =====================================================
// CONEXÃO WI-FI
// =====================================================

void conectarWiFi() {
  Serial.print("Conectando ao Wi-Fi");

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_NOME, WIFI_SENHA);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("Wi-Fi conectado.");

  Serial.print("Endereco do Dashboard: http://");
  Serial.println(WiFi.localIP());

  Serial.print("Endpoint JSON: http://");
  Serial.print(WiFi.localIP());
  Serial.println("/dados");
}

// =====================================================
// ENVIO PARA O SUPABASE
// =====================================================

void enviarParaSupabase() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println(
      "Envio cancelado: ESP32 sem conexao Wi-Fi."
    );

    return;
  }

  WiFiClientSecure clienteSeguro;

  /*
    Mantido para simplificar a demonstração acadêmica.
    Em uma aplicação de produção, recomenda-se validar
    corretamente o certificado TLS do servidor.
  */
  clienteSeguro.setInsecure();

  HTTPClient http;

  String endpoint =
    String(SUPABASE_URL) +
    "/rest/v1/leituras_ambientais";

  if (!http.begin(clienteSeguro, endpoint)) {
    Serial.println(
      "Nao foi possivel iniciar a conexao HTTPS."
    );

    return;
  }

  http.addHeader("Content-Type", "application/json");
  http.addHeader("apikey", SUPABASE_KEY);
  http.addHeader(
    "Authorization",
    "Bearer " + String(SUPABASE_KEY)
  );
  http.addHeader("Prefer", "return=minimal");

  String json = criarJson();

  Serial.println("Enviando leitura para o Supabase...");
  Serial.println(json);

  int codigoHttp = http.POST(json);

  Serial.print("Resposta HTTP: ");
  Serial.println(codigoHttp);

  if (codigoHttp == 200 || codigoHttp == 201) {
    Serial.println(
      "Leitura armazenada com sucesso."
    );
  } else {
    Serial.print("Erro retornado pelo Supabase: ");
    Serial.println(http.getString());
  }

  http.end();
}

// =====================================================
// CONFIGURAÇÃO INICIAL
// =====================================================

void setup() {
  Serial.begin(115200);

  pinMode(LED_VERDE, OUTPUT);
  pinMode(LED_AZUL, OUTPUT);
  pinMode(LED_VERMELHO, OUTPUT);

  pinMode(PINO_BOTAO, INPUT_PULLUP);
  pinMode(PINO_LDR, INPUT);

  digitalWrite(LED_VERDE, LOW);
  digitalWrite(LED_AZUL, LOW);
  digitalWrite(LED_VERMELHO, LOW);

  dht.begin();

  conectarWiFi();
  configurarRotas();

  servidor.begin();

  Serial.println("Servidor web iniciado.");

  // Primeira leitura logo após a inicialização.
  atualizarSensores();

  ultimaLeitura = millis();
  ultimoEnvioSupabase = millis();
}

// =====================================================
// LOOP PRINCIPAL
// =====================================================

void loop() {
  servidor.handleClient();
  verificarBotao();

  unsigned long agora = millis();

  if (agora - ultimaLeitura >= INTERVALO_LEITURA) {
    ultimaLeitura = agora;
    atualizarSensores();
  }

  if (
    agora - ultimoEnvioSupabase >=
    INTERVALO_SUPABASE
  ) {
    ultimoEnvioSupabase = agora;
    enviarParaSupabase();
  }
}
