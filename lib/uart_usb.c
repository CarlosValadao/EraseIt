#include "uart_usb.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>


/**
 * @brief Lê um único caractere da comunicação serial USB de forma bloqueante.
 * 
 * @return O caractere recebido via UART.
 * 
 * @note Esta função aguarda até que um caractere seja recebido via stdin (entrada serial).
 */
char uart_usb_blocking_getc()
{
    char recvd_char;
    scanf("%c", &recvd_char); // Bloqueia a execução até que um caractere seja recebido
    fflush(stdin); // Limpa o buffer de entrada para evitar leitura de dados residuais
    return recvd_char; // Retorna o caractere recebido
}

/**
 * @brief Lê uma string completa da comunicação serial USB de forma bloqueante.
 * 
 * @return Ponteiro para a string recebida via UART. O chamador é responsável por liberar a memória alocada.
 * 
 * @note A função aloca dinamicamente um buffer para armazenar a string recebida.
 *       O tamanho do buffer é limitado a 5 caracteres + '\0' (terminador nulo).
 */
char *uart_usb_blocking_gets()
{
    uint8_t buffer_size = 5; // Define o tamanho do buffer de entrada (5 caracteres + 1 para o terminador nulo)
    char *buffer = malloc(sizeof(char) * buffer_size + 1); // Aloca memória para a string
    fgets(buffer, buffer_size + 1, stdin); // Lê até 5 caracteres da entrada serial
    fflush(stdin); // Limpa o buffer de entrada para evitar leitura de dados indesejados
    return buffer; // Retorna o ponteiro para a string recebida
}