#include "infection_screens.h"
#include "general/general_screens.h"
#include "infection.h"
#include "menus_module.h"
#include "oled_screen.h"

static void show_infection_state(infection_ctx_t* ctx) {
  if (menus_module_get_current_menu() != MENU_INFECTION_STATE) {
    char* str = (char*) malloc(20);
    if (ctx->patient->remaining_time == 0) {
      sprintf(str, "Infectado");
    } else {
      sprintf(str, "%02d:%02d", ctx->patient->remaining_time / 60,
              ctx->patient->remaining_time % 60);
    }
    oled_screen_display_text(str, 88, 0, OLED_DISPLAY_NORMAL);
    free(str);
    return;
  }
  oled_screen_clear_buffer();
  char* str = (char*) malloc(100);
  sprintf(str, "Virus: %s Tiempo: %02d:%02d Amigos: %d",
          ctx->patient->state > VACCINATED ? virus_str[ctx->patient->virus]
                                           : "Negativo",
          ctx->patient->remaining_time / 60, ctx->patient->remaining_time % 60,
          ctx->patient->friends_saved_count);

  genera_screen_display_card_information(
      patient_states_str[ctx->patient->state], str);
  free(str);
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