# 🌿 Monitor Ambiental IoT com ESP32

> Sistema inteligente de monitoramento ambiental desenvolvido com ESP32, sensores, Dashboard Web, API REST e integração com Supabase.

![ESP32](https://img.shields.io/badge/ESP32-IoT-blue)
![Arduino](https://img.shields.io/badge/Arduino-C++-00979D)
![Supabase](https://img.shields.io/badge/Supabase-PostgreSQL-3ECF8E)
![REST API](https://img.shields.io/badge/API-REST-orange)
![Status](https://img.shields.io/badge/Status-Concluído-success)

---

# 📖 Sobre o projeto

Este projeto consiste em um sistema completo de monitoramento ambiental utilizando Internet das Coisas (IoT). O ESP32 realiza a leitura contínua de sensores de temperatura, umidade e luminosidade, classifica automaticamente o estado do ambiente, controla indicadores visuais por meio de LEDs, disponibiliza uma API REST local, exibe um Dashboard Web em tempo real e envia periodicamente todas as leituras para um banco de dados PostgreSQL hospedado no Supabase.

O objetivo foi desenvolver uma solução próxima de um cenário real de IoT, integrando hardware, firmware, comunicação em rede, APIs REST, banco de dados em nuvem e interface web.

---

# 🚀 Funcionalidades

- ✅ Leitura de temperatura e umidade (DHT11)
- ✅ Leitura de luminosidade (LDR)
- ✅ Classificação automática do ambiente
- ✅ Controle de LEDs conforme o estado do ambiente
- ✅ Reconhecimento manual de condições de Atenção e Alerta através de botão físico
- ✅ Dashboard Web em tempo real
- ✅ API REST local
- ✅ Comunicação Wi-Fi
- ✅ Integração com Supabase
- ✅ Banco de dados PostgreSQL
- ✅ Histórico das leituras em nuvem

---

# 🛠 Tecnologias utilizadas

## Hardware

- ESP32 DevKit
- Sensor DHT11
- Sensor LDR
- LED Verde
- LED Azul
- LED Vermelho
- Botão
- Protoboard
- Resistores
- Jumpers

## Software

- Arduino IDE
- C++
- HTML
- CSS
- JavaScript
- JSON
- HTTP
- HTTPS
- REST API
- Wi-Fi
- PostgreSQL
- Supabase

---

# 📚 Bibliotecas utilizadas

- DHT Sensor Library — Adafruit
- Adafruit Unified Sensor
- WiFi (ESP32)
- WebServer (ESP32)
- HTTPClient (ESP32)
- WiFiClientSecure (ESP32)

---

# 🏗 Arquitetura do sistema

```text
                   DHT11
                      │
                      ▼
            Temperatura / Umidade
                      │

LDR ───────────────► ESP32 ◄──────────── Botão
                      │
                      │
                Controle dos LEDs
                      │
                      ▼
                Dashboard Web
                      │
                      ▼
              API REST (/dados)
                      │
                   Wi-Fi
                      │
                      ▼
              Supabase REST API
                      │
                      ▼
                PostgreSQL
                      │
                      ▼
          Histórico das Leituras
```

---

# 🌡 Classificação do ambiente

## 🟢 Normal

- Temperatura ≤ 28 °C
- Umidade ≤ 75%
- LED Verde ligado

---

## 🔵 Atenção

- Temperatura > 28 °C
- ou
- Umidade > 75%

LED Azul ligado.

---

## 🔴 Alerta

- Temperatura > 30 °C
- ou
- Umidade > 85%

LED Vermelho ligado.

---

# ☀ Classificação da luminosidade

| Faixa | Classificação |
|--------|---------------|
| ≤ 2300 | Muito Claro |
| 2301 – 3300 | Normal |
| > 3300 | Escuro |

---

# 🌐 API REST

## Dashboard

```http
GET /
```

Retorna o Dashboard Web.

---

## Dados em tempo real

```http
GET /dados
```

Exemplo de resposta:

```json
{
  "temperatura": 28.8,
  "umidade": 67.2,
  "luminosidade": 2869,
  "nivel_luz": "NORMAL",
  "status": "ATENCAO",
  "alerta_reconhecido": false
}
```

---

# ☁ Banco de dados

As leituras são enviadas automaticamente para o Supabase utilizando comunicação HTTPS.

Tabela:

```
leituras_ambientais
```

Campos armazenados:

- id
- criado_em
- temperatura
- umidade
- luminosidade
- nivel_luz
- status
- alerta_reconhecido

---

# 📊 Dashboard Web

O Dashboard apresenta em tempo real:

- Temperatura
- Umidade
- Luminosidade
- Nível de luz
- Status ambiental
- Reconhecimento da condição

Atualização automática a cada 2 segundos.

---

# ▶ Como executar

1. Instale a Arduino IDE.
2. Instale o suporte para placas ESP32.
3. Instale as bibliotecas utilizadas.
4. Crie um arquivo `secrets.h` contendo as credenciais do Wi-Fi e do Supabase.
5. Compile o projeto.
6. Faça o upload para o ESP32.
7. Abra o Monitor Serial para visualizar o endereço IP.
8. Acesse o Dashboard pelo navegador utilizando o IP exibido.

---

# 📂 Estrutura do projeto

```text
monitor-ambiental-iot

│
├── firmware/
│   └── monitor_ambiental/
│       ├── monitor_ambiental.ino
│       └── secrets.example.h
│
├── docs/
│   └── Monitor_Ambiental_IoT.pdf
│
├── images/
│   ├── circuito.jpg
│   └── dashboard.png
│
├── .gitignore
├── LICENSE
└── README.md
```

---

# 💡 Aprendizados

Durante o desenvolvimento deste projeto foram aplicados conceitos de:

- Sistemas Embarcados
- Internet das Coisas (IoT)
- Comunicação HTTP e HTTPS
- APIs REST
- JSON
- Wi-Fi com ESP32
- Banco de Dados PostgreSQL
- Supabase
- Row Level Security (RLS)
- Desenvolvimento Web
- Integração entre hardware e software

---

# ✅ Resultado

O projeto foi capaz de:

- Monitorar temperatura, umidade e luminosidade em tempo real.
- Classificar automaticamente as condições ambientais.
- Exibir as informações em um Dashboard Web local.
- Disponibilizar uma API REST em formato JSON.
- Armazenar periodicamente as leituras em um banco PostgreSQL no Supabase.

---

# 🔒 Segurança

As credenciais do Wi-Fi e do Supabase foram removidas da versão pública deste repositório.

Para executar o projeto, crie um arquivo `secrets.h` contendo:

```cpp
const char* WIFI_NOME;
const char* WIFI_SENHA;

const char* SUPABASE_URL;
const char* SUPABASE_KEY;
```

---

# 👩‍💻 Autora

**Brena Vitória Aguiar Lemos**

Estudante de Ciência da Computação — Universidade de Fortaleza (UNIFOR)

**GitHub**

https://github.com/brenalemos09

**LinkedIn**

https://www.linkedin.com/in/brenavitorialemos

---

# ⭐ Considerações finais

Este projeto foi desenvolvido como parte de um desafio técnico voltado à área de Internet das Coisas (IoT), buscando demonstrar conhecimentos em programação embarcada, integração com serviços em nuvem, desenvolvimento de APIs REST, armazenamento de dados e construção de interfaces web para monitoramento em tempo real.

Além da implementação funcional, também houve preocupação com a organização do código, documentação técnica e adoção de boas práticas de desenvolvimento de software.
