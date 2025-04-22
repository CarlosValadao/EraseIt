#ifndef JOYSTICK_H
#define JOYSTICK_H

#include <stdint.h>
#include "hardware/timer.h"

/**
 * @file joystick.h
 * @brief Definições e funções para controle do joystick analógico.
 *
 * Este cabeçalho contém a definição da estrutura `joystick_t` e as funções 
 * para inicialização, leitura e configuração do joystick. Ele permite obter 
 * os valores dos eixos X e Y, verificar o estado do botão e configurar a deadzone.
 *
 * @author Seu Nome
 * @date 2025-02-16
 * @version 1.0
 * @license GNU General Public License v3.0 (GPLv3)
 * 
 * @copyright
 * Copyright (C) 2025 Carlos Valadão
 *
 * Este programa é um software livre: você pode redistribuí-lo e/ou modificá-lo 
 * sob os termos da Licença Pública Geral GNU, conforme publicada pela 
 * Free Software Foundation, na versão 3 da licença, ou (a seu critério) 
 * qualquer versão posterior.
 *
 * Este programa é distribuído na esperança de que seja útil, 
 * mas SEM QUALQUER GARANTIA; sem mesmo a garantia implícita de 
 * COMERCIALIZAÇÃO ou ADEQUAÇÃO A UM DETERMINADO FIM. 
 * Consulte a Licença Pública Geral GNU para mais detalhes.
 *
 * Você deve ter recebido uma cópia da Licença Pública Geral GNU 
 * junto com este programa. Se não, veja <https://www.gnu.org/licenses/>.
 */

/**
 * @defgroup Joystick Joystick Analógico
 * @brief Módulo responsável pelo controle e leitura de um joystick analógico.
 * @{
 */

 /**
 * @def UINT8_T_CONSTANT(num)
 * @brief Macro para converter um número em um valor do tipo uint8_t.
 *
 * Essa macro garante que o número passado seja tratado como um `uint8_t`,
 * evitando problemas de tipo em operações bit a bit e comparação de constantes.
 *
 * @param num O número a ser convertido.
 */
#define UINT8_T_CONSTANT(num) ((uint8_t) (num))

/**
 * @def ADC_GPIO_26
 * @brief Define o GPIO 26 como uma entrada ADC válida.
 */
#define ADC_GPIO_26 UINT8_T_CONSTANT(26)

/**
 * @def ADC_GPIO_27
 * @brief Define o GPIO 27 como uma entrada ADC válida.
 */
#define ADC_GPIO_27 UINT8_T_CONSTANT(27)

/**
 * @def ADC_GPIO_28
 * @brief Define o GPIO 28 como uma entrada ADC válida.
 */
#define ADC_GPIO_28 UINT8_T_CONSTANT(28)

/**
 * @def ADC_GPIO_29
 * @brief Define o GPIO 29 como uma entrada ADC válida.
 */
#define ADC_GPIO_29 UINT8_T_CONSTANT(29)

/**
 * @def ADC_CHANNEL_1
 * @brief Define o canal ADC 1 correspondente a um GPIO configurado como entrada analógica.
 */
#define ADC_CHANNEL_1 UINT8_T_CONSTANT(0)

/**
 * @def ADC_CHANNEL_2
 * @brief Define o canal ADC 2 correspondente a um GPIO configurado como entrada analógica.
 */
#define ADC_CHANNEL_2 UINT8_T_CONSTANT(1)

/**
 * @def ADC_CHANNEL_3
 * @brief Define o canal ADC 3 correspondente a um GPIO configurado como entrada analógica.
 */
#define ADC_CHANNEL_3 UINT8_T_CONSTANT(2)

/**
 * @def ADC_CHANNEL_4
 * @brief Define o canal ADC 4 correspondente a um GPIO configurado como entrada analógica.
 */
#define ADC_CHANNEL_4 UINT8_T_CONSTANT(3)

/**
 * @brief Estrutura que representa um joystick analógico.
 *
 * Essa estrutura armazena os pinos de entrada analógica para os eixos X e Y, 
 * o pino do botão do joystick e o valor da zona morta (deadzone).
 */
