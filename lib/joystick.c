#include "joystick.h"
#include "push_button.h"
#include "hardware/adc.h"

/**
 * @file joystick.c
 * @brief Implementação das funções para controle do joystick.
 *
 * Este módulo contém as funções para inicialização, leitura e configuração do joystick.
 * Ele permite obter as coordenadas X e Y, verificar se o botão foi pressionado,
 * identificar se o joystick está se movendo além da zona morta e configurar a deadzone.
 *
 * @author Carlos Valadão
 * @date 2025-02-16
 * @version 1.0
 * @copyright
 * Copyright (C) 2025 Carlos Valadão
 */

/**
 * @defgroup Joystick Funções do Joystick
 * @brief Grupo de funções relacionadas ao controle do joystick.
 *
 * Essas funções permitem inicializar o joystick, obter os valores dos eixos X e Y,
 * verificar o estado do botão e configurar a zona morta.
 * @{
 */

/**
 * @brief Converte um número de GPIO para o número do canal ADC correspondente.
 *
 * Esta função recebe um número de GPIO e retorna o número do canal ADC associado.
 * Os GPIOs válidos para ADC são 26, 27, 28 e 29, correspondentes aos canais 0, 1, 2 e 3, respectivamente.
 *
 * @note Apenas os GPIOs 26 a 29 são suportados, pois são os únicos pinos com entrada ADC no RP2040.
 * @warning Se um GPIO inválido for passado, a função pode retornar um valor indefinido.
 *
 * @param[in] gpio O número do pino GPIO a ser convertido.
 * @return O número do canal ADC correspondente, ou comportamento indefinido para valores inválidos.
 */
static uint8_t adc_gpio_to_channel_num(uint8_t gpio) {
    switch (gpio)
    {
        case ADC_GPIO_26: return ADC_CHANNEL_1;
        case ADC_GPIO_27: return ADC_CHANNEL_2;
        case ADC_GPIO_28: return ADC_CHANNEL_3;
        case ADC_GPIO_29: return ADC_CHANNEL_4;
    }
}

/**
 * @brief Inicializa o joystick configurando os pinos e a estrutura.
 *
 * Configura os pinos de entrada analógica para os eixos X e Y e o botão de push como entrada digital com pull-up ativado.
 * Além disso, preenche a estrutura `joystick_t` com os valores informados.
 *
 * @note Esta função deve ser chamada antes de realizar qualquer leitura do joystick.
 * @warning Se os pinos forem configurados incorretamente, o joystick não funcionará como esperado.
 *
 * @todo Implementar a configuração dos pinos e ADC.
 *
 * @param[in,out] joy Ponteiro para a estrutura do joystick que será inicializada.
 * @param[in] joy_vrx Canal ADC correspondente ao eixo X.
 * @param[in] joy_vry Canal ADC correspondente ao eixo Y.
 * @param[in] joy_pbutton Pino GPIO do botão do joystick.
 */
void joystick_init_all(joystick_t *joy, uint8_t joy_vrx, uint8_t joy_vry, uint8_t joy_pbutton, uint8_t deadzone)
{
    adc_init();
    adc_gpio_init(joy_vrx);
    adc_gpio_init(joy_vry);
    pb_config(joy_pbutton, true);
    uint8_t joy_vrx_channel = adc_gpio_to_channel_num(joy_vrx);
    uint8_t joy_vry_channel = adc_gpio_to_channel_num(joy_vry);
    joy->channel_x = joy_vrx_channel;
    joy->channel_y = joy_vry_channel;
    joy->joy_push_button = joy_pbutton;
    joy->deadzone = (uint8_t) (deadzone);
}

static uint16_t joystick_read_filtered(uint8_t channel, uint8_t deadzone)
{
    adc_select_input(channel);
    uint16_t raw_value = adc_read(); // Leitura do ADC
    uint16_t center = 2048; // Centro do joystick em um ADC de 12 bits
    // Se estiver dentro da deadzone, retorna o centro para evitar ruído
    if (raw_value > (center - deadzone) && raw_value < (center + deadzone))
    {
        return center;
    }
    return raw_value;
}

/**
 * @brief Obtém o valor do eixo X do joystick.
 *
 * Esta função realiza a leitura do canal ADC correspondente ao eixo X do joystick.
 * O valor retornado pode ser normalizado para ser usado na interface gráfica.
 *
 * @note O ADC deve estar corretamente inicializado antes da leitura.
 * @todo Implementar a leitura real do ADC.
 *
 * @param[in] joy Ponteiro para a estrutura do joystick.
 * @return Valor do ADC correspondente ao eixo X (exemplo: 0 - 4095 em ADC de 12 bits).
 */
uint16_t joystick_get_x(const joystick_t *joy)
{
    return joystick_read_filtered(joy->channel_x, joy->deadzone);
}

/**
 * @brief Obtém o valor do eixo Y do joystick.
 *
 * Esta função realiza a leitura do canal ADC correspondente ao eixo Y do joystick.
 *
 * @note O ADC deve estar corretamente inicializado antes da leitura.
 * @todo Implementar a leitura real do ADC.
 *
 * @param[in] joy Ponteiro para a estrutura do joystick.
 * @return Valor do ADC correspondente ao eixo Y (exemplo: 0 - 4095 em ADC de 12 bits).
 */
uint16_t joystick_get_y(const joystick_t *joy)
{
    return joystick_read_filtered(joy->channel_y, joy->deadzone);
}

/**
 * @brief Verifica se o botão do joystick está pressionado.
 *
 * Esta função lê o estado do pino digital associado ao botão do joystick.
 * Se o botão estiver pressionado, a função retorna `true`, caso contrário, retorna `false`.
 *
 * @note O pino do botão deve ser configurado corretamente como entrada digital com pull-up ativado.
 * @todo Implementar a leitura real do botão.
 *
 * @param[in] joy Ponteiro para a estrutura do joystick.
 * @return `true` se o botão estiver pressionado, `false` caso contrário.
 */
bool joystick_get_button(const joystick_t *joy)
{
    return false; // TODO: Implementar leitura do botão
}

/**
 * @brief Verifica se o joystick está se movendo além da deadzone.
 *
 * Esta função compara o valor lido dos eixos X e Y com a deadzone definida
 * para determinar se há um movimento significativo.
 *
 * @note A deadzone deve estar configurada corretamente para evitar detecção de ruídos.
 * @warning Se a deadzone for muito alta, pequenos movimentos podem ser ignorados.
 * @todo Implementar a verificação de movimento baseada na deadzone.
 *
 * @param[in] joy Ponteiro para a estrutura do joystick.
 * @return `true` se o joystick estiver fora da deadzone, `false` caso contrário.
 */
bool joystick_is_moving(const joystick_t *joy)
{
    return false; // TODO: Implementar detecção de movimento
}

/**
 * @brief Define o valor da zona morta (deadzone) do joystick.
 *
 * A zona morta é a faixa de valores ao redor do centro onde pequenas variações
 * de posição não são consideradas como movimento. Isso evita ruídos indesejados.
 *
 * @note O valor da deadzone deve ser ajustado de acordo com a sensibilidade desejada.
 * @todo Implementar a configuração da deadzone na estrutura do joystick.
 *
 * @param[in,out] joy Ponteiro para a estrutura do joystick.
 * @param[in] deadzone_value Novo valor da zona morta.
 */
void joystick_set_deadzone(joystick_t *joy, uint8_t deadzone_value)
{
    joy->deadzone = deadzone_value;
}

/** @} */ // Fim do grupo "Joystick"
