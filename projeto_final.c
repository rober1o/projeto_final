#include "projeto_final.h"



int main()
{

    inicializar_hardware(); // inicializa e configura todos os perifericos

    // Inicia com a matriz de led apagada
    desenha_fig(matriz_apagada, BRILHO_PADRAO, pio, sm);

    while (true)
    {

        if (!sistema_desligado) // verifica se o sistema não foi desligado para prosseguir
        {

            adc_select_input(0);
            uint16_t sensor_MQ7 = adc_read();
            adc_select_input(1);
            uint16_t sensor_MQ2 = adc_read();

            // Converte valores dos sensores para porcentagem ajustando de acordo o erro do joystick
            int porcentagem_MQ2 = (abs(sensor_MQ2 - 2047) * 100) / 2047 - MARGEM_ERRO;
            int porcentagem_MQ7 = (abs(sensor_MQ7 - 2047) * 100) / 2047 - MARGEM_ERRO;

            char texto_x[15], texto_y[15];
            sprintf(texto_x, "MQ2: %d%%", porcentagem_MQ2);
            sprintf(texto_y, "MQ7: %d%%", porcentagem_MQ7);

            ssd1306_fill(&ssd, false); // Limpa o display

            ssd1306_draw_string(&ssd, texto_x, 10, 20); // Exibe X
            ssd1306_draw_string(&ssd, texto_y, 10, 40); // Exibe Y

            ssd1306_send_data(&ssd); // Atualiza o display

            tratar_MQ_2(porcentagem_MQ2);
            tratar_MQ_7(porcentagem_MQ7);
            sleep_ms(200);
        }
    }
}

// Função para inicializar o PWM
void inicializar_pwm(uint pino, uint limite, float divisor)
{
    gpio_set_function(pino, GPIO_FUNC_PWM);
    uint slice = pwm_gpio_to_slice_num(pino);
    pwm_config config = pwm_get_default_config();
    pwm_config_set_wrap(&config, limite);
    pwm_config_set_clkdiv(&config, divisor);
    pwm_init(slice, &config, true);
}

// Inicializa hardware e periféricos
void inicializar_hardware()
{
    adc_init();
    adc_gpio_init(SENSOR_MQ7);
    adc_gpio_init(SENSOR_MQ2);

    gpio_init(LED_RED);
    gpio_init(LED_BLUE);
    gpio_init(LED_GREEN);
    gpio_set_dir(LED_RED, GPIO_OUT);
    gpio_set_dir(LED_BLUE, GPIO_OUT);
    gpio_set_dir(LED_GREEN, GPIO_OUT);
    gpio_set_dir(BUTTON_A, GPIO_IN);
    gpio_set_dir(BUTTON_B, GPIO_IN);
    gpio_pull_up(BUTTON_A);
    gpio_pull_up(BUTTON_B);

    // Configura interrupção no botão
    gpio_set_irq_enabled_with_callback(BUTTON_A, GPIO_IRQ_EDGE_FALL, true, gpio_irq_handler);
    gpio_set_irq_enabled_with_callback(BUTTON_B, GPIO_IRQ_EDGE_FALL, true, gpio_irq_handler);

    inicializar_pwm(BUZZER_PIN, 12500, 4.0);

    i2c_init(I2C_PORT, 400 * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    // Inicializa o display OLED
    ssd1306_init(&display_oled, 128, 64, false, ENDERECO_DISPLAY, I2C_PORT);

    // Define o PIO 0 para controle da matriz de LEDs
    pio = pio0;

    // Configura o clock do sistema para 133 MHz
    bool clock_setado = set_sys_clock_khz(133000, false);

    // Inicializa a comunicação serial
    stdio_init_all();

    // Exibe mensagem na serial caso o clock tenha sido configurado com sucesso
    if (clock_setado)
        printf("Clock setado %ld\n", clock_get_hz(clk_sys));

    // Carrega o programa PIO para controle da matriz de LEDs
    int offset = pio_add_program(pio, &Matriz_5x5_program);

    // Obtém um state machine livre para o PIO
    sm = pio_claim_unused_sm(pio, true);

    // Inicializa o programa PIO na matriz de LEDs
    Matriz_5x5_program_init(pio, sm, offset, MATRIZ_PIN);

    configurar_i2c();

    // Configura o display OLED SSD1306
    configurar_display(&ssd);
}

void configurar_i2c()
{
    i2c_init(I2C_PORT, 400 * 1000);            // Inicializa o I2C na velocidade de 400 kHz
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C); // Define os pinos para função I2C
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA); // Ativa pull-up nos pinos I2C
    gpio_pull_up(I2C_SCL);
}

