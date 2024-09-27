#include "vaccination.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "badge_pairing.h"
#include "general_screens.h"
#include "infection.h"
#include "infection_scenes.h"
#include "menus_module.h"
#include "oled_screen.h"

static vaccination_ctx_t* ctx = NULL;

void vaccination_exit();

static void show_scaning_dots(uint8_t x, uint8_t page) {
  static uint8_t dots = 0;
  dots = ++dots > 3 ? 0 : dots;
  for (int i = 0; i < 3; i++) {
    oled_screen_display_text(i < dots ? "." : "", x + (i * 8), page,
                             OLED_DISPLAY_NORMAL);
  }
}

static void show_waiting_screen() {
  genera_screen_display_notify_information(
      "Buscando", ctx->role ? "Receptor" : "Vacunador");
  show_scaning_dots(52, 3);
}

static void vaccination_input_cb(uint8_t button_name, uint8_t button_event) {
  if (button_event != BUTTON_DOWN) {
    return;
  }
  switch (button_name) {
    case BUTTON_LEFT:
      vaccination_exit();
      if (infection_get_patient_state() >= INFECTED && !CREATOR) {
        infection_scenes_vaccines_receiver_menu();
      } else {
        infection_scenes_vaccines_builder_menu();
      }
      break;
    default:
      break;
  }
}

static void vaccination_task() {
  while (ctx != NULL) {
    show_waiting_screen();
    vTaskDelay(pdMS_TO_TICKS(200));
  }
  vTaskDelete(NULL);
}

void vaccination_exit() {
  if (ctx == NULL) {
    return;
  }
  bool role = ctx->role;
  free(ctx);
  ctx = NULL;
  menus_module_set_app_state(false, NULL);
  badge_pairing_set_callbacks(NULL, NULL, NULL);
  badge_pairing_deinit();
}

void vaccination_begin() {
  badge_pairing_set_callbacks(send_vaccine_req_cmd, NULL, NULL);
  badge_pairing_init();

  menus_module_set_app_state(true, vaccination_input_cb);
  ctx = calloc(1, sizeof(vaccination_ctx_t));
  ctx->role = infection_get_patient_state() >= INFECTED ? RECIPIENT | CREATOR
                                                        : VACCINATOR;
  if (ctx->role) {
    badge_pairing_set_blue_team();
  } else {
    badge_pairing_set_red_team();
  }
  ctx->is_task_running = true;
  xTaskCreate(vaccination_task, "vaccination_task", 2048, NULL, 10, NULL);
}