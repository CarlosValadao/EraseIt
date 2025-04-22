#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include "pico/stdlib.h"
#include "pico/bootrom.h"
#include "lib/rgb.h"
#include "lib/joystick.h"
#include "lib/oledgfx.h"
#include "lib/push_button.h"
#include "hardware/pwm.h"
#include "lib/ws2812b.h"
#include "lib/ws2812b_motion.h"
#include "lib/uart_usb.h"
#include "lib/mlt8530.h"
#include "hardware/clocks.h"
#include "hardware/timer.h"

/// @brief Define a porta I2C utilizada pelo OLED.
#define I2C_PORT i2c1

/// @brief Definições dos pinos do display OLED.
#define OLED_SDA 14
#define OLED_SCL 15
#define OLED_ADDR 0x3C
#define OLED_BAUDRATE 400000  ///< Taxa de comunicação I2C para o display OLED.

/// @brief Definições dos pinos do joystick.
#define JOYSTICK_VRX 27  ///< Pino do eixo X do joystick.
#define JOYSTICK_VRY 26  ///< Pino do eixo Y do joystick.
#define JOYSTICK_PB  22  ///< Pino do botão do joystick.

#define BUZZER_A 10
#define BUZZER_B 21

/// @brief Definições dos pinos do LED RGB.
#define RED_PIN   13  ///< Pino do LED vermelho.
#define BLUE_PIN  12  ///< Pino do LED azul.
#define GREEN_PIN 11  ///< Pino do LED verde.

/// @brief Macros para verificar se um botão foi pressionado.
#define JOYSTICK_SW_PRESSED gpio == 22
#define BUTTON_A_PRESSED gpio == BUTTON_A
#define BUTTON_B_PRESSED gpio == BUTTON_B

#define GAME_STATUS_WAITING 0
#define GAME_STATUS_START   1
#define GAME_STATUS_END     3

// Converte um caractere numérico para seu valor inteiro correspondente
#define CHAR_TO_NUM(value) ((uint8_t) (value-48))

/// @brief Ativa o modo de boot USB para atualizar o firmware.
#define set_bootsel_mode() reset_usb_boot(0, 0)

/// @brief Variável global para controlar o tipo de borda no OLED.
static volatile uint8_t border_type = BORDER_LIGHT;

static volatile uint8_t timer_counter = 9;

static volatile uint8_t game_status = GAME_STATUS_WAITING;

static volatile uint16_t cleared_display_bits = 0;

/// @brief Variáveis globais para controlar o estado dos LEDs.
static volatile bool led_red_active = false;
static volatile bool led_green_active = false;
static volatile bool led_blue_active = false;
static volatile bool led_control_override = false; ///< Se ativo, sobrepõe os estados individuais dos LEDs.

/// @brief Ponteiro global para o objeto OLED.
static ssd1306_t *ssd_global = NULL;
static ws2812b_t *ws_global = NULL;

/**
 * @brief Normaliza um valor do joystick para a escala do display.
 *
 * @param joystick_vr Valor lido do joystick (0-4095).
 * @param new_max Valor máximo desejado para normalização.
 * @return Valor normalizado dentro da nova faixa.
 */
static uint16_t normalize_joystick_to_display(uint16_t joystick_vr, uint8_t new_max);

/**
 * @brief Ajusta a intensidade do LED com base no valor do PWM.
 *
 * @param pwm_led_value Valor PWM do LED.
 * @return Valor ajustado para o PWM.
 */
static uint16_t adjust_pwm_led_value(uint16_t pwm_value);

/**
 * @brief Callback para interrupções de GPIO (botões).
 *
 * @param gpio Pino que acionou a interrupção.
 * @param event Tipo de evento da interrupção.
 */
static void gpio_irq_callback(uint gpio, uint32_t event);

// Função de callback que será chamada periodicamente
bool repeating_timer_callback(struct repeating_timer *t);

// Definição das cores para exibição na matriz de LEDs WS2812B
static const uint8_t COLORS[] = {PURPLE, GREEN, BLUE_MARINE, RED, YELLOW, BLUE, GREEN, BLUE, YELLOW};