void configurar_display(ssd1306_t *ssd)
{
    ssd1306_init(ssd, WIDTH, HEIGHT, false, ENDERECO_DISPLAY, I2C_PORT); // Inicializa o display
    ssd1306_config(ssd);                                                 // Configura o display
    ssd1306_fill(ssd, false);                                            // Limpa o display
    ssd1306_send_data(ssd);                                              // Envia os dados para o display
}

// Função para emitir alertas de acordo com a porcentagem do sensor MQ-2  
void tratar_MQ_2(int valor_MQ_2)  
{
    // Verifica se a concentração detectada pelo sensor é baixa (até 30%)  
    if (valor_MQ_2 <= 30)  
    {
        // Ativa o LED Verde para indicar baixa concentração de gás  
        gpio_put(LED_RED, 0);   // Desliga LED Vermelho  
        gpio_put(LED_GREEN, 1); // Liga LED Verde  
        gpio_put(LED_BLUE, 0);  // Desliga LED Azul  
        
        // Para o som do buzzer, pois a concentração é segura  
        parar_buzzer();
    }
    // Verifica se a concentração detectada está em nível médio (entre 31% e 60%)  
    else if (valor_MQ_2 > 30 && valor_MQ_2 <= 60)  
    {
        // Ativa o LED Azul para indicar média concentração de gás  
        gpio_put(LED_RED, 0);   // Desliga LED Vermelho  
        gpio_put(LED_GREEN, 0); // Desliga LED Verde  
        gpio_put(LED_BLUE, 1);  // Liga LED Azul  
        
        // Aciona um alerta sonoro intermediário  
        tocar_alerta_amarelo();
    }
    // Caso contrário, a concentração é alta (acima de 60%)  
    else  
    {
        // Ativa o LED Vermelho para indicar alta concentração de gás  
        gpio_put(LED_RED, 1);   // Liga LED Vermelho  
        gpio_put(LED_GREEN, 0); // Desliga LED Verde  
        gpio_put(LED_BLUE, 0);  // Desliga LED Azul  
        
        // Aciona a sirene para alertar perigo  
        tocar_sirene();
    }
}


/// Função para emitir alertas de acordo com a porcentagem do sensor MQ-7  
void tratar_MQ_7(int valor_MQ_7)  
{
    // Verifica se o sistema está ligado antes de processar os alertas  
    if (!sistema_desligado)  
    {
        // Se a concentração detectada for baixa (até 30%)  
        if (valor_MQ_7 <= 30 && (!sistema_desligado))  
        {
            // Exibe alerta verde na interface gráfica  
            desenha_fig(alerta_verde, BRILHO_PADRAO, pio, sm);
            
            // Para o som do buzzer, pois a concentração está dentro do nível seguro  
            parar_buzzer();
        }
        // Se a concentração detectada estiver em nível médio (entre 31% e 60%)  
        else if (valor_MQ_7 > 30 && valor_MQ_7 <= 60)  
        {
            // Exibe alerta amarelo na interface gráfica  
            desenha_fig(alerta_amarelo, BRILHO_PADRAO, pio, sm);
            
            // Aciona um alerta sonoro intermediário  
            tocar_alerta_amarelo();
        }
        // Se a concentração detectada for alta (acima de 60%)  
        else  
        {
            // Exibe alerta vermelho na interface gráfica  
            desenha_fig(alerta_vermelho, BRILHO_PADRAO, pio, sm);
            
            // Aciona a sirene para indicar perigo  
            tocar_sirene();
        }
    }
}

