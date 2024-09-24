#ifndef LEDS_H
#define LEDS_H

#include <stdbool.h>
#include <stdint.h>

#include "ledc_controller.h"

void leds_begin();
void leds_deinit();
void leds_on();
void leds_off();
void leds_set_brightnes();
void led_stop();
void leds_on_g();
void leds_on_y();
void leds_on_o();
void leds_on_r();
void leds_notification(uint8_t pulses, uint16_t on_time, uint16_t off_time);
void leds_rgb_on();
#endif  // LEDS_H