/// @brief Função principal do programa.
int main()
{
    // Configura o clock do sistema para 128 MHz
    // por causa da matriz de LEDS (divisão exata do clock)
    set_sys_clock_khz(128000, false);

    stdio_init_all();  ///< Inicializa a comunicação serial.

    rgb_t rgb;
    uint16_t adj_led_red_pwm_value, adj_led_blue_pwm_value;
    uint8_t joystick_vrx_norm, joystick_vry_norm;
    uint16_t joystick_vrx, joystick_vry;
    joystick_t joy;
    ssd1306_t ssd;
    char cleared_bits_message[16];
    struct repeating_timer timer;

    // Inicializa o LED RGB, Joystick e Display OLED
    rgb_init_all(&rgb, RED_PIN, GREEN_PIN, BLUE_PIN, 1.0, 2048);
    joystick_init_all(&joy, JOYSTICK_VRX, JOYSTICK_VRY, JOYSTICK_PB, 120);
    oledgfx_init_all(&ssd, I2C_PORT, OLED_BAUDRATE, OLED_SDA, OLED_SCL, OLED_ADDR);
    ssd_global = &ssd;  ///< Guarda a referência global do OLED.

    // Configuração dos botões e interrupções
    pb_config(JOYSTICK_PB, true);
    pb_config_btn_a();
    pb_config_btn_b();
    pb_set_irq_callback(&gpio_irq_callback);
    pb_enable_irq(BUTTON_A);
    pb_enable_irq(JOYSTICK_PB);
    pb_enable_irq(BUTTON_B);

    buzzer_init(BUZZER_A);
    buzzer_init(BUZZER_B);

    // Declaração do objeto para a matriz de LEDs WS2812B
    ws2812b_t *ws;

    // Inicializa a matriz de LEDs WS2812B
    ws = init_ws2812b(pio0, WS2812B_PIN);
    ws_global = ws;
    
    ws2812b_turn_off_all(ws_global);
    pwm_set_gpio_level(GREEN_PIN, 128);
    // Desenha a borda inicial no OLED
    printf("intializing...\n");
    while(true)
    {
        if(game_status == GAME_STATUS_START)
        {
            buzzer_beep(BUZZER_A, 100, 2000);
            printf("game started\n");
            ws2812b_draw(ws_global, NUMERIC_GLYPHS[timer_counter], COLORS[timer_counter], 1);
            // Adiciona um alarme que chama repeating_timer_callback() a cada 1000 ms (1 segundo)
            add_repeating_timer_ms(1000, repeating_timer_callback, NULL, &timer);
            oledgfx_random_fill_display(ssd_global);
            while(game_status == GAME_STATUS_START)
            {
                // sleep_ms(10);
                oledgfx_draw_border(&ssd, border_type);
                //oledgfx_draw_border(&ssd, 1);
                // Lê os valores do joystick
                joystick_vrx = joystick_get_x(&joy);
                joystick_vry = joystick_get_y(&joy);
        
                // Normaliza os valores do joystick para o display
                joystick_vrx_norm = normalize_joystick_to_display(joystick_vrx, 127 - CURSOR_SIDE - border_type);
                joystick_vry_norm = (63 - CURSOR_SIDE) - normalize_joystick_to_display(joystick_vry, 63 - CURSOR_SIDE - border_type);
        
                // Atualiza o cursor e redesenha a borda no OLED
                oledgfx_update_cursor(&ssd, joystick_vrx_norm, joystick_vry_norm);
                oledgfx_render(&ssd);
    
                // Se o controle do LED não estiver sobreposto, ajusta as intensidades do LED com base no joystick
                // if(!led_control_override)
                // {
                adj_led_red_pwm_value = adjust_pwm_led_value(joystick_vrx);
                adj_led_blue_pwm_value = adjust_pwm_led_value(joystick_vry);
                pwm_set_gpio_level(BLUE_PIN, adj_led_blue_pwm_value);
                pwm_set_gpio_level(RED_PIN, adj_led_red_pwm_value);
            }
            printf("end game\n");
            buzzer_beep(BUZZER_A, 2000, 200);
            cleared_display_bits = oledgfx_count_lit_pixels(ssd_global);
            printf("%u bits was cleared\n", cleared_display_bits);
            // }
        }
        else if(game_status == GAME_STATUS_WAITING)
        {
            oledgfx_clear_screen(ssd_global);
            oledgfx_draw_border(ssd_global, BORDER_THICK);
            ssd1306_draw_string(ssd_global, "A to start", 5, 24);
            ssd1306_draw_string(ssd_global, "SW to this menu", 5, 32);
            sprintf(cleared_bits_message, "%u bits clear", cleared_display_bits);
            ssd1306_draw_string(ssd_global, cleared_bits_message, 5, 40);
            // ssd1306_draw_string(ssd_global, "", 25, 48);
            ssd1306_send_data(ssd_global);
            sleep_ms(50);
        }
        else if(game_status == GAME_STATUS_END)
        {
            cancel_repeating_timer(&timer);
            timer_counter = 9;
            // cleared_display_bits = 0;
            pwm_set_gpio_level(BLUE_PIN, 0);
            pwm_set_gpio_level(RED_PIN, 0);
            pwm_set_gpio_level(GREEN_PIN, 128);
            ws2812b_turn_off_all(ws_global);
            game_status = GAME_STATUS_WAITING;
        }
    }
    return EXIT_SUCCESS;
}

