#include "vaccine_builder.h"

#include "infection.h"
#include "infection_scenes.h"
#include "menus_module.h"
#include "vaccine_builder_screens.h"

#define COMPONENTS_NUM 3

static vaccine_builder_ctx_t* vac_builder_ctx = NULL;

static void update_component() {
  vaccine_builder_screen_update_comp(vac_builder_ctx);
}

static void vaccine_builder_input_cb(uint8_t button_name,
                                     uint8_t button_event) {
  if (button_event != BUTTON_PRESS_DOWN) {
    return;
  }
  switch (button_name) {
    case BUTTON_LEFT:
      free(vac_builder_ctx);
      infection_scenes_vaccines_builder_menu();
      break;
    case BUTTON_RIGHT:
      free(vac_builder_ctx);
      infection_scenes_vaccines_builder_menu();
      break;
    case BUTTON_UP:
      (*vac_builder_ctx->comp_ptr) = (*vac_builder_ctx->comp_ptr)-- == 0
                                         ? 3 - 1
                                         : (*vac_builder_ctx->comp_ptr);
      update_component();
      break;
    case BUTTON_DOWN:
      (*vac_builder_ctx->comp_ptr) = ++(*vac_builder_ctx->comp_ptr) > 3 - 1
                                         ? 0
                                         : (*vac_builder_ctx->comp_ptr);
      update_component();
      break;
    default:
      break;
  }
}

void vaccine_builder_begin(uint8_t comp, void* comp_ptr) {
  vac_builder_ctx = malloc(sizeof(vaccine_builder_ctx_t));
  vac_builder_ctx->comp = comp;
  vac_builder_ctx->comp_ptr = (uint8_t*) comp_ptr;
  menus_module_set_app_state(true, vaccine_builder_input_cb);
  update_component();
}