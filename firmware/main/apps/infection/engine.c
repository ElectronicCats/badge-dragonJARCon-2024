#include "engine.h"
#include "bitmaps_general.h"
#include "esp_random.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "infection_scenes.h"
#include "menus_module.h"
#include "screen_saver.h"
#include "virus.h"

#define ITEMOFFSET    1
#define SEQUENCE_SIZE 6

static uint8_t infection_probability = 10;  // 10%
static uint16_t infection_time = 30;
static bool combination_infect = false;

static void engine_infection_vaccine_init();
static void engine_handler_keyboard(uint8_t button_name, uint8_t button_event);
static void show_arrow(uint8_t idx, bool check);

static uint8_t current_item = 0;
static uint8_t keyboard_combination[SEQUENCE_SIZE];
static uint16_t keyboard_combination_user[SEQUENCE_SIZE];
static uint8_t keyboard_combination_index = 0;
static enum {
  VACCIN_MENU_HELP,
  VACCIN_MENU_CODE,
  VACCIN_MENU_RAND,
  VACCIN_MENU_COUNT
} vaccine_menu_item_e = VACCIN_MENU_CODE;
static char* vaccine_menu_items[VACCIN_MENU_COUNT] = {"Pistas", "Decifrar",
                                                      "Dados"};

static char* vaccine_help_items[41] = {
    "Un viejo sabio",      "te ha dejado un",    "mensaje secreto",
    "El mensaje lo ",      "encuentras mas",     "cerca de lo",
    "que piensas.",        "                ",   "Decifra el codigo",
    "del virus o",         "lanza los dados",    "para obtener la",
    "vacuna...",           "Pero cuidado",       "el virus se ",
    "propaga rapido",      "y el tiempo",        "se agota...",
    "Te sientes con ",     "suerte para",        "lanzar los dados?",
    "O         ",          "prefieres decifrar", "el codigo?",
    "Elige sabiamente...", "                  ", "                  ",
    "                  ",  "                  ", "                  ",
    "                  ",  "                  ", "                  ",
    "                  ",  "                  ", "                  ",
    "                  ",  "Compra una",         "Catsniffer y te",
    "damos el codigo",     "de la vacuna"};

static const general_menu_t vaccin_menu = {
    .menu_items = vaccine_menu_items,
    .menu_count = VACCIN_MENU_COUNT,
    .menu_level = GENERAL_TREE_APP_MENU,
};

static const general_menu_t vaccin_help_menu = {
    .menu_items = vaccine_help_items,
    .menu_count = 40,
    .menu_level = GENERAL_TREE_APP_INFORMATION,
};

static void engine_infection_vaccine_stop_timer() {
  combination_infect = false;
}

static void engine_infection_vaccine_start_timer() {
  while (combination_infect) {
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    if (!combination_infect) {
      break;
    }
    infection_time--;
    char infection_time_str[18];
    sprintf(infection_time_str, "Restante: %02d", infection_time);
    oled_screen_display_text(infection_time_str, 0, 0, OLED_DISPLAY_NORMAL);
    if (infection_time == 0) {
      engine_infection_vaccine_stop_timer();
      infection_get_infected();
    }
  }

  infection_time = 30;
  vTaskDelete(NULL);
}

static void module_reset_menu() {
  current_item = 0;
  general_register_menu(&vaccin_menu);
  general_screen_display_menu(current_item);
  menus_module_set_app_state(true, engine_handler_keyboard);
}

static void engine_infection_vaccine_decipher() {
  genera_screen_display_card_information("Decifra clave", "");
}