typedef struct
{
    uint8_t channel_x;       /**< Canal ADC correspondente ao eixo X. */
    uint8_t channel_y;       /**< Canal ADC correspondente ao eixo Y. */
    uint8_t joy_push_button; /**< Pino GPIO do botão do joystick. */
    uint8_t deadzone;        /**< Valor da zona morta para evitar ruídos no centro. */
} joystick_t;

/**
 * @brief Inicializa o joystick, configurando os pinos ADC e o botão de push.
 *
 * Esta função inicializa o ADC para leitura dos eixos X e Y do joystick, 
 * configura os pinos `joy_vrx` e `joy_vry` como entradas analógicas 
 * e configura o `joy_pbutton` como entrada digital com pull-up ativado. 
 * Além disso, inicializa a estrutura `joystick_t`.
 *
 * @note Esta função deve ser chamada antes de realizar leituras do joystick.
 * @warning Se os pinos forem configurados incorretamente, o joystick pode não responder corretamente.
 *
 * @param[out] joy Ponteiro para a estrutura do joystick que será inicializada.
 * @param[in] joy_vrx Canal ADC correspondente ao eixo X.
 * @param[in] joy_vry Canal ADC correspondente ao eixo Y.
 * @param[in] joy_pbutton Pino GPIO do botão do joystick.
 */
void joystick_init_all(joystick_t *joy, uint8_t joy_vrx, uint8_t joy_vry, uint8_t joy_pbutton, uint8_t deadzone);

/**
 * @brief Obtém o valor do eixo X do joystick.
 *
 * Esta função realiza a leitura do canal ADC correspondente ao eixo X do joystick.
 * O valor pode ser utilizado para normalização e controle gráfico.
 *
 * @note O ADC deve estar corretamente inicializado antes da leitura.
 *
 * @param[in] joy Ponteiro para a estrutura do joystick.
 * @return Valor do ADC correspondente ao eixo X (exemplo: 0 - 4095 em ADC de 12 bits).
 */
uint16_t joystick_get_x(const joystick_t *joy);

/**
 * @brief Obtém o valor do eixo Y do joystick.
 *
 * Esta função realiza a leitura do canal ADC correspondente ao eixo Y do joystick.
 *
 * @note O ADC deve estar corretamente inicializado antes da leitura.
 *
 * @param[in] joy Ponteiro para a estrutura do joystick.
 * @return Valor do ADC correspondente ao eixo Y (exemplo: 0 - 4095 em ADC de 12 bits).
 */
uint16_t joystick_get_y(const joystick_t *joy);

/**
 * @brief Verifica se o botão do joystick está pressionado.
 *
 * Esta função lê o estado do pino digital associado ao botão do joystick.
 * Se o botão estiver pressionado, retorna `true`, caso contrário, retorna `false`.
 *
 * @note O pino do botão deve ser configurado corretamente como entrada digital com pull-up ativado.
 *
 * @param[in] joy Ponteiro para a estrutura do joystick.
 * @return `true` se o botão estiver pressionado, `false` caso contrário.
 */
bool joystick_get_button(const joystick_t *joy);

/**
 * @brief Verifica se o joystick está se movendo além da deadzone.
 *
 * Esta função compara o valor lido dos eixos X e Y com a deadzone definida
 * para determinar se há um movimento significativo.
 *
 * @note A deadzone deve estar configurada corretamente para evitar detecção de ruídos.
 * @warning Se a deadzone for muito alta, pequenos movimentos podem ser ignorados.
 *
 * @param[in] joy Ponteiro para a estrutura do joystick.
 * @return `true` se o joystick estiver fora da deadzone, `false` caso contrário.
 */
bool joystick_is_moving(const joystick_t *joy);

/**
 * @brief Define o valor da zona morta (deadzone) do joystick.
 *
 * A zona morta é a faixa de valores ao redor do centro onde pequenas variações
 * de posição não são consideradas como movimento. Isso evita ruídos indesejados.
 *
 * @note O valor da deadzone deve ser ajustado de acordo com a sensibilidade desejada.
 *
 * @param[in,out] joy Ponteiro para a estrutura do joystick.
 * @param[in] deadzone_value Novo valor da zona morta.
 */
void joystick_set_deadzone(joystick_t *joy, uint8_t deadzone_value);

/** @} */ // Fim do grupo "Joystick"

#endif // JOYSTICK_H
