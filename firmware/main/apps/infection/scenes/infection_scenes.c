#include "infection_scenes.h"

#include "engine.h"
#include "general_screens.h"
#include "general_submenu.h"
#include "infection.h"
#include "infection_screens.h"
#include "menus_module.h"
#include "vaccination.h"
#include "vaccine_builder.h"

static uint8_t current_scene = 0;

void infection_scenes_vaccines_builder_menu();
void infection_scenes_vaccines_builder_help();
void infection_scenes_vaccines_receiver_help();
void infection_scenes_vaccines_receiver_menu();
void infection_scenes_help();
void infection_scenes_state_menu();

uint8_t infection_scenes_get_scene() {
  return current_scene;
}

//////////////////////////////////// Main Menu ////////////////////////////////
typedef enum {
  STATE_OPTION,
  VACCINES_OPTION,
  MAIN_HELP_OPTION
} infection_main_menu_options;
const char* infection_main_menu_items[] = {"Estado", "Vacunas", "Ayuda"};

static void main_menu_selection_handler(uint8_t selection) {
  switch (selection) {
    case STATE_OPTION:
      infection_scenes_state_menu();
      break;
    case VACCINES_OPTION:
      if (infection_get_patient_state() >= INFECTED) {
        infection_scenes_vaccines_receiver_menu();
      } else {
        infection_scenes_vaccines_builder_menu();
      }
      break;
    case MAIN_HELP_OPTION:
      infection_scenes_help();
      break;
    default:
      break;
  }
}

static void main_menu_exit_handler() {
  menus_module_exit_app();
}

void infection_scenes_main_menu() {
  current_scene = INFECTION_MAIN_SCENE;
  general_submenu_menu_t main_menu;
  main_menu.options = infection_main_menu_items;
  main_menu.options_count = sizeof(infection_main_menu_items) / sizeof(char*);
  main_menu.select_cb = main_menu_selection_handler;
  main_menu.exit_cb = main_menu_exit_handler;
  general_submenu(main_menu);
}

//////////////////////////////////// Ayuda Menu ////////////////////////////////
static const char* infection_help_txt[] = {"El virus se",
                                           "propaga rapido",
                                           "",
                                           "Debes estar",
                                           "alerta todo el",
                                           "tiempo!!!",
                                           "",
                                           "La infeccion se",
                                           "puede evitar",
                                           "",
                                           "Seras capaz de",
                                           "sobrevivir?"};

static const general_menu_t infection_help_menu = {
    .menu_count = 12,
    .menu_items = infection_help_txt,
    .menu_level = GENERAL_TREE_APP_SUBMENU};

void infection_scenes_help() {
  current_scene = INFECTION_MAIN_HELP_SCENE;
  general_register_scrolling_menu(&infection_help_menu);
  general_screen_display_scrolling_text_handler(infection_scenes_main_menu);
}

////////////////////////////    Vacunas Menu   ////////////////////////////
typedef enum {
  CREATE_VACCINE_OPTION,
  ADMINISTER_VACCINE_OPTION,
  DICE_ROLL_OPTION,
  VACCINES_BUILD_HELP_OPTION
} infection_vaccines_menu_options;
const char* vaccines_builder_menu_items[] = {"Crear Vacuna", "Aplicar Vacuna",
                                             "Tirar Dados", "Ayuda"};

static void vaccines_builder_menu_selection_handler(uint8_t selection) {
  switch (selection) {
    case CREATE_VACCINE_OPTION:
      vaccine_builder_begin();
      break;
    case ADMINISTER_VACCINE_OPTION:
      vaccination_begin();
      break;
    case DICE_ROLL_OPTION:
      engine_infection_vaccine_dice();
      break;
    case VACCINES_BUILD_HELP_OPTION:
      infection_scenes_vaccines_builder_help();
      break;
    default:
      break;
  }
}

void infection_scenes_vaccines_builder_menu() {
  current_scene = INFECTION_VACCINES_BUILDER_SCENE;
  general_submenu_menu_t vaccines_builder_menu;
  vaccines_builder_menu.options = vaccines_builder_menu_items;
  vaccines_builder_menu.options_count =
      sizeof(vaccines_builder_menu_items) / sizeof(char*);
  vaccines_builder_menu.select_cb = vaccines_builder_menu_selection_handler;
  vaccines_builder_menu.exit_cb = infection_scenes_main_menu;
  general_submenu(vaccines_builder_menu);
}

