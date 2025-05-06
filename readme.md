# 📏 Ohmímetro com Raspberry Pi Pico

Este projeto implementa um **ohmímetro** em um microcontrolador **Raspberry Pi Pico**, utilizando:

- **Display OLED SSD1306** para exibir o valor da resistência e as cores das faixas.
- **Matriz de LEDs WS2812B 5x5**, inicializada e limpa ao ligar.
- **Conversor ADC** para medir a tensão em um circuito divisor de tensão.

O sistema mede resistências na faixa de **510 Ω a 100 kΩ**, calcula o valor mais próximo da série **E24**, determina as cores das faixas (primeira, segunda e multiplicador) e exibe as informações no display.  
Ideal para aprendizado em **sistemas embarcados**, **eletrônica** e **desenvolvimento com o Pico SDK**.

---

## 📁 Estrutura do Projeto

```
Ohmimetro/
├── ohmimetro.c         # Código principal do projeto
├── CMakeLists.txt      # Configuração de build com Pico SDK
├── lib/                # Bibliotecas auxiliares (display, fontes, etc.)
└── lib/pio_matrix.pio  # Código PIO para controle da matriz de LEDs
```

---

## 📋 Especificações do Projeto

### Periféricos Utilizados
- **Display OLED SSD1306 128x64**: Exibe resistência, cores das faixas e mensagens de erro.
- **Matriz de LEDs WS2812B 5x5**: Inicializada e apagada ao ligar.
- **Resistor Conhecido (10 kΩ)**: Parte do divisor de tensão.
- **ADC (GP28)**: Lê a tensão para calcular a resistência, com média de 100 leituras.

### Recursos do MCU (RP2040)
- **PIO**: Controle da matriz de LEDs WS2812B.
- **I2C**: Comunicação com o display OLED.
- **ADC**: Conversão analógico-digital da tensão lida.
- **GPIO**: Gerenciamento dos pinos para I2C e ADC.

---

## 🔌 Materiais Necessários
- Raspberry Pi Pico ou Pico W
- Cabo micro-USB para USB-A
- Protoboard e fios jumper
- Resistor conhecido de 10 kΩ
- Display OLED SSD1306 (I2C)
- Matriz de LEDs WS2812B 5x5
- Resistores para teste (510 Ω a 100 kΩ)

---

## 🛠️ Softwares Utilizados
- Visual Studio Code (recomendado)
- Pico SDK
- ARM GCC (compilador C)
- CMake
- Minicom ou similar (opcional, para monitoramento via serial)

---

## ⚙️ Como Utilizar

### 1. Configurar o Hardware

| Componente         | Pino no Pico |
|---------------------|--------------|
| Matriz WS2812B       | GP7          |
| Display OLED (SDA)   | GP14         |
| Display OLED (SCL)   | GP15         |
| ADC (Resistor)       | GP28 (ADC2)  |

**Circuito Divisor de Tensão:**
- Conecte um resistor conhecido de 10 kΩ entre 3.3V e o pino GP28.
- Conecte o resistor desconhecido entre GP28 e GND.
- Utilize uma protoboard para organizar as conexões.

---

### 2. Operação

- Conecte o Pico ao computador via USB.
- Insira um resistor no circuito divisor de tensão.
- O display OLED mostrará:
  - Nomes das cores das faixas (e.g., "1: Amarelo", "2: Violeta", "Multi.: Vermelho").
  - Valor da resistência calculada.
  - Mensagem "**Fora de faixa**" se o resistor estiver fora do intervalo suportado.

A matriz de LEDs é inicializada mas permanece apagada.


---

## 🔎 Indicadores

| Indicador         | Função                                    |
|-------------------|-------------------------------------------|
| Display OLED      | Exibe cores, resistência e mensagens de erro |
| Matriz de LEDs    | Inicializada e apagada ao ligar           |
| Saída Serial      | Debug com resistência e cores             |

---

## ⚠️ Limitações

- **Precisão reduzida em resistências altas**: acima de 47 kΩ, a precisão da leitura diminui devido à aproximação da tensão de 3.3V.
- **Faixa limitada**: medição entre **510 Ω a 100 kΩ** apenas.
- **Matriz de LEDs**: atualmente não exibe cores nem padrões.

---

## 🚀 Melhorias Futuras
- Implementar exibição das cores nas faixas da matriz de LEDs.
- Representar graficamente o resistor no display OLED.
- Melhorar a precisão em resistências altas (utilizar resistor conhecido maior ou circuito de seleção).
- Adicionar botão para recalibrar ou reiniciar a medição.

---

## 👤 Autor
**Desenvolvido por [Elmer Carvalho](https://github.com/Elmer-Carvalho)**

---

## 📝 Licença
Este projeto está licenciado sob a licença **MIT**.  
Consulte o arquivo [LICENSE](LICENSE) para mais informações.
