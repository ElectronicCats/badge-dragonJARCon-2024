#include <stdio.h>
#include "coroutine.h"
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

typedef struct {
  uint8_t pulses;
  uint16_t on_time;
  uint16_t off_time;
} leds_notification_ctx;

static leds_notification_ctx leds_ctx = {0, 0, 0};

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

void leds_notification_coroutine() {
  for (uint8_t i = 0; i < leds_ctx.pulses; i++) {
    leds_on();
    vTaskDelay(pdMS_TO_TICKS(leds_ctx.on_time));
    leds_off();
    vTaskDelay(pdMS_TO_TICKS(leds_ctx.off_time));
  }
  vTaskDelete(NULL);
}

void leds_notification(uint8_t pulses, uint16_t on_time, uint16_t off_time) {
  leds_ctx.pulses = pulses;
  leds_ctx.on_time = on_time;
  leds_ctx.off_time = off_time;
  xTaskCreate(leds_notification_coroutine, "leds_notification_coroutine", 2048,
              NULL, 5, NULL);
}

void leds_rgb_on() {
  led_controller_led_off(led_r);
  led_controller_led_on(led_g);
  led_controller_led_on(led_b);
}