#include "leds_d.h"
#include <string.h>
#include "coroutine.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "leds_d.h"
#include "preferences.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#define LED_G_1  GPIO_NUM_4
#define LED_G_2  GPIO_NUM_18
#define LED_Y_1  GPIO_NUM_19
#define LED_Y_2  GPIO_NUM_20
#define LED_O_1  GPIO_NUM_21
#define LED_O_2  GPIO_NUM_22
#define LED_R_1  GPIO_NUM_23
#define LED_R_2  GPIO_NUM_15
#define LEDS_NUM 8

static const gpio_num_t led_pins[] = {LED_R_2, LED_R_1, LED_O_2, LED_O_1,
                                      LED_Y_2, LED_Y_1, LED_G_2, LED_G_1};

void leds_begin() {
  for (uint8_t i = 0; i < LEDS_NUM; i++) {
    gpio_set_direction(led_pins[i], GPIO_MODE_OUTPUT);
    gpio_set_level(led_pins[i], 0);
  }
}

void leds_deinit() {
  for (uint8_t i = 0; i < LEDS_NUM; i++) {
    gpio_set_level(led_pins[i], 0);
    gpio_set_direction(led_pins[i], GPIO_MODE_DISABLE);
  }
}

void leds_on() {
  for (uint8_t i = 0; i < LEDS_NUM; i++) {
    gpio_set_level(led_pins[i], 1);
  }
}

void leds_off() {
  for (uint8_t i = 0; i < LEDS_NUM; i++) {
    gpio_set_level(led_pins[i], 0);
  }
}

void leds_on_g() {
  leds_off();
  gpio_set_level(led_pins[LED_G_1], 1);
  gpio_set_level(led_pins[LED_G_2], 1);
}

void leds_on_y() {
  leds_off();
  gpio_set_level(led_pins[LED_Y_1], 1);
  gpio_set_level(led_pins[LED_Y_2], 1);
}

void leds_on_o() {
  leds_off();
  gpio_set_level(led_pins[LED_O_1], 1);
  gpio_set_level(led_pins[LED_O_2], 1);
}

void leds_on_r() {
  leds_off();
  gpio_set_level(led_pins[LED_R_1], 1);
  gpio_set_level(led_pins[LED_R_2], 1);
}

static void leds_notification_coroutine() {
  for (uint8_t i = 0; i < LEDS_NUM; i++) {
    leds_off();
    gpio_set_level(led_pins[i], 1);
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
  vTaskDelete(NULL);
}

void leds_notification() {
  start_coroutine(leds_notification_coroutine, NULL);
}

void leds_set_brightness() {}

void led_stop() {}