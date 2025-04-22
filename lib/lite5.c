#include "lite5.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "ws2812b_motion.h"
#include "ws2812b_definitions.h"
#include "pico/stdlib.h"

/**
 * @brief Variável global que armazena o último comando executado.
 */
Command last_executed_cmd = EMPTY_CMD;

/**
 * @brief Converte um caractere de cor para um valor numérico correspondente.
 * 
 * @param color Caractere representando a cor ('r' para vermelho, 'g' para verde, etc.).
 * @return Valor numérico correspondente à cor, usado na exibição dos LEDs.
 */
static uint8_t hash_color(uint8_t color)
{
    switch (color) {
        case 'r': return 0;  // Red
        case 'g': return 1;  // Green
        case 'b': return 2;  // Blue
        case 'y': return 3;  // Yellow
        case 'p': return 4;  // Purple
        case 'w': return 5;  // White
        case 'm': return 6;  // Marines
        default: return 0;   // Retorno padrão caso a cor não seja reconhecida
    }
}

/**
 * @brief Interpreta uma string de entrada e converte em um comando válido.
 *
 * @param input Ponteiro para a string contendo o comando.
 * @return Estrutura `Command` preenchida com os valores extraídos. Retorna um comando vazio
 *         se o comprimento da string for inválido.
 */
Command parse_command(const char *input)
{
    // Define um comando vazio para caso de erro
    Command cmd = EMPTY_CMD;
    
    // Verifica se a string de entrada tem um tamanho válido
    if(strlen(input) > 5) return cmd;
    
    // Converte os parâmetros da string para um comando válido
    cmd.pattern = (input[0] - 48); // Converte o primeiro caractere para um número (0-9)
    cmd.color = hash_color(input[1]); // Converte a cor usando hash_color()
    cmd.intensity = atoi(&input[2]); // Converte os caracteres restantes para um número inteiro
    
    return cmd;
}

/**
 * @brief Executa um comando na matriz de LEDs WS2812B.
 *
 * @param ws Ponteiro para a estrutura da matriz de LEDs WS2812B.
 * @param cmd Ponteiro para a estrutura `Command` contendo os valores a serem aplicados.
 * 
 * @note Se já houver um comando em execução, a transição entre os estados é suavizada.
 *       Caso contrário, o comando é diretamente aplicado à matriz de LEDs.
 */
void execute_command(const ws2812b_t *ws, const Command *cmd)
{
    if(last_executed_cmd.intensity != 0) {
        // Executa uma transição suave entre comandos anteriores e o novo comando
        ws2812b_motion_transition(ws, &last_executed_cmd, cmd);
    } else {
        // Aplica o novo comando diretamente à matriz de LEDs
        ws2812b_draw(ws, NUMERIC_GLYPHS[cmd->pattern], cmd->color, cmd->intensity);
    }
    
    // Armazena o comando como último comando executado
    last_executed_cmd = *cmd;
}

/**
 * @brief Verifica se um comando é vazio.
 *
 * @param cmd Ponteiro para a estrutura `Command` a ser analisada.
 * @return `true` se o comando for válido, `false` se for um comando vazio.
 */
bool lite5_is_empty_cmd(const Command *cmd)
{
    return (bool) (cmd->pattern != 0 && cmd->color != 0 && cmd->intensity != 0);
}
