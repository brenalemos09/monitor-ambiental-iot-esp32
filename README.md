# 🌿 Monitor Ambiental IoT com ESP32

> Sistema inteligente de monitoramento ambiental desenvolvido com ESP32, sensores, Wi-Fi, API REST, Supabase e Dashboard Web em tempo real.

![ESP32](https://img.shields.io/badge/ESP32-IoT-blue)
![Arduino](https://img.shields.io/badge/Arduino-C++-00979D)
![Supabase](https://img.shields.io/badge/Supabase-PostgreSQL-3ECF8E)
![REST API](https://img.shields.io/badge/API-REST-orange)
![Status](https://img.shields.io/badge/Status-Concluído-success)

---
DHT sensor library — Adafruit
Adafruit Unified Sensor
WiFi — incluída no pacote ESP32
WebServer — incluída no pacote ESP32
HTTPClient — incluída no pacote ESP32
WiFiClientSecure — incluída no pacote ESP32

# 📖 Sobre o projeto

Este projeto consiste em um sistema completo de monitoramento ambiental utilizando Internet das Coisas (IoT). O ESP32 realiza a leitura contínua de sensores de temperatura, umidade e luminosidade, classifica automaticamente o estado do ambiente, controla indicadores visuais através de LEDs, disponibiliza uma API REST local, exibe um dashboard web em tempo real e envia periodicamente todas as leituras para um banco de dados PostgreSQL hospedado no Supabase.

O objetivo foi desenvolver uma solução próxima de um cenário real de IoT, integrando hardware, firmware, comunicação em rede, APIs REST, banco de dados em nuvem e interface web.

---

# 🚀 Funcionalidades

- ✅ Leitura de temperatura e umidade (DHT11)
- ✅ Leitura de luminosidade (LDR)
- ✅ Classificação automática do ambiente
- ✅ Controle de LEDs por nível de alerta
- ✅ Reconhecimento manual do alerta através de botão
- ✅ Dashboard Web em tempo real
- ✅ API REST local
- ✅ Comunicação Wi-Fi
- ✅ Integração com Supabase
- ✅ Banco de dados PostgreSQL
- ✅ Histórico das leituras na nuvem

---

# 🛠 Tecnologias utilizadas

## Hardware

- ESP32 DevKit
- Sensor DHT11
- Sensor LDR
- LEDs
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
- REST API
- HTTP
- Wi-Fi
- PostgreSQL
- Supabase

---

# 🏗 Arquitetura do sistema

```text
                   DHT11
                      │
                      ▼
                 Temperatura
                 Umidade
                      │

LDR ───────────────► ESP32 ◄──────────── Botão
                      │
                      │
                 Controle LEDs
                      │
                      ▼
              Dashboard Local
                      │
                      ▼
                 API REST Local
                      │
                  Wi-Fi
                      │
                      ▼
          Supabase REST API
                      │
                      ▼
          PostgreSQL Database
                      │
                      ▼
            Histórico das Leituras
```

---

# 🌡 Classificação do ambiente

## Status NORMAL

- Temperatura ≤ 28°C
- Umidade ≤ 75%
- LED Verde ligado

---

## Status ATENÇÃO

- Temperatura > 28°C
- ou
- Umidade > 75%

LED Azul ligado.

---

## Status ALERTA

- Temperatura > 30°C
- ou
- Umidade > 85%

LED Vermelho ligado.

---

# ☀ Classificação da luminosidade

| Faixa | Classificação |
|--------|---------------|
| ≤ 2300 | Muito Claro |
| 2301–3300 | Normal |
| >3300 | Escuro |

---

# 🌐 API REST

## Página principal

```
GET /
```

Retorna o dashboard web.

---

## Dados dos sensores

```
GET /dados
```

Exemplo:

```json
{
    "temperatura":28.8,
    "umidade":67.2,
    "luminosidade":2869,
    "nivel_luz":"NORMAL",
    "status":"ATENCAO",
    "alerta_reconhecido":false
}
```

---

# ☁ Banco de Dados

Os dados são enviados automaticamente para o Supabase utilizando HTTPS.

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

# 📊 Dashboard

O dashboard apresenta em tempo real:

- Temperatura
- Umidade
- Luminosidade
- Nível de luz
- Status ambiental
- Alerta reconhecido

Atualização automática a cada 2 segundos.

---

# 📷 Demonstração

## Dashboard

> *(Adicionar imagem)*

```
images/dashboard.png
```

---

## Circuito

> *(Adicionar foto da montagem)*

```
images/circuito.jpg
```

---

## ESP32

> *(Adicionar foto da placa)*

```
images/esp32.jpg
```

---

# 📂 Estrutura do projeto

```
monitor-ambiental-iot-esp32

│

├── codigo/
│      monitor_ambiental.ino
│
├── images/
│      dashboard.png
│      circuito.jpg
│      esp32.jpg
│
├── docs/
│      arquitetura.png
│
├── README.md
│
└── LICENSE
```

---

# 💡 Aprendizados

Durante o desenvolvimento deste projeto foram aplicados conceitos de:

- Sistemas Embarcados
- Internet das Coisas
- Comunicação HTTP
- REST API
- JSON
- Wi-Fi com ESP32
- Banco de Dados PostgreSQL
- Supabase
- Segurança com Row Level Security (RLS)
- Desenvolvimento Web
- Integração Hardware + Software

---

# 🔒 Segurança

As credenciais do Wi-Fi e da API foram removidas da versão pública do projeto.

Para utilizar este projeto, configure:

```cpp
const char* WIFI_NOME;
const char* WIFI_SENHA;

const char* SUPABASE_URL;
const char* SUPABASE_KEY;
```

---

# 👩‍💻 Autora

**Brena Vitória Lemos**

Estudante de Ciência da Computação — Universidade de Fortaleza (UNIFOR)

GitHub:
https://github.com/brenalemos09

LinkedIn:
https://www.linkedin.com/in/brenavitorialemos

---

# ⭐ Considerações finais

Este projeto foi desenvolvido como parte de um desafio técnico voltado para Internet das Coisas (IoT), buscando demonstrar conhecimentos em programação embarcada, integração com serviços em nuvem, desenvolvimento de APIs REST, armazenamento de dados e construção de interfaces web para monitoramento em tempo real.

Além da implementação funcional, o foco também esteve na organização do código, documentação e adoção de boas práticas de desenvolvimento de software.
