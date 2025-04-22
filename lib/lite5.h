/**
 * @file lite5.h - Light Terminal 5
 * @brief Este arquivo contém definições e declarações para manipulação de comandos
 *        provenientes do serial monitor e que serão impressos na matriz de LEDs WS2812B.
 *
 * @note As funções permitem interpretar e executar comandos relacionados a padrões,
 *       cores e intensidades de LEDs.
 *
 * @author Carlos Valadão
 * @date 09/02/2025
 */

 #ifndef LITE5_H
 #define LITE5_H
 
 #include <stdint.h>
 #include "ws2812b.h"
 
 /**
  * @brief Macro que define um comando vazio.
  * 
  * @note Um comando vazio é representado por valores nulos para padrão, cor e intensidade.
  */
 #define EMPTY_CMD ((Command) {0, 0, 0})
 
 /**
  * @brief Estrutura para representar um comando de controle da matriz de LEDs WS2812B.
  */
 typedef struct {
     uint8_t pattern;   ///< Padrão do efeito aplicado aos LEDs
     uint8_t color;     ///< Cor associada ao comando
     uint8_t intensity; ///< Intensidade da cor aplicada aos LEDs
 } Command;
 
 /**
  * @brief Armazena o último comando executado.
  *
  * @note Esta variável é global e pode ser usada para verificar o último comando processado.
  */
 extern Command last_executed_cmd;
 
 /**
  * @brief Interpreta uma string de entrada e converte em um comando válido.
  * 
  * @param input Ponteiro para a string contendo o comando.
  * @return Estrutura `Command` preenchida com os valores extraídos.
  */
 Command parse_command(const char *input);
 
 /**
  * @brief Executa um comando na matriz de LEDs WS2812B.
  * 
  * @param ws Ponteiro para a estrutura da matriz de LEDs WS2812B.
  * @param cmd Ponteiro para a estrutura `Command` contendo os valores a serem aplicados.
  */
 void execute_command(const ws2812b_t *ws, const Command *cmd);
 
 /**
  * @brief Verifica se um comando é vazio.
  * 
  * @param cmd Ponteiro para a estrutura `Command` a ser analisada.
  * @return `true` se o comando for vazio, `false` caso contrário.
  */
 bool lite5_is_empty_cmd(const Command *cmd);
 
 #endif // LITE5_H
 