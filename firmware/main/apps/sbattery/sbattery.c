#include "sbattery.h"
#include "esp_log.h"
#include "leds_d.h"
#include "menus_module.h"
#include "oled_screen.h"
#include "preferences.h"

/**
 * Baterry level
 * 0 = Red_x 1 = Red_y
 * 2 = Orange_x 3 = Orange_y
 * 4 = Yellow_x 5 = Yellow_y
 * 6 = Green_x 7 = Green_y
 */
static uint8_t battery_levl = 0;
static uint16_t current_item = 0;

static const general_menu_t sbattery = {
    .menu_items = sbattery_menu_items,
    .menu_count = MENU_ITEM_COUNT,
    .menu_level = GENERAL_TREE_APP_MENU,
};

static void sbattery_handler();
static void sbattery_display_status();
static void module_cb_event(uint8_t button_name, uint8_t button_event);
static void module_cb_event_leds(uint8_t button_name, uint8_t button_event);

static void module_cb_event(uint8_t button_name, uint8_t button_event) {
  if (button_event != BUTTON_PRESS_DOWN) {
    return;
  }
  switch (button_name) {
    case BUTTON_UP:
      current_item = current_item-- == 0 ? SBATTERY_CHANGE - 1 : current_item;
      general_screen_display_menu(current_item);
      break;
    case BUTTON_DOWN:
      current_item = ++current_item > SBATTERY_CHANGE - 1 ? 0 : current_item;
      general_screen_display_menu(current_item);
      break;
    case BUTTON_RIGHT:
      current_item = 0;
      uint8_t scroll_pos = (4 * 2) + 2;
      general_clear_screen();
      genera_screen_display_card_information("Bateria", "");
      sbattery_display_status();
      oled_screen_display_bitmap(simple_up_arrow_bmp, (128 - 10), scroll_pos, 8,
                                 8, OLED_DISPLAY_NORMAL);
      oled_screen_display_bitmap(simple_down_arrow_bmp, (128 - 10),
                                 scroll_pos + 8, 8, 8, OLED_DISPLAY_NORMAL);
      menus_module_set_app_state(true, module_cb_event_leds);
      break;
    case BUTTON_LEFT:
      menus_module_restart();
      break;
    default:
      break;
  }
}

static void module_cb_event_leds(uint8_t button_name, uint8_t button_event) {
  if (button_event != BUTTON_PRESS_DOWN) {
    return;
  }
  switch (button_name) {
    case BUTTON_UP:
      battery_levl = ++battery_levl > 4 - 1 ? 0 : battery_levl;
      sbattery_handler();
      sbattery_display_status();
      break;
    case BUTTON_DOWN:
      battery_levl = battery_levl-- == 0 ? 4 - 1 : battery_levl;
      sbattery_handler();
      sbattery_display_status();
      break;
    case BUTTON_RIGHT:
      break;
    case BUTTON_LEFT:
      general_register_menu(&sbattery);
      menus_module_set_app_state(true, module_cb_event);
      general_screen_display_menu(current_item);
      break;
    default:
      break;
  }
}

static void sbattery_handler() {
  ESP_LOGI("SBATTERY", "sbattery_handler level: %d", battery_levl);
  // leds_off();
  if (battery_levl == 0) {
    leds_off();
    leds_on_r();
  } else if (battery_levl == 1) {
    leds_off();
    leds_on_r();
    leds_on_o();
  } else if (battery_levl == 2) {
    leds_off();
    leds_on_r();
    leds_on_o();
    leds_on_y();
  } else if (battery_levl == 3) {
    leds_off();
    leds_on_r();
    leds_on_o();
    leds_on_y();
    leds_on_g();
  }
  preferences_put_int("sbattery", battery_levl);
}

static void sbattery_display_status() {
  char battery_status[16];
  sprintf(battery_status, "Nivel: %d", (battery_levl + 1));
  oled_screen_display_text(battery_status, 8, 2, OLED_DISPLAY_NORMAL);
}

void sbattery_set_status() {
  int battery_level = preferences_get_int("sbattery", 0);
  battery_levl = (uint8_t) battery_level;

  ESP_LOGI("SBATTERY", "Battery level: %d", battery_levl);
  sbattery_handler();
}

void sbattery_begin() {
  int battery_level = preferences_get_int("sbattery", 0);
  battery_levl = (uint8_t) battery_level;
  general_register_menu(&sbattery);
  menus_module_set_app_state(true, module_cb_event);
  // sbattery_display_status();
  general_screen_display_menu(current_item);
}
