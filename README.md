# SISTEMA EMBARCADO PARA MONITORAMENTO DE VAZAMENTO DE GÁS

Este projeto tem como objetivo desenvolver um sistema de alerta para detecção de vazamento de gases inflamáveis e monóxido de carbono, utilizando exclusivamente a placa BitDogLab. A proposta consiste em simular o funcionamento dos sensores MQ-2 (capaz de detectar gases inflamáveis como metano, butano e hidrogênio) e MQ-7 (especializado na detecção de monóxido de carbono), aproveitando o joystick embutido na BitDogLab como fonte de dados analógicos.

O joystick, que também gera sinais analógicos, será utilizado para simular as variações de concentração de gás, convertendo esses valores em dados digitais para análise. Com base nesses valores, o sistema ativará diferentes alertas visuais e sonoros, representando níveis seguros, moderados ou críticos de gases no ambiente.

Este projeto visa demonstrar, de forma acessível e didática, a aplicação de sensores de gás em sistemas embarcados, explorando a BitDogLab como plataforma de prototipagem. 

## Componentes Utilizados

1. **LED RGB** 
2. **Matriz de LED 5x5 WS2812** 
3. **Microcontrolador Raspberry Pi Pico W**
4. **Dois botões Pushbutton**
5. **Buzzer**
6 . **Joystick**
7. **Display OLED 1306**

## Funcionalidade

Ao iniciar o programa, o display OLED exibirá constantemente valores de 0 a 100, representando a concentração de gás detectada por cada um dos sensores simulados (MQ-2 e MQ-7). O joystick será utilizado para ajustar esses valores, simulando diferentes níveis de detecção.

MQ-2 (Gases Inflamáveis - Metano, Butano, Hidrogênio)

 - Se a concentração for igual o inferiro a 30 o sistema manterá o led RGB na cor Verde e não emitirá nenhum sinal sonoro
 - Se a concentração for entre 31 - 60 o sistema alternará  a cor do led RGB para azul, indicando um risco moderado e o buzzer emitirá pequenos bips
 - Se a concentração for superior a 60 o sistema alternará a cor do Led RGB para Vermelho, indicando risco alto e o buzzer emitirar um toque de sirene oscilante.


 MQ-7 (Gás carbono)

 - Se a concentração for igual o inferiro a 30 o sistema manterá o a matriz de led com um quadrado na cor verde e não emitirá nenhum sinal sonoro
 - Se a concentração for entre 31 - 60 o sistema alternará a matriz de led com um quadrado na amarela , indicando um risco moderado e o buzzer emitirá pequenos bips
 - Se a concentração for superior a 60 o sistema alternará  a matriz de led com um quadrado na cor Vermelha, indicando risco alto e o buzzer emitirar um toque de sirene oscilante.

Funcionalidades dos Botões

Botão A: Liga/desliga o sistema de monitoramento de gases.

Botão B: Ativa/desativa a sirene manualmente.

### Como Usar

#### Usando o Simulador Wokwi

- Clone este repositório: git clone https://github.com/rober1o/10.37M2---U4C4O12T----Atividade_interrupcao.git;
- Usando a extensão Raspberry Pi Pico importar o projeto;
- Compilar o código e clicar no arquivo diagram.json.

#### Usando a BitDogLab

- Clone este repositório: git clone https://github.com/rober1o/10.37M2---U4C4O12T----Atividade_interrupcao.git;
- Usando a extensão Raspberry Pi Pico importar o projeto;
- Compilar o projeto;
- Plugar a BitDogLab usando um cabo apropriado e gravar o código.

## Atenção

O brilho dos LEDs é muito intenso, para evitar danos aos olhos recomendamos que o brilho_padrao seja ajustado para 10% quando estiver usando o kit BitDogLab.

## Demonstração

<!-- TODO: adicionar link do vídeo -->
Vídeo demonstrando as funcionalidades da solução implementada: [Demonstração](https://youtu.be/pDz5KutL2i4)