/**
 * @brief Normaliza um valor do joystick para a escala do display.
 *
 * @param joystick_vr Valor lido do joystick (0-4095).
 * @param new_max Valor máximo desejado para normalização.
 * @return Valor normalizado dentro da nova faixa.
 */
static uint16_t normalize_joystick_to_display(uint16_t joystick_vr, uint8_t new_max)
{
    return (joystick_vr * new_max) / 4095;
}

/**
 * @brief Callback para interrupções de botões e joystick.
 *
 * Se o botão B for pressionado, entra no modo de boot USB.
 * Se o botão A for pressionado, desliga os LEDs e alterna `led_control_override`.
 * Se o botão do joystick for pressionado, alterna entre bordas finas e grossas no OLED.
 *
 * @param gpio Pino que acionou a interrupção.
 * @param event Tipo de evento da interrupção.
 */
static void gpio_irq_callback(uint gpio, uint32_t event)
{
    if(pb_is_debounce_delay_over())
    {
        if(BUTTON_B_PRESSED) 
        {
            set_bootsel_mode();
        }
        // colocar o program_mode em conjunto
        // tanto nesse else if quanto no outro else if
        else if(BUTTON_A_PRESSED) 
        {
            game_status = GAME_STATUS_START;
            pwm_set_gpio_level(GREEN_PIN, 0);
            // pwm_set_gpio_level(BLUE_PIN, 0);
            // led_control_override = !led_control_override;
        }
        else if(JOYSTICK_SW_PRESSED)
        {
            // oledgfx_clear_screen(ssd_global);
            // if(led_green_active)
            // {
            //     border_type = BORDER_LIGHT;
            //     oledgfx_draw_border(ssd_global, BORDER_LIGHT);
            //     pwm_set_gpio_level(GREEN_PIN, 0);
            // }
            // else
            // {
                // border_type = BORDER_THICK;
                // oledgfx_draw_border(ssd_global, BORDER_THICK);
                game_status = GAME_STATUS_END;
                // pwm_set_gpio_level(BLUE_PIN, 0);
                // pwm_set_gpio_level(RED_PIN, 0);
                // pwm_set_gpio_level(GREEN_PIN, 128);
            // }
            // led_green_active = !led_green_active;
        }
    }
}

/**
 * @brief Ajusta o valor do PWM do LED com base no movimento do joystick.
 *
 * Se o joystick for movido para a direita, o brilho do LED diminui.
 * Se o joystick for movido para a esquerda, o brilho do LED aumenta gradualmente.
 *
 * @param pwm_value Valor do joystick (0-4095).
 * @return Valor ajustado para o PWM (0-2048).
 */
static uint16_t adjust_pwm_led_value(uint16_t pwm_value)
{
    if(pwm_value >= 2048)
        return pwm_value - 2048;
    else 
        return 2048 - pwm_value;  ///< Ajusta o brilho gradualmente para a esquerda.
}

// Função de callback que será chamada periodicamente
bool repeating_timer_callback(struct repeating_timer *t) {
    timer_counter--;
    ws2812b_draw(ws_global, NUMERIC_GLYPHS[timer_counter], COLORS[timer_counter], 1);
    if(timer_counter == 0)
    {
        cancel_repeating_timer(t);
        game_status = GAME_STATUS_END;
        timer_counter = 9;
    }
    return true;
}