// Função para tocar o buzzer com uma determinada frequência e volume  
void tocar_buzzer(uint16_t frequencia, float volume)  
{
    // Se a frequência ou o volume forem zero, apenas desliga o buzzer  
    if (frequencia == 0 || volume == 0)  
    {
        parar_buzzer(); // Desliga o buzzer  
        return;         // Sai da função  
    }

    // Obtém o número do slice PWM correspondente ao pino do buzzer  
    uint slice = pwm_gpio_to_slice_num(BUZZER_PIN);

    // Calcula o valor de "wrap" para definir a frequência do PWM  
    uint16_t wrap = 125000000 / (frequencia * 4); // Ajuste da frequência  

    // Define o valor de "wrap" para o slice PWM  
    pwm_set_wrap(slice, wrap);

    // Ajusta o nível de saída do PWM para definir o volume (duty cycle)  
    pwm_set_gpio_level(BUZZER_PIN, wrap * volume); 

    // Ativa o PWM, caso ainda não esteja ativado  
    pwm_set_enabled(slice, true);  
}


// Função para parar o buzzer  
void parar_buzzer()  
{
    // Define o nível de saída do PWM como 0, desligando o buzzer  
    pwm_set_gpio_level(BUZZER_PIN, 0);                         

    // Desativa o PWM do pino do buzzer  
    pwm_set_enabled(pwm_gpio_to_slice_num(BUZZER_PIN), false); 

    // Garante que o pino do buzzer esteja em nível baixo (LOW)  
    gpio_put(BUZZER_PIN, 0);                                   
}


// Função para tocar a sirene com variação de frequência  
void tocar_sirene()  
{
    // Verifica se a sirene está ativada antes de executar o som  
    if (sirene_ativa)  
    {
        // Aumenta a frequência do buzzer de 500 Hz para 1500 Hz em passos de 10 Hz  
        for (int freq = 500; freq <= 1500; freq += 10)  
        {
            tocar_buzzer(freq, 0.5); // Define a frequência e volume do buzzer  
            sleep_ms(20);            // Aguarda 20 milissegundos para criar o efeito sonoro  
        }

        // Diminui a frequência do buzzer de 1500 Hz para 500 Hz em passos de 10 Hz  
        for (int freq = 1500; freq >= 500; freq -= 10)  
        {
            tocar_buzzer(freq, 0.5); // Define a frequência e volume do buzzer  
            sleep_ms(20);            // Aguarda 20 milissegundos para criar o efeito sonoro  
        }

        // Para o buzzer ao final da execução  
        parar_buzzer();  
    }
}


// Função para tocar o alerta amarelo (três bips curtos)  
void tocar_alerta_amarelo()  
{
    // Verifica se a sirene está ativada antes de executar o som  
    if (sirene_ativa)  
    {
        // Executa três bips curtos  
        for (int i = 0; i < 3; i++)  
        {
            tocar_buzzer(1500, 0.5); // Toca um bip em 1500 Hz com volume médio  
            sleep_ms(100);           // Mantém o som por 100 milissegundos  
            
            parar_buzzer();          // Para o buzzer após o bip  
            sleep_ms(200);           // Aguarda 200 milissegundos antes do próximo bip  
        }
    }
}


// Função para desligar o sistema  
void desligar_sistema()  
{
    // Apaga qualquer desenho ou indicação visual na matriz de LEDs  
    desenha_fig(matriz_apagada, BRILHO_PADRAO, pio, sm);

    // Desliga os LEDs RGB  
    gpio_put(LED_BLUE, false);  
    gpio_put(LED_GREEN, false);  
    gpio_put(LED_RED, false);  

    // Limpa completamente o display OLED  
    ssd1306_fill(&ssd, false);  // Preenche a tela com "falso" (apagado)  
    ssd1306_send_data(&ssd);    // Envia os dados para atualizar o display  

    // Para qualquer som do buzzer  
    parar_buzzer();  
}

// Função para alternar o estado da sirene (ligar/desligar)  
void alternar_sirene()  
{
    if (sirene_ativa)  
    {
        gpio_put(BUZZER_PIN, 0); // Desliga o buzzer (nível LOW)  
        sirene_ativa = false;    // Atualiza o estado da sirene para desativada  
    }
    else  
    {
        gpio_put(BUZZER_PIN, 1); // Liga o buzzer (nível HIGH)  
        sirene_ativa = true;     // Atualiza o estado da sirene para ativada  
    }
}



// Função de interrupção do botão
void gpio_irq_handler(uint gpio, uint32_t events)
{

    if ((events & GPIO_IRQ_EDGE_FALL) && debounce_ok(gpio))
    { // Verifica se passou 200ms desde a última chamada do botão

        if (gpio == BUTTON_A)
        { 
            sistema_desligado = !sistema_desligado; // alterna entre o estado ligado e desligado do sistema
            desligar_sistema(); // chama a função para desligar todo sistema
        }
        else if (gpio == BUTTON_B)
        { 
            alternar_sirene();// ativa ou desativa o som buzzer
        }
    }
}

