#ifndef LEDS_H
#define LEDS_H

#include <stdbool.h>
#include <stdint.h>

#include "ledc_controller.h"

typedef enum {
  LED_R_1,
  LED_R_2,
  LED_O_1,
  LED_O_2,
  LED_Y_1,
  LED_Y_2,
  LED_G_1,
  LED_G_2,
  LEDS_NUM
} leds_enum;

void leds_begin();
void leds_deinit();
void leds_on();
void leds_off();
void leds_set_brightness(leds_enum led, uint8_t brightness);
void led_stop(leds_enum led);
void leds_on_g();
void leds_on_y();
void leds_on_o();
void leds_on_r();
void leds_notification();

#endif  // LEDS_H
