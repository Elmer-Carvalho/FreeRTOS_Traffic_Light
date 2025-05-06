#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pico/stdlib.h"
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"
#include "hardware/gpio.h"
#include "hardware/pwm.h"
#include "hardware/i2c.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "./lib/ssd1306.h"
#include "./lib/font.h"
#include "pio_matrix.pio.h"

// Definições de pinos
#define I2C_PORT i2c1
#define I2C_SDA_PIN 14
#define I2C_SCL_PIN 15
#define SSD_ADDR 0x3C
#define SSD_WIDTH 128
#define SSD_HEIGHT 64
#define MATRIZ_LEDS_PIN 7
#define NUM_LEDS 25
#define LED_RED_PIN 13
#define LED_GREEN_PIN 11
#define LED_BLUE_PIN 12
#define BUZZER_A_PIN 21
#define BUZZER_B_PIN 10
#define BUTTON_A_PIN 5

// Estados do semáforo
typedef enum {
    GREEN,
    YELLOW,
    RED
} SemaphoreState;

// Variáveis globais
volatile bool is_night_mode = false;
volatile SemaphoreState semaphore_state = GREEN;
ssd1306_t ssd;
PIO pio = pio0;
uint sm;

// Protótipos
void setup();
void init_i2c_display();
void init_matrix_leds();
void init_led_rgb();
void init_buzzers();
void init_button();
void clear_matrix();
void set_matrix_color(uint8_t r, uint8_t g, uint8_t b);
void set_rgb_led(uint8_t r, uint8_t g, uint8_t b);
void buzzer_beep(uint32_t duration_ms);
void vSemaphoreTask(void *pvParameters);
void vBuzzersTask(void *pvParameters);
void button_irq_handler(uint gpio, uint32_t events);
void vDisplayTask(void *pvParameters);

// Função principal
int main() {
    stdio_init_all();
    setup();

    // Criar tarefas FreeRTOS
    xTaskCreate(vSemaphoreTask, "Semaphore Task", 256, NULL, 2, NULL);
    xTaskCreate(vBuzzersTask, "Buzzer Task", 256, NULL, 2, NULL);
    xTaskCreate(vDisplayTask, "Display Task", 256, NULL, 1, NULL);

    // Iniciar o scheduler
    vTaskStartScheduler();

    // Não deve chegar aqui
    while (true) {}
}

// Configuração inicial
void setup() {
    init_matrix_leds();
    init_i2c_display();
    init_led_rgb();
    init_buzzers();
    init_button();
}

// Inicializar display I2C
void init_i2c_display() {
    i2c_init(I2C_PORT, 400000);
    gpio_set_function(I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA_PIN);
    gpio_pull_up(I2C_SCL_PIN);
    ssd1306_init(&ssd, SSD_WIDTH, SSD_HEIGHT, false, SSD_ADDR, I2C_PORT);
    ssd1306_config(&ssd);
    ssd1306_fill(&ssd, false);
    ssd1306_send_data(&ssd);
}

// Inicializar matriz de LEDs
void init_matrix_leds() {
    sm = pio_claim_unused_sm(pio, true);
    uint offset = pio_add_program(pio, &pio_matrix_program);
    pio_matrix_program_init(pio, sm, offset, MATRIZ_LEDS_PIN);
    pio_sm_set_enabled(pio, sm, true);
    clear_matrix();
}

// Limpar matriz de LEDs
void clear_matrix() {
    for (uint i = 0; i < NUM_LEDS; i++) {
        pio_sm_put_blocking(pio, sm, 0);
    }
}

// Definir cor na matriz de LEDs (ordem GRB)
void set_matrix_color(uint8_t r, uint8_t g, uint8_t b) {
    for (uint i = 0; i < NUM_LEDS; i++) {
        uint32_t color = (g << 24) | (r << 16) | (b << 8);
        pio_sm_put_blocking(pio, sm, color);
    }
}

// Inicializar LED RGB
void init_led_rgb() {
    gpio_init(LED_RED_PIN);
    gpio_init(LED_GREEN_PIN);
    gpio_init(LED_BLUE_PIN);
    gpio_set_dir(LED_RED_PIN, GPIO_OUT);
    gpio_set_dir(LED_GREEN_PIN, GPIO_OUT);
    gpio_set_dir(LED_BLUE_PIN, GPIO_OUT);
    set_rgb_led(0, 0, 0); // Desligar inicialmente
}

// Definir cor no LED RGB
void set_rgb_led(uint8_t r, uint8_t g, uint8_t b) {
    gpio_put(LED_RED_PIN, r ? 1 : 0);
    gpio_put(LED_GREEN_PIN, g ? 1 : 0);
    gpio_put(LED_BLUE_PIN, b ? 1 : 0);
}

