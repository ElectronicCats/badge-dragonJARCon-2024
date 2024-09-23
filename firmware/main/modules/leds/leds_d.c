#include "leds.h"
#include <string.h>
#include "driver/ledc.h"
#include "ledc_controller_d.h"
#include "preferences.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#define LED_G_1           GPIO_NUM_4
#define LED_G_2           GPIO_NUM_18
#define LED_Y_1           GPIO_NUM_19
#define LED_Y_2           GPIO_NUM_20
#define LED_O_1           GPIO_NUM_21
#define LED_O_2           GPIO_NUM_22
#define LED_R_1           GPIO_NUM_23
#define LED_R_2           GPIO_NUM_15

#define LED_G_C  LEDC_CHANNEL_0
#define LED_Y_C LEDC_CHANNEL_1
#define LED_O_C LEDC_CHANNEL_2
#define LED_R_C LEDC_CHANNEL_3
#define LEDC_TIMER        LEDC_TIMER_0

#define LED_R GPIO_NUM_11
#define LED_G GPIO_NUM_2
#define LED_B GPIO_NUM_3
#define LED_RGB_R_C LEDC_CHANNEL_4
#define LED_RGB_G_C LEDC_CHANNEL_5
#define LED_RGB_B_C LEDC_CHANNEL_5

static led_t *led_g_x, *led_g_y, *led_y_x, *led_y_y, *led_o_x, *led_o_y, *led_r_x, *led_r_y, *led_r, *led_g, *led_b;

void leds_begin() {
  led_g_x = (led_t*) malloc(sizeof(led_t));
  led_g_y = (led_t*) malloc(sizeof(led_t));
  led_y_x = (led_t*) malloc(sizeof(led_t));
  led_y_y = (led_t*) malloc(sizeof(led_t));
  led_o_x = (led_t*) malloc(sizeof(led_t));
  led_o_y = (led_t*) malloc(sizeof(led_t));
  led_r_x = (led_t*) malloc(sizeof(led_t));
  led_r_y = (led_t*) malloc(sizeof(led_t));
  // RGB
  led_r = (led_t*) malloc(sizeof(led_t));
  led_g = (led_t*) malloc(sizeof(led_t));
  led_b = (led_t*) malloc(sizeof(led_t));

  *led_g_x = led_controller_led_new(LED_G_1, LED_G_C);
  *led_g_y = led_controller_led_new(LED_G_2, LED_G_C);
  *led_y_x = led_controller_led_new(LED_Y_1, LED_Y_C);
  *led_y_y = led_controller_led_new(LED_Y_2, LED_Y_C);
  *led_o_x = led_controller_led_new(LED_O_1, LED_O_C);
  *led_o_y = led_controller_led_new(LED_O_2, LED_O_C);
  *led_r_x = led_controller_led_new(LED_R_1, LED_R_C);
  *led_r_y = led_controller_led_new(LED_R_2, LED_R_C);
  // RGB
  *led_r = led_controller_led_new(LED_R, LED_RGB_R_C);
  *led_g = led_controller_led_new(LED_G, LED_RGB_G_C);
  *led_b = led_controller_led_new(LED_B, LED_RGB_B_C);



  led_controller_led_init(led_g_x);
  led_controller_led_init(led_g_y);
  led_controller_led_init(led_y_x);
  led_controller_led_init(led_y_y);
  led_controller_led_init(led_o_x);
  led_controller_led_init(led_o_y);
  led_controller_led_init(led_r_x);
  led_controller_led_init(led_r_y);
  // RGB
  led_controller_led_init(led_r);
  led_controller_led_init(led_g);
  led_controller_led_init(led_b);
}

void leds_deinit() {
  if(!led_g_x || !led_g_y || !led_y_x || !led_y_y || !led_o_x || !led_o_y || !led_r_x || !led_r_y){
    return;
  }

  led_controller_led_deinit(led_g_x);
  led_controller_led_deinit(led_g_y);
  led_controller_led_deinit(led_y_x);
  led_controller_led_deinit(led_y_y);
  led_controller_led_deinit(led_o_x);
  led_controller_led_deinit(led_o_y);
  led_controller_led_deinit(led_r_x);
  led_controller_led_deinit(led_r_y);
  // RGB
  led_controller_led_deinit(led_r);
  led_controller_led_deinit(led_g);
  led_controller_led_deinit(led_b);

  free(led_g_x);
  free(led_g_y);
  free(led_y_x);
  free(led_y_y);
  free(led_o_x);
  free(led_o_y);
  free(led_r_x);
  free(led_r_y);
  // RGB
  free(led_r);
  free(led_g);
  free(led_b);

  led_g_x = NULL;
  led_g_y = NULL;
  led_y_x = NULL;
  led_y_y = NULL;
  led_o_x = NULL;
  led_o_y = NULL;
  led_r_x = NULL;
  led_r_y = NULL;
  // RGB
  led_r = NULL;
  led_g = NULL;
  led_b = NULL;
}

void leds_on() {
  led_controller_led_on(led_g_x);
  led_controller_led_on(led_g_y);
  led_controller_led_on(led_y_x);
  led_controller_led_on(led_y_y);
  led_controller_led_on(led_o_x);
  led_controller_led_on(led_o_y);
  led_controller_led_on(led_r_x);
  led_controller_led_on(led_r_y);
}

void leds_off() {
  led_stop(0);
  led_controller_led_off(led_g_x);
  led_controller_led_off(led_g_y);
  led_controller_led_off(led_y_x);
  led_controller_led_off(led_y_y);
  led_controller_led_off(led_o_x);
  led_controller_led_off(led_o_y);
  led_controller_led_off(led_r_x);
  led_controller_led_off(led_r_y);
}

void leds_on_g(){
  leds_off();
  led_controller_led_on(led_g_x);
  led_controller_led_on(led_g_y);
  ESP_LOGI("LED", "led_g_x");
}

void leds_on_y(){
  leds_off();
  led_controller_led_on(led_y_x);
  led_controller_led_on(led_y_y);
  ESP_LOGI("LED", "led_y_x");
}

void leds_on_o(){
  leds_off();
  led_controller_led_on(led_o_x);
  led_controller_led_on(led_o_y);
  ESP_LOGI("LED", "led_o_x");
}

void leds_on_r(){
  leds_off();
  led_controller_led_on(led_r_x);
  led_controller_led_on(led_r_y);
  ESP_LOGI("LED", "led_r_x");
}

void leds_notification(){
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
}

void leds_set_brightness(uint8_t led, uint8_t brightness) {
  led_controller_set_duty(led_g_x, brightness);
}

void led_stop(uint8_t led) {
  led_controller_stop_any_effect(led_g_x);
  led_controller_stop_any_effect(led_g_y);
  led_controller_stop_any_effect(led_y_x);
  led_controller_stop_any_effect(led_y_y);
  led_controller_stop_any_effect(led_o_x);
  led_controller_stop_any_effect(led_o_y);
  led_controller_stop_any_effect(led_r_x);
  led_controller_stop_any_effect(led_r_y);
}


void leds_rgb_on(){
  led_controller_led_off(led_r);
  led_controller_led_on(led_g);
  led_controller_led_on(led_b);
}