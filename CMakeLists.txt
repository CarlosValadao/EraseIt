cmake_minimum_required(VERSION 3.13)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(PICO_BOARD pico_w CACHE STRING "Board type")
include(pico_sdk_import.cmake)

# Define o nome do projeto como Teste_ADC_Display, suportando C, C++ e Assembly.
project(BitInteract C CXX ASM) 
pico_sdk_init()


add_executable(${PROJECT_NAME}  
        BitInteract.c # Código principal em C
        lib/ssd1306.c # Biblioteca para o display OLED
        lib/mlt8530.c # Biblioteca para o buzzer passivo MLT8530
        lib/oledgfx.c # Biblioteca de mais alto nível para o display OLED
        lib/joystick.c # Biblioteca para o joystick
        lib/push_button.c # Biblioteca para switches no geral
        lib/rgb.c # Biblioteca para o LED RGB SMD5050
        lib/ws2812b.c # Biblioteca para a matriz de LEDS WS2812b
        lib/lite5.c # Protocolo de comunicação sobre UART
        lib/ws2812b_motion.c # Implementação de animações sobre ws2812b
        lib/uart_usb.c # Biblioteca para comunicação UART sobre USB
        )

target_link_libraries(${PROJECT_NAME} 
        pico_stdlib
        pico_bootrom
        hardware_i2c
        hardware_adc
        hardware_timer
        hardware_pwm
        hardware_pio
        hardware_clocks
        )

pico_enable_stdio_usb(${PROJECT_NAME} 1)
pico_enable_stdio_uart(${PROJECT_NAME} 1)

file(MAKE_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/generated)
pico_generate_pio_header(${PROJECT_NAME} ${CMAKE_CURRENT_LIST_DIR}/ws2812b.pio OUTPUT_DIR ${CMAKE_CURRENT_LIST_DIR}/generated)

pico_add_extra_outputs(${PROJECT_NAME})