static void infected_dice_result(infection_ctx_t* ctx, uint8_t dice_roll) {
  char dice_roll_str[20];
  if (dice_roll == 20) {
    if (!(get_random_uint8() % 3)) {
      dice_roll = (esp_random() % 20) + 1;
    }
  }
  sprintf(dice_roll_str, "%d", dice_roll);
  if (dice_roll < 20) {
    sprintf(dice_roll_str, "%d", dice_roll);
    genera_screen_display_card_information(dice_roll_str, "Mala suerte");
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    sprintf(dice_roll_str, "%d segundos", dice_roll);
    genera_screen_display_card_information("Tiempo perdido", dice_roll_str);
    ctx->patient->remaining_time -= dice_roll;
    if (ctx->patient->remaining_time > LIFE_TIME) {
      ctx->patient->remaining_time = 0;
    }
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    infection_scenes_vaccines_receiver_menu();
  } else {
    genera_screen_display_card_information(dice_roll_str, "Salvado");
    ctx->patient->remaining_time += 3;
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    infection_get_vaccinated();
  }
}

static void get_random_vaccine(infection_ctx_t* ctx, uint8_t dice_roll) {
  char dice_roll_str[20];
  sprintf(dice_roll_str, "%d", dice_roll);
  uint8_t vaccine_idx = get_random_uint8() % 3;
  memcpy(ctx->vaccine, cures[vaccine_idx], sizeof(vaccine_t));
  genera_screen_display_card_information(dice_roll_str, "Felicidades");
  vTaskDelay(1500 / portTICK_PERIOD_MS);
  sprintf(dice_roll_str, "para %s", virus_str[vaccine_idx]);
  genera_screen_display_card_information("Ganaste: vacuna", dice_roll_str);
  vTaskDelay(1500 / portTICK_PERIOD_MS);
  genera_screen_display_card_information("Atento", "");
  vTaskDelay(1500 / portTICK_PERIOD_MS);
  genera_screen_display_card_information(VACCINE_COMP_STR_1,
                                         arn_str[ctx->vaccine->arn]);
  vTaskDelay(1500 / portTICK_PERIOD_MS);
  genera_screen_display_card_information(
      VACCINE_COMP_STR_2, viral_code_str[ctx->vaccine->viral_code]);
  vTaskDelay(1500 / portTICK_PERIOD_MS);
  genera_screen_display_card_information(
      VACCINE_COMP_STR_3, lipid_layer_str[ctx->vaccine->lipid_layer]);
  vTaskDelay(1500 / portTICK_PERIOD_MS);
}

static void healty_dice_result(infection_ctx_t* ctx, uint8_t dice_roll) {
  char dice_roll_str[20];
  sprintf(dice_roll_str, "%d", dice_roll);
  if (dice_roll < 10) {
    genera_screen_display_card_information(dice_roll_str, "Mala suerte");
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    genera_screen_display_card_information(dice_roll_str, "Infectado");
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    infection_get_infected();
  } else if (dice_roll < 20) {
    genera_screen_display_card_information(dice_roll_str, "Sin efecto");
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    infection_scenes_vaccines_builder_menu();
  } else {
    get_random_vaccine(ctx, dice_roll);
    infection_scenes_vaccines_builder_menu();
  }
}

void engine_infection_vaccine_dice() {
  infection_ctx_t* ctx = infection_get_context();
  genera_screen_display_card_information("Lanzando dados", "Objetivo: 20");
  vTaskDelay(1500 / portTICK_PERIOD_MS);
  uint8_t dice_roll = (esp_random() % 20) + 1;
  if (ctx->patient->state >= INFECTED) {
    infected_dice_result(ctx, dice_roll);
  } else {
    healty_dice_result(ctx, dice_roll);
  }
}

static void engine_infection_keyboard_combination_fail() {
  genera_screen_display_card_information("Error", "Combo incorrecto");
  vTaskDelay(pdMS_TO_TICKS(2000));
}

static void engine_infection_keyboard_combination_done() {
  genera_screen_display_card_information("Excelente", "Combo correcto");
  vTaskDelay(pdMS_TO_TICKS(2000));
  genera_screen_display_card_information("...", "Mantente alerta");
  vTaskDelay(pdMS_TO_TICKS(2000));
}

