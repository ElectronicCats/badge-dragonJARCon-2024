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
  MAIN_HELP_OPTION,
  STATE_OPTION,
  VACCINES_OPTION
} infection_main_menu_options;
const char* infection_main_menu_items[] = {"Instrucciones", "Estado",
                                           "Vacunas"};

static void main_menu_selection_handler(uint8_t selection) {
  switch (selection) {
    case MAIN_HELP_OPTION:
      infection_scenes_help();
      break;
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
                                           "si estas atento",
                                           "al badge",
                                           "",
                                           "Es importante",
                                           "leer todas las",
                                           "instrucciones",
                                           "ya que el menu",
                                           "VACUNAS cambia",
                                           "entre badges",
                                           "infectados",
                                           "y badges",
                                           "saludables",
                                           "",
                                           "Descubre que",
                                           "tipo de persona",
                                           "eres:",
                                           "- Observador",
                                           "- Con suerte",
                                           "- Fuerza bruta",
                                           "",
                                           "Seras capaz de",
                                           "sobrevivir?"
                                           "",
                                           "",
                                           "",
                                           "",
                                           "",
                                           "",
                                           "Bnloq`\31tm`\31",
                                           "B`srmheedq\31x\31sd",
                                           "c`lnr\31dk\31bnchfn",
                                           "cd\31k`\31u`btm`",
                                           "",
                                           "",
                                           "",
                                           "",
                                           "",
                                           "",
                                           ""};
static const general_menu_t infection_help_menu = {
    .menu_count = 47,
    .menu_items = infection_help_txt,
    .menu_level = GENERAL_TREE_APP_SUBMENU};

void infection_scenes_help() {
  current_scene = INFECTION_MAIN_HELP_SCENE;
  general_register_scrolling_menu(&infection_help_menu);
  general_screen_display_scrolling_text_handler(infection_scenes_main_menu);
}

////////////////////////////    Vacunas Menu   ////////////////////////////
typedef enum {
  VACCINES_BUILD_HELP_OPTION,
  CREATE_VACCINE_OPTION,
  ADMINISTER_VACCINE_OPTION,
  DICE_ROLL_OPTION
} infection_vaccines_menu_options;
const char* vaccines_builder_menu_items[] = {"Instrucciones", "Crear Vacuna",
                                             "Aplicar Vacuna", "Tirar Dados"};

static void vaccines_builder_menu_selection_handler(uint8_t selection) {
  switch (selection) {
    case VACCINES_BUILD_HELP_OPTION:
      infection_scenes_vaccines_builder_help();
      break;
    case CREATE_VACCINE_OPTION:
      vaccine_builder_begin();
      break;
    case ADMINISTER_VACCINE_OPTION:
      vaccination_begin();
      break;
    case DICE_ROLL_OPTION:
      engine_infection_vaccine_dice();
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
                                                  "---------------",
                                                  " CREAR VACUNA ",
                                                  "---------------",
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
                                                  "   Ejemplo de  ",
                                                  "     mezcla:   ",
                                                  VACCINE_COMP_STR_1 ":",
                                                  "-" ARN_STR_1,
                                                  VACCINE_COMP_STR_2 ":",
                                                  "-" VIRAL_CODE_STR_1,
                                                  VACCINE_COMP_STR_3 ":",
                                                  "-" LIPID_LAYER_STR_1,
                                                  "",
                                                  "Toma un badge",
                                                  "infectado para",
                                                  "probar tu",
                                                  "vacuna desde el",
                                                  "menu:",
                                                  "APLICAR VACUNA",
                                                  "acercate lo mas",
                                                  "que puedas.",
                                                  "",
                                                  "Si la vacuna es",
                                                  "correcta, el",
                                                  "infectado ",
                                                  "muestra el",
                                                  "texto: CURADO",
                                                  "de lo contrario",
                                                  "muestra:",
                                                  "SIN EFECTO",
                                                  "",
                                                  "Puede ser de",
                                                  "ayuda conocer",
                                                  "el virus",
                                                  "contraido por",
                                                  "el infectado,",
                                                  "encuentralo en",
                                                  "el menu: ESTADO",
                                                  "dentro del",
                                                  "badge infectado",
                                                  "",
                                                  "---------------",
                                                  " LANZAR DADOS",
                                                  "---------------",
                                                  "Los dados son",
                                                  "peligrosos,",
                                                  "usalos bajo tu",
                                                  "responsabilidad",
                                                  "",
                                                  "Si no quieres",
                                                  "buscar una cura",
                                                  "pon a prueba tu",
                                                  "suerte con los",
                                                  "dados",
                                                  "",
                                                  "Obten 20 y gana",
                                                  "la combinacion",
                                                  "para una vacuna",
                                                  "u obten el",
                                                  "numero 1 y ",
                                                  "contrae la",
                                                  "infeccion al",
                                                  "instante",
                                                  "",
                                                  "Cualquier otro",
                                                  "numero no tiene",
                                                  "efecto",
                                                  "",
                                                  "Pero recuerda..",
                                                  "Debes seguir",
                                                  "alerta todo el",
                                                  "tiempo!!!"};

static const general_menu_t vaccines_builder_help_menu = {
    .menu_count = 96,
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
  RECEIVER_HELP_OPTION,
  RECEIVER_OPTION,
  DICE_OPTION
} infection_receiver_menu_options;
const char* infection_receiver_menu_items[] = {
    "Instrucciones", "Recibir Vacuna", "Lanzar Dados"};

static void vaccines_receiver_menu_selection_handler(uint8_t selection) {
  switch (selection) {
    case RECEIVER_HELP_OPTION:
      infection_scenes_vaccines_receiver_help();
      break;
    case RECEIVER_OPTION:
      vaccination_begin();
      break;
    case DICE_OPTION:
      engine_infection_vaccine_dice();
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
                                                   "cura en este",
                                                   "momento",
                                                   "",
                                                   "---------------",
                                                   "RECIBIR VACUNAS",
                                                   "---------------",
                                                   "Abre el menu",
                                                   "RECIBIR VACUNAS",
                                                   "y busca",
                                                   "un badge",
                                                   "saludable para",
                                                   "para que te",
                                                   "administre",
                                                   "vacunas",
                                                   "",
                                                   "Sera de ayuda",
                                                   "conocer el tipo",
                                                   "de virus",
                                                   "contraido,",
                                                   "encuentralo en",
                                                   "el menu ESTADO",
                                                   "",
                                                   "---------------",
                                                   "  LANZAR DADOS ",
                                                   "---------------",
                                                   "Se paciente,",
                                                   "o prueba suerte",
                                                   "con los dados",
                                                   "",
                                                   "Intenta hasta",
                                                   "obtener el",
                                                   "numero 20",
                                                   "para salvarte",
                                                   "",
                                                   "Solo no",
                                                   "lo olvides...",
                                                   "",
                                                   "Cada tiro te",
                                                   "resta el poco",
                                                   "tiempo que te",
                                                   "queda"};

static const general_menu_t vaccines_receiver_help_menu = {
    .menu_count = 51,
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