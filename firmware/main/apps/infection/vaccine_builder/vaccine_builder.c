#include "vaccine_builder.h"

#include "bitmaps_general.h"
#include "infection.h"
#include "infection_scenes.h"
#include "menus_module.h"
#include "oled_screen.h"

#define COMPONENTS_NUM 4
#define VARIANTS_NUM   4

static uint8_t* comp_ptr;
static uint8_t comp = 0;

static uint8_t* get_comp_ptr(uint8_t comp_idx);

static void show_vaccine() {
  oled_screen_clear_buffer();
  oled_screen_display_text_center("Resumen", 0, OLED_DISPLAY_INVERT);
  for (uint8_t i = 0; i < COMPONENTS_NUM - 1; i++) {
    oled_screen_display_text(components_str[i][*get_comp_ptr(i)], 0, i + 1,
                             OLED_DISPLAY_NORMAL);
  }
  oled_screen_display_show();
}

static void update_component() {
  if (comp == COMPONENTS_NUM - 1) {
    show_vaccine();
    return;
  }
  oled_screen_clear_buffer();
  oled_screen_display_text_center(vaccine_comp_str[comp], 0,
                                  OLED_DISPLAY_INVERT);
  uint8_t x = 64 + strlen(vaccine_comp_str[comp]) * 8 / 2;
  oled_screen_display_bitmap(simple_right_arrow_bmp, x, 0, 8, 8,
                             OLED_DISPLAY_NORMAL);
  oled_screen_display_bitmap(simple_up_arrow_bmp, 60, 8, 8, 8,
                             OLED_DISPLAY_NORMAL);
  oled_screen_display_bitmap(simple_down_arrow_bmp, 60, 24, 8, 8,
                             OLED_DISPLAY_NORMAL);
  oled_screen_display_text_center(components_str[comp][*comp_ptr], 2,
                                  OLED_DISPLAY_NORMAL);
  oled_screen_display_show();
}

static uint8_t* get_comp_ptr(uint8_t comp_idx) {
  infection_ctx_t* ctx = infection_get_context();
  switch (comp_idx) {
    case mRNA_COMP:
      comp_ptr = &ctx->vaccine->arn;
      break;
    case VIRAL_CODE_COMP:
      comp_ptr = &ctx->vaccine->viral_code;
      break;
    case LIPID_LAYER_COMP:
      comp_ptr = &ctx->vaccine->lipid_layer;
      break;
    default:
      break;
  }
  return comp_ptr;
}

static void vaccine_builder_input_cb(uint8_t button_name,
                                     uint8_t button_event) {
  if (button_event != BUTTON_PRESS_DOWN) {
    return;
  }
  switch (button_name) {
    case BUTTON_LEFT:
      infection_scenes_vaccines_builder_menu();
      break;
    case BUTTON_RIGHT:
      comp = ++comp > COMPONENTS_NUM - 1 ? 0 : comp;
      get_comp_ptr(comp);
      update_component();
      break;
    case BUTTON_UP:
      if (comp == COMPONENTS_NUM - 1) {
        return;
      }
      (*comp_ptr) = (*comp_ptr)-- == 0 ? VARIANTS_NUM - 1 : (*comp_ptr);
      update_component();
      break;
    case BUTTON_DOWN:
      if (comp == COMPONENTS_NUM - 1) {
        return;
      }
      (*comp_ptr) = ++(*comp_ptr) > VARIANTS_NUM - 1 ? 0 : (*comp_ptr);
      update_component();
      break;
    default:
      break;
  }
}

void vaccine_builder_begin() {
  menus_module_set_app_state(true, vaccine_builder_input_cb);
  get_comp_ptr(comp);
  update_component();
}