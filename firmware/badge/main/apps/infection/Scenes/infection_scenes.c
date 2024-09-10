#include "infection_scenes.h"

#include "general_screens.h"
#include "general_submenu.h"
#include "infection.h"
#include "menus_module.h"

void infection_scenes_vaccines_builder_menu();
void infection_scenes_vaccines_builder_help();
void infection_scenes_vaccines_receiver_help();
void infection_scenes_vaccines_receiver_menu();
void infection_scenes_help();

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
      printf("STATE_OPTION\n");
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
  general_register_scrolling_menu(&infection_help_menu);
  general_screen_display_scrolling_text_handler(infection_scenes_main_menu);
}

////////////////////////////    Vacunas Menu   ////////////////////////////
typedef enum {
  mRNA_OPTION,
  VIRAL_CODE_OPTION,
  LIPID_LAYER_OPTION,
  ADMINISTER_VACCINE_OPTION,
  VACCINES_BUILD_HELP_OPTION
} infection_vaccines_menu_options;
const char* vaccines_builder_menu_items[] = {
    VACCINE_COMP_STR_1, VACCINE_COMP_STR_2, VACCINE_COMP_STR_3,
    "Aplicar Vacuna", "Ayuda"};

static void vaccines_builder_menu_selection_handler(uint8_t selection) {
  switch (selection) {
    case mRNA_OPTION:
      infection_vaccine_builder_mRNA();
      break;
    case VIRAL_CODE_OPTION:
      infection_vaccine_builder_viral_code();
      break;
    case LIPID_LAYER_OPTION:
      infection_vaccine_builder_Lipid_layer();
      break;
    case ADMINISTER_VACCINE_OPTION:
      infection_start_pairing();
      break;
    case VACCINES_BUILD_HELP_OPTION:
      infection_scenes_vaccines_builder_help();
      break;
    default:
      break;
  }
}

void infection_scenes_vaccines_builder_menu() {
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
                                                  "Hay 3 variantes",
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
  general_register_scrolling_menu(&vaccines_builder_help_menu);
  general_screen_display_scrolling_text_handler(
      infection_scenes_vaccines_builder_menu);
}

////////////////////////////    Vaccines Receiver Menu
///////////////////////////////
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
      infection_start_pairing();
      break;
    case DICE_OPTION:
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
  general_register_scrolling_menu(&vaccines_receiver_help_menu);
  general_screen_display_scrolling_text_handler(
      infection_scenes_vaccines_receiver_menu);
}