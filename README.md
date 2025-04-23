# Erase It - Jogo Interativo de Apagar Pixels 🎮✨

## Objetivo Geral 🎯

**Erase It** é um jogo interativo onde o jogador deve apagar o máximo possível de caracteres (pixels) em um display OLED SSD1306 em um tempo limite de 10 segundos. O jogo utiliza um joystick para movimentar um cursor que apaga os pixels no display, enquanto uma matriz de LEDs WS2812b exibe a contagem regressiva do tempo restante. O brilho de um LED SMD5050 também é controlado pelo joystick, proporcionando uma resposta visual dinâmica. O sistema visa oferecer uma experiência de entretenimento simples e envolvente por meio da **BitDogLab**.

## Descrição Funcional 🕹️

### Estados do Jogo ⚙️

O jogo possui três estados principais:

1. **GAME_STATUS_WAITING (I)** - O jogo espera por uma interação do jogador. O estado aguarda até que o jogador pressione um botão para iniciar o jogo.
2. **GAME_STATUS_START (II)** - O jogo é iniciado com a interação do jogador. O jogo começa a contagem regressiva e a comunicação entre o computador e o jogo via UART.
3. **GAME_STATUS_END (III)** - O jogo termina quando o tempo se esgota. O jogo exibe o feedback final e retorna ao estado inicial.

### Fluxo do Jogo 🔄

- **Início do Jogo**: Na tela inicial, o jogador deve pressionar o botão A para iniciar o jogo. Ao pressioná-lo, a contagem regressiva é iniciada na matriz de LEDs WS2812b, e um beep de 300 ms é emitido pelo buzzer MLT 8530.
- **Movimento e Interação**: O joystick é utilizado para mover o cursor no display OLED SSD1306, apagando os pixels à medida que o jogador se move.
- **Contagem de Tempo**: O tempo restante é mostrado na matriz de LEDs WS2812b e é atualizado a cada segundo.
- **Fim do Jogo**: Ao final do jogo, um beep de 3000 ms é emitido pelo buzzer, e o jogo retorna à tela inicial, exibindo a quantidade de pixels apagados.

https://github.com/user-attachments/assets/bf6aabbe-994e-4bb2-8fdc-dbdef15ae9d9

### Funcionalidade da Comunicação 💬

- **Início do Jogo**: O sistema envia uma mensagem via UART informando que o jogo começou.
- **Finalização do Jogo**: Ao final, uma mensagem é enviada indicando o fim do jogo.
- **Pixels Apagados**: A quantidade de pixels apagados é enviada periodicamente via UART para monitoramento remoto.

## Uso dos Periféricos da BitDogLab 🛠️

### Potenciômetro do Joystick

Os potenciômetros do joystick controlam o movimento do cursor no display OLED SSD1306. O movimento é lido e normalizado para apagar pixels no display e ajustar o brilho do LED RGB SMD5050.

### Botões 🕹️

- **Botão A**: Inicia o jogo. Ao pressioná-lo, a contagem regressiva começa.
- **Botão SW (Joystick)**: Reseta o jogo, retornando à tela inicial.
- **Botão B**: Entra no modo bootloader para reiniciar ou atualizar o sistema.

### Interrupções ⚡

Interrupções de GPIO são usadas para capturar eventos dos botões, garantindo respostas rápidas e precisas.

### Tratamento de Debounce dos Botões 🔄

O debounce é realizado em software, com um tempo de 200 ms para garantir que os botões respondam de forma estável, evitando leituras erradas.

### Display OLED SSD1306 🖥️

O display OLED SSD1306 exibe o estado principal do jogo, incluindo os caracteres a serem apagados, a contagem de pixels apagados e o tempo restante.

### Matriz de LEDs WS2812b 🌈

A matriz de LEDs WS2812b exibe a contagem regressiva do tempo, atualizando a cor a cada segundo e fornecendo feedback visual sobre o progresso do jogo.

### LED RGB SMD5050 💡

O LED RGB SMD5050 fornece feedback visual sobre o movimento do joystick, ajustando seu brilho conforme a intensidade do movimento.

### Buzzer 🔊

O buzzer emite beeps curtos e longos para fornecer feedback sonoro, indicando o início e o fim do jogo.

### Comunicação Serial via UART 🔌

A comunicação UART permite a troca de informações entre o sistema e o computador, incluindo o estado do jogo e a quantidade de pixels apagados.

## Tecnologias Utilizadas 💻

- **Linguagem**: C
- **SDK**: Pico SDK 2.1.0
- **Placa**: BitDogLab

## Como Executar o Projeto 🚀

1. **Conectar a Placa BitDogLab** via USB ao seu computador.
2. **Instalar o Pico SDK 2.1.0** em seu ambiente de desenvolvimento. Se necessário, siga as instruções de instalação do SDK [aqui](https://github.com/raspberrypi/pico-sdk).
3. **Carregar o código** no dispositivo usando uma IDE compatível com o Pico SDK (ex: VS Code ou CLion).
4. **Compilar e carregar o projeto** para o dispositivo. No terminal, use o seguinte comando para compilar o código:

   ```bash
   cmake -B build
   make -C build
