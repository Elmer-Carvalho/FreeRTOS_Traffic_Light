# SmartLight: Sistema de Semáforo Inteligente

## 📘 Introdução

O **SmarLight** é um sistema embarcado implementado no microcontrolador **RP2040** da plataforma **BitDogLab**, simulando um semáforo de trânsito inteligente com dois modos de operação: **Normal** e **Noturno**.

O projeto utiliza **FreeRTOS** para gerenciar tarefas concorrentes, integrando periféricos como **LED RGB**, **matriz de LEDs WS2812B 5x5**, **display OLED SSD1306**, **dois buzzers** e **um botão**.

Ele exibe os estados de semáforo (verde, amarelo, vermelho) com feedback visual e sonoro, alternando modos via botão. Ideal para aprendizado em sistemas embarcados, FreeRTOS e controle de periféricos, o SmartLight também serve como protótipo para sinalização de trânsito inteligente.

---

## 📁 Estrutura do Projeto

```
Smart_TrafficLight/
├── smart_traffic_light.c       # Código principal do projeto
├── CMakeLists.txt              # Configuração de build com Pico SDK
├── lib/                        # Bibliotecas auxiliares (display, fontes)
└── lib/pio_matri.pio          # Código PIO para controle da matriz de LEDs
```

---

## 🧩 Epecificações do Projeto

### 🔌 Periféricos Utilizados

- **LED RGB**: Exibe cores do semáforo (verd, amarelo, vermelho) nos pinos **13 (vermelho)**, **11 (verde)** e **12 (azul)**.
- **Matriz de LEDs WS2812B 5x5**: Mostra cores sincronizadas com o LED RGB, controlada via pino **7**.
- **Display OLED SSD1306 128x64**: Exibe o modo e o estado atual via **I2C**.
- **Buzzers**: Dois buzzers (**pinos 21 e 10**) emitem beeps sincronizados.
- **Botão**: Alterna modos via interrupção no **pino 5**, com debounce de 400 ms.

### 🔧 Recursos do MCU RP2040)

- **GPIO**: Controle de LED RGB e botão.
- **PIO**: Controle da matriz WS2812B.
- **I2C**: Comunicação com o display OLED.
- **PWM**: Geração de tons nos buzzers.
- **FreeRTOS**: Três tarefas concorrentes (semáforo, buzzers, display).

---

## 📦 Materiais Necessários

- Plac **BitDogLab** com **RP2040**
- Cabo **micro-USB para USB-A**
- **Protoboard** e fios **jumper**
- **LED RGB** (cátodo comum)
- **Matriz de LEDs WS2812B 5x5**
- **Display OLED SSD1306 (I2C)**
- **Dois buzzers passivos**
- **Botão** push-button
- **Resistores** (220–330 Ω, se necessário)

---

## 💻 Softwares Utilizados

 **Visual Studio Code** (recomendado)
- **Pico SDK** (versão 2.1.1 ou superior)
- **FreeRTOS-Kernel** (integrado ao projeto)
- **ARM GCC** (compilador C)
- **CMake**
- **Minicom** ou similar (opcional)

---

## ⚙️ Como Utilizar

### 🛠️ Configurar o Hardware

| Coponente          | Pino no RP2040 |
|---------------------|----------------|
| LED RGB (Vermelho)  | GP13           |
| LED RGB (Verde)     | GP11           |
| LED RGB (Azul)      | GP12           |
| Matriz WS2812B      | GP7            |
| Display OLED (SDA)  | GP14           |
| Display OLED (SCL)  | GP15           |
| Buzzer A            | GP21           |
| Buzzer B            | GP10           |
| Botão               | GP5            |

#### Conexões:

- Conecte o **LED RGB** aos pinos **13, 11 e 12** (usar resistores de 220–330 Ω se necessário).
- Ligue a **matriz WS2812B** ao pino **7** (pode precisar de alimentação externa).
- Conecte o **display OLED** aos pinos **14 (SDA)** e **15 (SCL)**.
- Ligue os **buzzers** aos pinos **21** e **10**.
- Conecte o **botão** ao pino **5** com pull-up interno.

---

### ▶️ Operação

1. Conecte a BitDogLab ao computador via **USB**.
2. Carregue o firmware (`smart_traffic_light.uf2`) na placa.
3. O sistema inicia no **Modo Normal**, exibindo o ciclo de semáforo:

#### 🔄 Ciclo no Mod Normal:

- **Verde (3 s)**:
  - LED RGB e matriz verdes.
  - Dois beeps de 1 s (com pausa de 200 ms), seguido de beep de 600 ms.
- **Amarelo (3 s)**:
  - LED RGB e matriz amarelos.
  - Sete beeps de 350 ms (pausa de 50 ms), seguido de beep de 200 ms.
- **Vermelho (3 s)**:
  - LED RGB e matriz vermelhos.
  - Beep de 500 ms → pausa de 1500 ms → beep de 500 ms → pausa de 500 ms.

#### 🌙 Mod Noturno:

- Ativado ao **pressionar o botão**.
- LED amarelo **pisca** (1 s ligado, 1 s desligado).
- Beep curto de **100 ms** a cada 2 segundos.
- Display mostra o modo e o estado atual.

---

## 🔎 Indiadores

| Indicador     | Função                                   |
|---------------|-------------------------------------------|
| LED RGB       | Exibe as cores do semáforo               |
| Matriz de LEDs| Sincroniza cores com o LED RGB           |
| Display OLED  | Mostra o modo e o estado do semáforo     |
| Buzzers       | Emitir beeps específicos por estado      |
| Saída Serial  | Debug com informações do sistema         |

---

## ⚠️ Limitações

- **Temporizações fixas**: Ciclo de 3 s por estado não é configurável.
- **Matriz de LEDs**: Exibe apenas cores sólidas.
- **Buzzers**: Limitados por PWM, sem variação de tom.
- **Modo Noturno**: Não há detecção automática de luminosidade.

---

## 🚀 Melhorias Futuras

- Adicionar **contagem regressiva** na matriz de LEDs.
- Incluir **segundo botã** para reinício ou BOOTSEL.
- Adicionar **sensor de luminosidade** para Modo Noturno automático.
- Permitir **configuração de tempos** via botões ou serial.
- Adicionar **variações de tom** nos buzzers para estados diferentes.

---

## 👨‍💻 Autor

Desenvolvido por **Elmer Carvalho**

---

## 📄 Licença

Este projeto está licenciado sob a **licença MIT**. Consulte o arquivo `LICENSE` para mais informações.
