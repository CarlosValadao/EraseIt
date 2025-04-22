/**
 * @file uart_usb.h
 * @brief Este arquivo contém declarações de funções para comunicação UART via USB.
 * 
 * @note As funções fornecem uma interface para recebimento de caracteres e strings
 *       de forma síncrona (bloqueante), utilizando a comunicação serial UART.
 * 
 * @author Carlos Valadão
 * @date 09/02/2025
 */

 #ifndef UART_USB_H
 #define UART_USB_H
 
 /**
  * @brief Lê um único caractere da comunicação serial USB de forma bloqueante.
  * 
  * @return O caractere recebido via UART.
  */
 char uart_usb_blocking_getc();
 
 /**
  * @brief Lê uma string completa da comunicação serial USB de forma bloqueante.
  * 
  * @return Ponteiro para a string recebida via UART.
  */
 char *uart_usb_blocking_gets();
 
 #endif // UART_USB_H
 