#ifndef MLT8530_H
#define MLT8530_H

#include "pico/stdlib.h"

/** 
 * @file rgb.h
 * @brief Este arquivo contém declarações de funções e definições relacionadas a um
 *        buzzer conectado aos pinos GPIO
 *
 * @author Carlos Valadao
 * @date 17/01/2025
 */

#define BUZZER_PIN 21

#define buzzer_beep_default(duration, frequency) buzzer_beep(BUZZER_PIN, duration, frequency)
#define buzzer_init_default() buzzer_init(BUZZER_PIN)

void buzzer_init(uint8_t buzzer_pin);
void buzzer_beep(uint8_t buzzer_pin, uint16_t duration, uint16_t frequency);

#endif // MLT8530_H
