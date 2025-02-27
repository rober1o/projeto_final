#ifndef PROJETO_FINAL  // Previne múltiplas inclusões do cabeçalho
#define PROJETO_FINAL

// =============================
// Bibliotecas padrão e específicas do Raspberry Pi Pico
// =============================
#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"   // Biblioteca padrão do Raspberry Pi Pico
#include "hardware/pio.h"  // Controle do PIO (Programável I/O)
#include "hardware/clocks.h" // Manipulação de clock
#include "hardware/i2c.h"  // Comunicação I2C
#include "pico/bootrom.h"  // Funções de bootloader
#include "pico/time.h"     // Manipulação de tempo
#include "hardware/adc.h"
#include "hardware/pwm.h"

// =============================
// Bibliotecas do projeto
// =============================
#include "pio_wave.pio.h"  // Código PIO 
#include "numeros.h"       // Arrays para exibir na matriz de LED
#include "inc/ssd1306.h"   // Controle do display OLED SSD1306
#include "inc/font.h"      // Manipulação de fontes

// =============================
// Definições de hardware
// =============================

// LEDs RGB
#define LED_GREEN 11
#define LED_BLUE 12
#define LED_RED 13

// Botões
#define BUTTON_A 5
#define BUTTON_B 6

// Configurações gerais
#define BRILHO_PADRAO 100  // Intensidade padrão do brilho
#define DEBOUNCE_TIME 200
// Matriz de LEDs
#define MATRIZ_PIN 7       // Pino da matriz de LEDs
#define NUM_PIXELS 25      // Número de pixels na matriz

// Configurações I2C (Display OLED)
#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define ENDERECO_DISPLAY 0x3C  // Endereço I2C do display OLED

#define SENSOR_MQ2 26
#define SENSOR_MQ7 27
#define BOTAO_JOYSTICK 22
#define PWM_LIMITE 4095
#define PWM_DIVISOR 30.52f
#define MARGEM_ERRO 0  // Define a faixa onde os LEDs permanecerão apagados
#define BUZZER_PIN 21 // Defina o pino do buzzer


// =============================
// Variáveis globais
// =============================

volatile uint32_t ultimo_tempoA = 0;  // Última leitura do botão A
volatile uint32_t ultimo_tempoB = 0;  // Última leitura do botão B
volatile bool ligado = false;         // Estado do sistema
ssd1306_t display_oled;
volatile bool sistema_desligado = false;
bool sirene_ativa = true; // Variável de estado

// Instâncias de hardware
ssd1306_t ssd;  // Display OLED
PIO pio;        // Instância do PIO
int sm;         // Máquina de estado do PIO

// =============================
// Declaração das funções
// =============================

void configurar_i2c(void);
bool debounce_ok(uint gpio);
void gpio_irq_handler(uint gpio, uint32_t events);
void inicializar_pwm(uint pino, uint limite, float divisor);
void desenha_fig(uint32_t *_matriz, uint8_t _intensidade, PIO pio, uint sm);
void configurar_display(ssd1306_t *ssd);
void tratar_MQ_2(int valor_MQ_2);
void tratar_MQ_7(int valor_MQ_7);
void tocar_buzzer(uint16_t frequencia, float volume);
void parar_buzzer(void);
void tocar_sirene(void);
void tocar_alerta_amarelo(void);
void desligar_sistema(void); 
void alternar_sirene(void);
#endif // PROJETO_FINAL
