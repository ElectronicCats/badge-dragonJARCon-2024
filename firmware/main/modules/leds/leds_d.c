#include "leds_d.h"
#include <string.h>
#include "coroutine.h"
#include "driver/ledc.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "ledc_controller_d.h"
#include "leds_d.h"
#include "preferences.h"

#define LED_G_1 GPIO_NUM_4
#define LED_G_2 GPIO_NUM_18
#define LED_Y_1 GPIO_NUM_19
#define LED_Y_2 GPIO_NUM_20
#define LED_O_1 GPIO_NUM_21
#define LED_O_2 GPIO_NUM_22
#define LED_R_1 GPIO_NUM_23
#define LED_R_2 GPIO_NUM_15

#define LED_G_C    LEDC_CHANNEL_0
#define LED_Y_C    LEDC_CHANNEL_1
#define LED_O_C    LEDC_CHANNEL_2
#define LED_R_C    LEDC_CHANNEL_3
#define LEDC_TIMER LEDC_TIMER_0

static const uint8_t led_chs[] = {LED_R_C, LED_O_C, LED_Y_C, LED_G_C};

static const uint8_t led_pins[] = {LED_R_1, LED_R_2, LED_O_1, LED_O_2,
                                   LED_Y_1, LED_Y_2, LED_G_1, LED_G_2};
static led_t* led_ptrs[8];

void leds_begin() {
  for (uint8_t i = 0; i < LEDS_NUM; i++) {
    led_ptrs[i] = malloc(sizeof(led_t));
    *led_ptrs[i] = led_controller_led_new(led_pins[i], led_chs[i / 2]);
    led_controller_led_init(led_ptrs[i]);
  }
}

void leds_deinit() {
  for (uint8_t i = 0; i < LEDS_NUM; i++) {
    if (!led_ptrs[i]) {
      return;
    }
    led_controller_led_deinit(led_ptrs[i]);
    free(led_ptrs[i]);
    led_ptrs[i] = NULL;
  }
}

void leds_on() {
  for (uint8_t i = 0; i < LEDS_NUM; i++) {
    led_controller_led_on(led_ptrs[i]);
  }
}

void leds_off() {
  for (uint8_t i = 0; i < LEDS_NUM; i++) {
    led_controller_led_off(led_ptrs[i]);
  }
}

void leds_on_g() {
  leds_off();
  led_controller_led_off(led_ptrs[LED_G_1]);
  led_controller_led_off(led_ptrs[LED_G_2]);
}

void leds_on_y() {
  leds_off();
  led_controller_led_off(led_ptrs[LED_Y_1]);
  led_controller_led_off(led_ptrs[LED_Y_2]);
}

void leds_on_o() {
  leds_off();
  led_controller_led_off(led_ptrs[LED_O_1]);
  led_controller_led_off(led_ptrs[LED_O_2]);
}

void leds_on_r() {
  leds_off();
  led_controller_led_off(led_ptrs[LED_R_1]);
  led_controller_led_off(led_ptrs[LED_R_2]);
}

static void leds_notification_coroutine() {
  leds_off();
  leds_on();
  vTaskDelay(1000 / portTICK_PERIOD_MS);
  leds_off();
  vTaskDelay(500 / portTICK_PERIOD_MS);
  leds_on();
  vTaskDelay(1000 / portTICK_PERIOD_MS);
  leds_off();
  vTaskDelay(500 / portTICK_PERIOD_MS);
  leds_on();
  vTaskDelay(1000 / portTICK_PERIOD_MS);
  leds_off();
  vTaskDelay(500 / portTICK_PERIOD_MS);
  leds_on();
  vTaskDelay(1000 / portTICK_PERIOD_MS);
  leds_off();
  vTaskDelete(NULL);
}

void leds_notification() {
  start_coroutine(leds_notification_coroutine, NULL);
}

void leds_set_brightness(leds_enum led, uint8_t brightness) {
  led_controller_set_duty(led_ptrs[led], brightness);
}

void led_stop(leds_enum led) {
  led_controller_stop_any_effect(led_ptrs[led]);
}