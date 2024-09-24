#include <stdio.h>
#include "driver/gpio.h"
#include "esp_err.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "ledc_controller_d.h"
#include "leds.h"

#define LED_G_1 GPIO_NUM_4
#define LED_G_2 GPIO_NUM_18
#define LED_Y_1 GPIO_NUM_19
#define LED_Y_2 GPIO_NUM_20
#define LED_O_1 GPIO_NUM_21
#define LED_O_2 GPIO_NUM_22
#define LED_R_1 GPIO_NUM_23
#define LED_R_2 GPIO_NUM_15

#define LED_ER   GPIO_NUM_11
#define LED_EG   GPIO_NUM_2
#define LED_EB   GPIO_NUM_3
#define LED_ER_C LEDC_CHANNEL_0
#define LED_EG_C LEDC_CHANNEL_1
#define LED_EB_C LEDC_CHANNEL_2

static led_t *led_r, *led_g, *led_b;

void leds_begin() {
  gpio_reset_pin(LED_G_1);
  gpio_set_direction(LED_G_1, GPIO_MODE_OUTPUT);
  gpio_reset_pin(LED_G_2);
  gpio_set_direction(LED_G_2, GPIO_MODE_OUTPUT);
  gpio_reset_pin(LED_Y_1);
  gpio_set_direction(LED_Y_1, GPIO_MODE_OUTPUT);
  gpio_reset_pin(LED_Y_2);
  gpio_set_direction(LED_Y_2, GPIO_MODE_OUTPUT);
  gpio_reset_pin(LED_O_1);
  gpio_set_direction(LED_O_1, GPIO_MODE_OUTPUT);
  gpio_reset_pin(LED_O_2);
  gpio_set_direction(LED_O_2, GPIO_MODE_OUTPUT);
  gpio_reset_pin(LED_R_1);
  gpio_set_direction(LED_R_1, GPIO_MODE_OUTPUT);
  gpio_reset_pin(LED_R_2);
  gpio_set_direction(LED_R_2, GPIO_MODE_OUTPUT);

  // RGB
  led_r = (led_t*) malloc(sizeof(led_t));
  led_g = (led_t*) malloc(sizeof(led_t));
  led_b = (led_t*) malloc(sizeof(led_t));
  *led_r = led_controller_led_new(LED_ER, LED_ER_C);
  *led_g = led_controller_led_new(LED_EG, LED_EG_C);
  *led_b = led_controller_led_new(LED_EB, LED_EB_C);
  led_controller_led_init(led_r);
  led_controller_led_init(led_g);
  led_controller_led_init(led_b);

  leds_on();
}

void leds_on_g() {
  gpio_set_level(LED_G_1, 1);
  gpio_set_level(LED_G_2, 1);
}

void leds_on_y() {
  gpio_set_level(LED_Y_1, 1);
  gpio_set_level(LED_Y_2, 1);
}

void leds_on_o() {
  gpio_set_level(LED_O_1, 1);
  gpio_set_level(LED_O_2, 1);
}

void leds_on_r() {
  gpio_set_level(LED_R_1, 1);
  gpio_set_level(LED_R_2, 1);
}

void leds_on() {
  gpio_set_level(LED_G_1, 1);
  gpio_set_level(LED_G_2, 1);
  gpio_set_level(LED_Y_1, 1);
  gpio_set_level(LED_Y_2, 1);
  gpio_set_level(LED_O_1, 1);
  gpio_set_level(LED_O_2, 1);
  gpio_set_level(LED_R_1, 1);
  gpio_set_level(LED_R_2, 1);
}

void leds_off() {
  gpio_set_level(LED_G_1, 0);
  gpio_set_level(LED_G_2, 0);
  gpio_set_level(LED_Y_1, 0);
  gpio_set_level(LED_Y_2, 0);
  gpio_set_level(LED_O_1, 0);
  gpio_set_level(LED_O_2, 0);
  gpio_set_level(LED_R_1, 0);
  gpio_set_level(LED_R_2, 0);
}

void leds_notification() {
  leds_on();
  vTaskDelay(500 / portTICK_PERIOD_MS);
  leds_off();
  vTaskDelay(250 / portTICK_PERIOD_MS);
  leds_on();
  vTaskDelay(500 / portTICK_PERIOD_MS);
  leds_off();
  vTaskDelay(250 / portTICK_PERIOD_MS);
  leds_on();
  vTaskDelay(500 / portTICK_PERIOD_MS);
  leds_off();
}

void leds_rgb_on() {
  led_controller_led_off(led_r);
  led_controller_led_on(led_g);
  led_controller_led_on(led_b);
}