#include "vaccine_builder_screens.h"

#include "oled_screen.h"
#include "vaccine_builder.h"
#include "virus.h"

void vaccine_builder_screen_update_comp(vaccine_builder_ctx_t* ctx) {
  oled_screen_clear_buffer();
  oled_screen_display_text_center(vaccine_comp_str[ctx->comp], 0,
                                  OLED_DISPLAY_NORMAL);
  oled_screen_display_text(components_str[ctx->comp][*ctx->comp_ptr], 0, 2,
                           OLED_DISPLAY_NORMAL);
  oled_screen_display_show();
}