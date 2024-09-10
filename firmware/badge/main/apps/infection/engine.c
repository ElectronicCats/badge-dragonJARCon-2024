#include "engine.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "screen_saver.h"
#include "menus_module.h"
#include "esp_random.h"
#include "general/bitmaps_general.h"

#define ITEMOFFSET 1

static uint8_t infection_probability = 10; // 10%
static uint16_t infection_time = 30;
static bool combination_infect = false;

static void engine_infection_vaccine_init();
static void engine_handler_keyboard(uint8_t button_name, uint8_t button_event);

static uint16_t current_item = 0;
static uint16_t keyboard_combination[8] = {BUTTON_UP,BUTTON_UP,BUTTON_DOWN,BUTTON_DOWN,BUTTON_LEFT,BUTTON_RIGHT,BUTTON_LEFT,BUTTON_RIGHT};
static uint16_t keyboard_combination_user[8] = {0,0,0,0,0,0,0,0};
static uint8_t keyboard_combination_index = 0;
static enum {
  VACCIN_MENU_HELP,
  VACCIN_MENU_CODE,
  VACCIN_MENU_RAND,
  VACCIN_MENU_COUNT
} vaccine_menu_item_e = VACCIN_MENU_CODE;
static char* vaccine_menu_items[VACCIN_MENU_COUNT] = {"Pistas", "Decifrar", "Dados"};

static char* vaccine_help_items[41] = {
    "Un viejo sabio",
    "te ha dejado un",
    "mensaje secreto",
    "El mensaje lo ",
    "encuentras mas",
    "cerca de lo",
    "que piensas.",
    "                ",
    "Decifra el codigo",
    "del virus o",
    "lanza los dados",
    "para obtener la",
    "vacuna...",
    "Pero cuidado",
    "el virus se ",
    "propaga rapido",
    "y el tiempo",
    "se agota...",
    "Te sientes con ",
    "suerte para",
    "lanzar los dados?",
    "O         ",
    "prefieres decifrar",
    "el codigo?",
    "Elige sabiamente...",
    "                  ",
    "                  ",
    "                  ",
    "                  ",
    "                  ",
    "                  ",
    "                  ",
    "                  ",
    "                  ",
    "                  ",
    "                  ",
    "                  ",
    "Compra una",
    "Catsniffer y te",
    "damos el codigo",
    "de la vacuna"
};

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
  while(combination_infect){
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    infection_time--;
    char infection_time_str[18];
    sprintf(infection_time_str, "Restante: %d", infection_time);
    oled_screen_display_text(infection_time_str, 0, 0, OLED_DISPLAY_NORMAL);
    if(infection_time == 0){
      engine_infection_vaccine_stop_timer();
      get_infected();
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

static void engine_infection_vaccine_dice() {
  genera_screen_display_card_information("Lanzando dados", "");
  
  // Simulate dice roll with a 20 sided dice
  uint8_t dice_roll = (esp_random() % 20) + 1;
  // Recalculate infection probability
  infection_probability = (dice_roll * 5) % 100;
  char dice_roll_str[3];
  sprintf(dice_roll_str, "%d", infection_probability);
  if(infection_probability < 9){
    genera_screen_display_card_information("Infectado", dice_roll_str);
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    get_infected();
  }else{
    genera_screen_display_card_information("Salvado", dice_roll_str);
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    module_reset_menu();
  }
}

static void engine_infection_keyboard_combination_fail(){
  genera_screen_display_notify_information("Error", "La combinacion es incorrecta");
}

static void engine_infection_keyboard_combination_done(){
  genera_screen_display_notify_information("Excelente", "La combinacion es correcta");
}

static void engine_handler_keyboard_combination(uint8_t button_name, uint8_t button_event) {
  if (button_event != BUTTON_PRESS_DOWN) {
    return;
  }

  switch (button_name) {
    case BUTTON_UP:
      keyboard_combination_user[keyboard_combination_index] = BUTTON_UP;
      break;
    case BUTTON_DOWN:
      keyboard_combination_user[keyboard_combination_index] = BUTTON_DOWN;
      break;
    case BUTTON_LEFT:
      keyboard_combination_user[keyboard_combination_index] = BUTTON_LEFT;
      break;
    case BUTTON_RIGHT:
      keyboard_combination_user[keyboard_combination_index] = BUTTON_RIGHT;
      break;
    default:
      break;
  }
  keyboard_combination_index++;
  if(keyboard_combination_index >= 8){
    keyboard_combination_index = 0;
    for(int i = 0; i < 8; i++){
      if(keyboard_combination[i] != keyboard_combination_user[i]){
        engine_infection_keyboard_combination_fail();
        return;
      }
    }
    engine_infection_vaccine_stop_timer();
    engine_infection_keyboard_combination_done();
    general_register_menu(&vaccin_menu);
    menus_module_set_app_state(true, engine_handler_keyboard);
    general_screen_display_menu(current_item);
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
    if(current_item == 0){
      current_item = VACCIN_MENU_COUNT - 1;
    }
    
    general_screen_display_menu(current_item);
      break;
    case BUTTON_DOWN:
    current_item++;
    if(current_item >= VACCIN_MENU_COUNT){
      current_item = 0;
    }
    general_screen_display_menu(current_item);
      break;
    case BUTTON_LEFT:
      break;
    case BUTTON_RIGHT:
      if(current_item == VACCIN_MENU_HELP){
        current_item = 0;
        general_register_scrolling_menu(&vaccin_help_menu);
        general_screen_display_scrolling_text_handler(module_reset_menu);
      }else if(current_item == VACCIN_MENU_CODE){
        current_item = 0;
        engine_infection_vaccine_decipher();
      }else if(current_item == VACCIN_MENU_RAND){
        current_item = 0;
        engine_infection_vaccine_dice();
      }
      break;
    default:
      break;
  }
}

void engine_infection_alert() {
  screen_saver_stop();
  genera_screen_display_notify_information("Alerta", "En proceso de infeccion, por favor mantenga la calma");
  vTaskDelay(2000 / portTICK_PERIOD_MS);
  genera_screen_display_notify_information("Alerta", "Soluciona el retos para evitar la infeccion");
  vTaskDelay(2000 / portTICK_PERIOD_MS);
  combination_infect = true;

  xTaskCreate(engine_infection_vaccine_start_timer, "infection_vaccine_timer", 4096, NULL, 10, NULL);

  menus_module_set_app_state(true, engine_handler_keyboard_combination);
}