////////////////// Vaccines Builder Help Menu //////////////////
static const char* vaccines_builder_help_txt[] = {"Aun hay tiempo",
                                                  "de salvarlos",
                                                  "",
                                                  "Encuentra las",
                                                  "vacunas para",
                                                  "curar badges",
                                                  "infectados",
                                                  "",
                                                  "3 virus",
                                                  "3 vacunas",
                                                  "",
                                                  "Cada vacuna",
                                                  "contiene estos",
                                                  "3 componentes:",
                                                  "-" VACCINE_COMP_STR_1,
                                                  "-" VACCINE_COMP_STR_2,
                                                  "-" VACCINE_COMP_STR_3,
                                                  "",
                                                  "Hay 4 variantes",
                                                  "por componente",
                                                  "",
                                                  "Busca la mezcla",
                                                  "correcta antes",
                                                  "de que se les",
                                                  "acabe el tiempo",
                                                  "",
                                                  "Pero recuerda..",
                                                  "Debes seguir",
                                                  "alerta todo el",
                                                  "tiempo!!!"};

static const general_menu_t vaccines_builder_help_menu = {
    .menu_count = 30,
    .menu_items = vaccines_builder_help_txt,
    .menu_level = GENERAL_TREE_APP_SUBMENU};

void infection_scenes_vaccines_builder_help() {
  current_scene = INFECTION_VACCINES_BUILDER_HELP_SCENE;
  general_register_scrolling_menu(&vaccines_builder_help_menu);
  general_screen_display_scrolling_text_handler(
      infection_scenes_vaccines_builder_menu);
}

///////////////////////    Vaccines Receiver Menu   ///////////////////////////
typedef enum {
  RECEIVER_OPTION,
  DICE_OPTION,
  RECEIVER_HELP_OPTION,
} infection_receiver_menu_options;
const char* infection_receiver_menu_items[] = {"Recibir Vacuna", "Tirar Dados",
                                               "Ayuda"};

static void vaccines_receiver_menu_selection_handler(uint8_t selection) {
  switch (selection) {
    case RECEIVER_OPTION:
      vaccination_begin();
      break;
    case DICE_OPTION:
      engine_infection_vaccine_dice();
      break;
    case RECEIVER_HELP_OPTION:
      infection_scenes_vaccines_receiver_help();
      break;
      break;
    default:
      break;
  }
}

void infection_scenes_vaccines_receiver_menu() {
  current_scene = INFECTION_VACCINES_RECEIVER_SCENE;
  general_submenu_menu_t vaccines_receiver_menu;
  vaccines_receiver_menu.options = infection_receiver_menu_items;
  vaccines_receiver_menu.options_count =
      sizeof(infection_receiver_menu_items) / sizeof(char*);
  vaccines_receiver_menu.select_cb = vaccines_receiver_menu_selection_handler;
  vaccines_receiver_menu.exit_cb = infection_scenes_main_menu;
  general_submenu(vaccines_receiver_menu);
}

////////////////// Vaccines Receiver Help Menu //////////////////
static const char* vaccines_receiver_help_txt[] = {"Estas infectado",
                                                   "y tienes poco",
                                                   "tiempo",
                                                   "",
                                                   "Pero manten la",
                                                   "calma, espera a",
                                                   "que llegue la",
                                                   "ayuda",
                                                   "",
                                                   "Todos buscan la",
                                                   "cura ahora mismo",
                                                   "",
                                                   "Se paciente,",
                                                   "o prueba suerte",
                                                   "con los dados",
                                                   "",
                                                   "Solo no",
                                                   "lo olvides...",
                                                   "",
                                                   "Cada tiro te",
                                                   "resta el poco",
                                                   "tiempo que te",
                                                   "queda"};

static const general_menu_t vaccines_receiver_help_menu = {
    .menu_count = 23,
    .menu_items = vaccines_receiver_help_txt,
    .menu_level = GENERAL_TREE_APP_SUBMENU};

void infection_scenes_vaccines_receiver_help() {
  current_scene = INFECTION_VACCINES_RECEIVER_HELP_SCENE;
  general_register_scrolling_menu(&vaccines_receiver_help_menu);
  general_screen_display_scrolling_text_handler(
      infection_scenes_vaccines_receiver_menu);
}

///////////////////////    State Menu   ///////////////////////////

static void infection_state_input_cb(uint8_t button_name,
                                     uint8_t button_event) {
  if (button_event != BUTTON_PRESS_DOWN) {
    return;
  }
  if (button_name == BUTTON_LEFT) {
    infection_scenes_main_menu();
  }
}

void infection_scenes_state_menu() {
  menus_module_set_app_state(true, infection_state_input_cb);
  current_scene = INFECTION_STATE_SCENE;
  infection_screens_handler(SHOW_INFECTION_STATE_EV, infection_get_context());
}