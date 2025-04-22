#ifndef WS2812B_MOTION_H
#define WS2812B_MOTION_H

#include "lite5.h"
#include "ws2812b.h"
#include <stdint.h>

/** 
 * @file ws2812b_motion.h
 * @brief Este arquivo contém declarações de funções relacionadas à animação
 *        de uma matriz de LEDs WS2812B 5x5, permitindo a movimentação e os efeitos visuais nos LEDs.
 *        
 *        ***************
 *        *** ATENÇÃO ***
 *        ***************
 *       Todos os efeitos nesta matriz são realizados em um quadrado 3x3 de LEDs
 *       (no centro da matriz 5x5), conforme ilustrado abaixo:
 * 
 *           . # # # .
 *           . # # # .
 *           . # # # .
 *           . # # # .
 *           . # # # .   
 * 
 * @author Carlos Valadao
 * @date 23/01/2025
 */


/**
 * @brief Função que realiza um deslocamento do glyph para a esquerda.
 * 
 * Esta função move a matriz de LEDs (glyph) para a esquerda, criando um efeito de "deslizamento".
 * 
 * @param glyph Ponteiro para a matriz de LEDs (glyph) a ser movida.
 * @param shifted_glyph Ponteiro para a matriz de LEDs onde será armazenada a versão movida.
 */
void ws2812b_motion_shift_left(uint8_t *glyph, uint8_t *shifted_glyph);

/**
 * @brief Função que realiza um deslocamento do glyph para a direita.
 * 
 * Esta função move a matriz de LEDs (glyph) para a direita, criando um efeito de "deslizamento".
 * 
 * @param glyph Ponteiro para a matriz de LEDs (glyph) a ser movida.
 * @param shifted_glyph Ponteiro para a matriz de LEDs onde será armazenada a versão movida.
 */
void ws2812b_motion_shift_right(uint8_t *glyph, uint8_t *shifted_glyph);

/**
 * @brief Função que realiza um movimento de deslizamento do glyph para a esquerda.
 * 
 * O glyph é deslocado para a esquerda na matriz de LEDs, criando um efeito de movimento horizontal.
 * 
 * @param ws Ponteiro para a estrutura `ws2812b_t` que contém as configurações do WS2812B.
 * @param glyph Ponteiro para a matriz de LEDs (glyph) a ser movida.
 * @param color A cor a ser utilizada para o movimento.
 * @param intensity A intensidade da cor nos LEDs.
 */
void ws2812b_motion_slide_left(const ws2812b_t *ws, const uint8_t *glyph, uint8_t color, uint8_t intensity);

/**
 * @brief Função que realiza um movimento de deslizamento do glyph para a direita.
 * 
 * O glyph é deslocado para a direita na matriz de LEDs, criando um efeito de movimento horizontal.
 * 
 * @param ws Ponteiro para a estrutura `ws2812b_t` que contém as configurações do WS2812B.
 * @param glyph Ponteiro para a matriz de LEDs (glyph) a ser movida.
 * @param color A cor a ser utilizada para o movimento.
 * @param intensity A intensidade da cor nos LEDs.
 */
void ws2812b_motion_slide_right(const ws2812b_t *ws, const uint8_t *glyph, uint8_t color, uint8_t intensity);

/**
 * @brief Função que realiza um movimento de deslizamento do glyph para cima.
 * 
 * O glyph é deslocado para cima na matriz de LEDs, criando um efeito de movimento vertical.
 * 
 * @param ws Ponteiro para a estrutura `ws2812b_t` que contém as configurações do WS2812B.
 * @param glyph Ponteiro para a matriz de LEDs (glyph) a ser movida.
 * @param color A cor a ser utilizada para o movimento.
 * @param intensity A intensidade da cor nos LEDs.
 */

 void ws2812b_motion_transition(const ws2812b_t *ws, const Command *last_cmd, const Command *cmd);

#endif