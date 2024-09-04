#include "infection_screens.h"

#include "infection.h"
#include "menus_module.h"
#include "oled_screen.h"

static void show_infection_state(infection_ctx_t* ctx) {
  if (menus_module_get_current_menu() != MENU_INFECTION_STATE) {
    char* str = (char*) malloc(20);
    sprintf(str, "%02d:%02d", ctx->patient->remaining_time / 60,
            ctx->patient->remaining_time % 60);
    oled_screen_display_text(str, 88, 3, OLED_DISPLAY_NORMAL);
    free(str);
    return;
  }
  oled_screen_clear_buffer();
  char* str = (char*) malloc(20);
  sprintf(str, "State: %s", patient_states_str[ctx->patient->state]);
  oled_screen_display_text(str, 0, 0, OLED_DISPLAY_NORMAL);
  sprintf(str, "Virus: %s",
          ctx->patient->state > VACCINATED ? virus_str[ctx->patient->virus]
                                           : "Negativo");
  oled_screen_display_text(str, 0, 1, OLED_DISPLAY_NORMAL);
  sprintf(str, "Time: %02d:%02d", ctx->patient->remaining_time / 60,
          ctx->patient->remaining_time % 60);
  oled_screen_display_text(str, 0, 2, OLED_DISPLAY_NORMAL);
  sprintf(str, "Friends: %d", ctx->patient->friends_saved_count);
  oled_screen_display_text(str, 0, 3, OLED_DISPLAY_NORMAL);
  free(str);
  oled_screen_display_show();
}

void infection_screens_handler(infection_event_t event, void* context) {
  switch (event) {
    case SHOW_INFECTION_STATE_EV:
      show_infection_state(context);
      break;
    default:
      break;
  }
}