// Função para verificar debounce
bool debounce_ok(uint gpio)
{
    uint32_t tempo_atual = to_ms_since_boot(get_absolute_time());

    if (gpio == BUTTON_A)
    {
        if (tempo_atual - ultimo_tempoA > DEBOUNCE_TIME)
        {
            ultimo_tempoA = tempo_atual;
            return true;
        }
    }
    else if (gpio == BUTTON_B)
    {
        if (tempo_atual - ultimo_tempoB > DEBOUNCE_TIME)
        {
            ultimo_tempoB = tempo_atual;
            return true;
        }
    }
    return false;
}

void desenha_fig(uint32_t *_matriz, uint8_t _intensidade, PIO pio, uint sm)
{
    uint32_t pixel = 0;
    uint8_t r, g, b;

    for (int i = 24; i > 19; i--) // Linha 1
    {
        pixel = _matriz[i];
        b = ((pixel >> 16) & 0xFF) * (_intensidade / 100.00); // Isola os 8 bits mais significativos (azul)
        g = ((pixel >> 8) & 0xFF) * (_intensidade / 100.00);  // Isola os 8 bits intermediários (verde)
        r = (pixel & 0xFF) * (_intensidade / 100.00);         // Isola os 8 bits menos significativos (vermelho)
        pixel = 0;
        pixel = (g << 16) | (r << 8) | b;
        pio_sm_put_blocking(pio, sm, pixel << 8u);
    }

    for (int i = 15; i < 20; i++) // Linha 2
    {
        pixel = _matriz[i];
        b = ((pixel >> 16) & 0xFF) * (_intensidade / 100.00); // Isola os 8 bits mais significativos (azul)
        g = ((pixel >> 8) & 0xFF) * (_intensidade / 100.00);  // Isola os 8 bits intermediários (verde)
        r = (pixel & 0xFF) * (_intensidade / 100.00);         // Isola os 8 bits menos significativos (vermelho)
        pixel = 0;
        pixel = (b << 16) | (r << 8) | g;
        pixel = (g << 16) | (r << 8) | b;
        pio_sm_put_blocking(pio, sm, pixel << 8u);
    }

    for (int i = 14; i > 9; i--) // Linha 3
    {
        pixel = _matriz[i];
        b = ((pixel >> 16) & 0xFF) * (_intensidade / 100.00); // Isola os 8 bits mais significativos (azul)
        g = ((pixel >> 8) & 0xFF) * (_intensidade / 100.00);  // Isola os 8 bits intermediários (verde)
        r = (pixel & 0xFF) * (_intensidade / 100.00);         // Isola os 8 bits menos significativos (vermelho)
        pixel = 0;
        pixel = (g << 16) | (r << 8) | b;
        pio_sm_put_blocking(pio, sm, pixel << 8u);
    }

    for (int i = 5; i < 10; i++) // Linha 4
    {
        pixel = _matriz[i];
        b = ((pixel >> 16) & 0xFF) * (_intensidade / 100.00); // Isola os 8 bits mais significativos (azul)
        g = ((pixel >> 8) & 0xFF) * (_intensidade / 100.00);  // Isola os 8 bits intermediários (verde)
        r = (pixel & 0xFF) * (_intensidade / 100.00);         // Isola os 8 bits menos significativos (vermelho)
        pixel = 0;
        pixel = (g << 16) | (r << 8) | b;
        pio_sm_put_blocking(pio, sm, pixel << 8u);
    }

    for (int i = 4; i > -1; i--) // Linha 5
    {
        pixel = _matriz[i];
        b = ((pixel >> 16) & 0xFF) * (_intensidade / 100.00); // Isola os 8 bits mais significativos (azul)
        g = ((pixel >> 8) & 0xFF) * (_intensidade / 100.00);  // Isola os 8 bits intermediários (verde)
        r = (pixel & 0xFF) * (_intensidade / 100.00);         // Isola os 8 bits menos significativos (vermelho)
        pixel = 0;
        pixel = (g << 16) | (r << 8) | b;
        pio_sm_put_blocking(pio, sm, pixel << 8u);
    }
}


