#include "infection_screens.h"
#include "general/general_screens.h"
#include "infection.h"
#include "infection_scenes.h"
#include "menus_module.h"
#include "oled_screen.h"

typedef enum {
  VIRUS_FRAME,
  TIME_FRAME,
  FRIENDS_FRAME,
  FRAMES_NUM
} infection_state_frames_t;

static void show_corner_timer(infection_ctx_t* ctx) {
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

static void show_infection_state(infection_ctx_t* ctx) {
  if (infection_scenes_get_scene() < INFECTION_STATE_SCENE) {
    show_corner_timer(ctx);
    return;
  }
  static uint8_t frame = 0;
  if (ctx->patient->state < INFECTED) {
    frame = 0;
  }
  oled_screen_clear_buffer();
  char* str = (char*) malloc(100);
  switch (frame / 2) {
    case VIRUS_FRAME:
      sprintf(str, "Virus: %s",
              ctx->patient->state > VACCINATED ? virus_str[ctx->patient->virus]
                                               : "Negativo");
      break;
    case TIME_FRAME:
      sprintf(str, "Tiempo: %02d:%02d", ctx->patient->remaining_time / 60,
              ctx->patient->remaining_time % 60);
      break;
    case FRIENDS_FRAME:
      sprintf(str, "Amigos: %d", ctx->patient->friends_saved_count);
      break;
    default:
      break;
  }
  genera_screen_display_card_information(
      patient_states_str[ctx->patient->state], str);
  free(str);
  frame = ++frame > FRAMES_NUM * 2 - 3 ? 0 : frame;
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