// Inicializar buzzers
void init_buzzers() {
    // Inicializar Buzzer A
    gpio_set_function(BUZZER_A_PIN, GPIO_FUNC_PWM);
    uint slice_num_a = pwm_gpio_to_slice_num(BUZZER_A_PIN);
    pwm_config config_a = pwm_get_default_config();
    pwm_config_set_clkdiv(&config_a, 125.0f); // 1 MHz
    pwm_config_set_wrap(&config_a, 500);      // 2 kHz
    pwm_init(slice_num_a, &config_a, true);
    pwm_set_gpio_level(BUZZER_A_PIN, 0);      // Desligado inicialmente

    // Inicializar Buzzer B
    gpio_set_function(BUZZER_B_PIN, GPIO_FUNC_PWM);
    uint slice_num_b = pwm_gpio_to_slice_num(BUZZER_B_PIN);
    pwm_config config_b = pwm_get_default_config();
    pwm_config_set_clkdiv(&config_b, 125.0f); // 1 MHz
    pwm_config_set_wrap(&config_b, 500);      // 2 kHz
    pwm_init(slice_num_b, &config_b, true);
    pwm_set_gpio_level(BUZZER_B_PIN, 0);      // Desligado inicialmente
}

// Tocar ambos os buzzers por uma duração
void buzzer_beep(uint32_t duration_ms) {
    pwm_set_gpio_level(BUZZER_A_PIN, 250); // 50% duty cycle (Buzzer A)
    pwm_set_gpio_level(BUZZER_B_PIN, 250); // 50% duty cycle (Buzzer B)
    vTaskDelay(pdMS_TO_TICKS(duration_ms));
    pwm_set_gpio_level(BUZZER_A_PIN, 0);
    pwm_set_gpio_level(BUZZER_B_PIN, 0);
}

// Inicializar botão com interrupção
void init_button() {
    gpio_init(BUTTON_A_PIN);
    gpio_set_dir(BUTTON_A_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_A_PIN);
    gpio_set_irq_enabled_with_callback(BUTTON_A_PIN, GPIO_IRQ_EDGE_FALL, true, &button_irq_handler);
}

// Manipulador de interrupção do botão
void button_irq_handler(uint gpio, uint32_t events) {
    static uint32_t last_time = 0;
    uint32_t current_time = to_ms_since_boot(get_absolute_time());

    if (current_time - last_time < 400) { // Debounce de 400ms
        return;
    }
    last_time = current_time;

    if (gpio == BUTTON_A_PIN) {
        is_night_mode = !is_night_mode; // Alternar modo
    }
}

// Tarefa de controle do semáforo
void vSemaphoreTask(void *pvParameters) {
    while (true) {
        if (is_night_mode) {
            // Modo Noturno: Amarelo piscando
            semaphore_state = YELLOW;
            set_rgb_led(255, 255, 0);
            set_matrix_color(255, 255, 0);
            vTaskDelay(pdMS_TO_TICKS(1000));
            set_rgb_led(0, 0, 0);
            set_matrix_color(0, 0, 0);
            vTaskDelay(pdMS_TO_TICKS(1000));
        } else {
            // Modo Normal: Ciclo de cores
            // Verde
            semaphore_state = GREEN;
            set_rgb_led(0, 255, 0);
            set_matrix_color(0, 255, 0);
            vTaskDelay(pdMS_TO_TICKS(3000));
            // Amarelo
            semaphore_state = YELLOW;
            set_rgb_led(255, 255, 0);
            set_matrix_color(255, 255, 0);
            vTaskDelay(pdMS_TO_TICKS(3000));
            // Vermelho
            semaphore_state = RED;
            set_rgb_led(255, 0, 0);
            set_matrix_color(255, 0, 0);
            vTaskDelay(pdMS_TO_TICKS(3000));
        }
    }
}

// Tarefa do buzzer
void vBuzzersTask(void *pvParameters) {
    while (true) {
        if (is_night_mode) {
            // Modo Noturno: Beep a cada 2s
            buzzer_beep(100);
            vTaskDelay(pdMS_TO_TICKS(2000));
        } else {
            switch (semaphore_state) {
                case GREEN: {
                    // Beep de 1s, pausa de 200ms, por 3s
                    for (int i = 0; i < 2; i++) {
                        buzzer_beep(1000);
                        vTaskDelay(pdMS_TO_TICKS(200));
                    }
                    buzzer_beep(600); // Último beep ajustado para 3s totais
                    break;
                }
                case YELLOW: {
                    // Beep de 350ms, pausa de 50ms, por 3s
                    for (int i = 0; i < 7; i++) {
                        buzzer_beep(350);
                        vTaskDelay(pdMS_TO_TICKS(50));
                    }
                    buzzer_beep(200); // Último beep ajustado para 3s totais
                    break;
                }
                case RED: {
                    // Beep de 500ms, pausa de 1500ms, por 3s
                    buzzer_beep(500);
                    vTaskDelay(pdMS_TO_TICKS(1500));
                    buzzer_beep(500);
                    vTaskDelay(pdMS_TO_TICKS(500)); // Ajustado para 3s totais
                    break;
                }
            }
        }
    }
}

// Tarefa do display
void vDisplayTask(void *pvParameters) {
    while (true) {
        ssd1306_fill(&ssd, false);
        ssd1306_draw_string(&ssd, is_night_mode ? "Modo: Noturno" : "Modo: Normal", 0, 0);
        switch (semaphore_state) {
            case GREEN:
                ssd1306_draw_string(&ssd, "Sinal: Verde", 0, 16);
                break;
            case YELLOW:
                ssd1306_draw_string(&ssd, "Sinal: Amarelo", 0, 16);
                break;
            case RED:
                ssd1306_draw_string(&ssd, "Sinal: Vermelho", 0, 16);
                break;
        }
        ssd1306_send_data(&ssd);
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}