static void engine_handler_keyboard_combination(uint8_t button_name,
                                                uint8_t button_event) {
  if (button_event != BUTTON_PRESS_DOWN) {
    return;
  }
  if (keyboard_combination_index == 0) {
    genera_screen_display_card_information("", "");
  }
  keyboard_combination_user[keyboard_combination_index] = button_name;
  show_arrow(keyboard_combination_index, true);

  keyboard_combination_index++;
  if (keyboard_combination_index >= SEQUENCE_SIZE) {
    keyboard_combination_index = 0;
    for (int i = 0; i < SEQUENCE_SIZE; i++) {
      if (keyboard_combination[i] != keyboard_combination_user[i]) {
        engine_infection_vaccine_stop_timer();
        engine_infection_keyboard_combination_fail();
        infection_get_infected();
        return;
      }
    }
    engine_infection_vaccine_stop_timer();
    engine_infection_keyboard_combination_done();
    infection_set_inmunity_time();
    infection_set_patient_state(HEALTY);
    infection_scenes_main_menu();
    // general_register_menu(&vaccin_menu);
    // menus_module_set_app_state(true, engine_handler_keyboard);
    // general_screen_display_menu(current_item);
    return;
  }
}

static void engine_handler_keyboard(uint8_t button_name, uint8_t button_event) {
  if (button_event != BUTTON_PRESS_DOWN) {
    return;
  }
  switch (button_name) {
    case BUTTON_UP:
      current_item--;
      if (current_item == 0) {
        current_item = VACCIN_MENU_COUNT - 1;
      }

      general_screen_display_menu(current_item);
      break;
    case BUTTON_DOWN:
      current_item++;
      if (current_item >= VACCIN_MENU_COUNT) {
        current_item = 0;
      }
      general_screen_display_menu(current_item);
      break;
    case BUTTON_LEFT:
      break;
    case BUTTON_RIGHT:
      if (current_item == VACCIN_MENU_HELP) {
        current_item = 0;
        general_register_scrolling_menu(&vaccin_help_menu);
        general_screen_display_scrolling_text_handler(module_reset_menu);
      } else if (current_item == VACCIN_MENU_CODE) {
        current_item = 0;
        engine_infection_vaccine_decipher();
      } else if (current_item == VACCIN_MENU_RAND) {
        current_item = 0;
        engine_infection_vaccine_dice();
      }
      break;
    default:
      break;
  }
}

static void new_sequence() {
  for (uint8_t i = 0; i < SEQUENCE_SIZE; i++) {
    keyboard_combination[i] = get_random_uint8() % BUTTON_DOWN + 1;
  }
}

static void show_arrow(uint8_t idx, bool check) {
  bool correct = keyboard_combination[idx] != keyboard_combination_user[idx];
  if (!check) {
    correct = false;
  }
  oled_screen_display_bitmap(arrows_bitmap_arr[keyboard_combination[idx] - 1],
                             (idx * 16) + (4 * idx) + 2, 12, 16, 16, correct);
}

static void show_sequence() {
  new_sequence();
  for (uint8_t i = 0; i < SEQUENCE_SIZE; i++) {
    show_arrow(i, false);
  }
}

void engine_infection_alert() {
  screen_saver_stop();
  menus_module_disable_input();
  genera_screen_display_card_information("Alerta", "Virus detectado");
  vTaskDelay(2500 / portTICK_PERIOD_MS);
  genera_screen_display_card_information("Porfavor", "conservar calma");
  vTaskDelay(2500 / portTICK_PERIOD_MS);
  genera_screen_display_card_information("Memorice", "la secuencia");
  vTaskDelay(2500 / portTICK_PERIOD_MS);
  genera_screen_display_card_information("", "");
  show_sequence();
  vTaskDelay(4500 / portTICK_PERIOD_MS);
  genera_screen_display_card_information("Intruducir", "Secuencia");
  combination_infect = true;
  menus_module_enable_input();
  xTaskCreate(engine_infection_vaccine_start_timer, "infection_vaccine_timer",
              4096, NULL, 10, NULL);
  menus_module_set_app_state(true, engine_handler_keyboard_combination);
}