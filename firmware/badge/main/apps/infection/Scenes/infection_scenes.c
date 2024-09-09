#include "infection_scenes.h"

#include "general_submenu.h"
#include "infection.h"
#include "menus_module.h"

void infection_scenes_vaccines_menu();

//////////////////////////////////// Main Menu ////////////////////////////////
typedef enum {
  ESTADO_OPTION,
  VACUNA_OPTION,
  AYUDA_OPTION
} infection_main_menu_options;
const char* infection_main_menu_items[] = {"Estado", "Vacunar", "Ayuda"};

static void main_menu_selection_handler(uint8_t selection) {
  switch (selection) {
    case ESTADO_OPTION:
      printf("ESTADO_OPTION\n");
      break;
    case VACUNA_OPTION:
      infection_scenes_vaccines_menu();
      break;
    case AYUDA_OPTION:
      printf("AYUDA_OPTION\n");
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

//////////////////////////////////// Vacunas Menu
///////////////////////////////////
typedef enum {
  mRNA_OPTION,
  VIRAL_CODE_OPTION,
  LIPID_LAYER_OPTION
} infection_vaccines_menu_options;
const char* infection_vaccines_menu_items[] = {"mRNA", "Viral Code",
                                               "Lipid Layer"};

static void vaccines_menu_selection_handler(uint8_t selection) {
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
    default:
      break;
  }
}

void infection_scenes_vaccines_menu() {
  general_submenu_menu_t vaccines_menu;
  vaccines_menu.options = infection_vaccines_menu_items;
  vaccines_menu.options_count =
      sizeof(infection_vaccines_menu_items) / sizeof(char*);
  vaccines_menu.select_cb = vaccines_menu_selection_handler;
  vaccines_menu.exit_cb = infection_scenes_main_menu;
  general_submenu(vaccines_menu);
}

//////////////////////////////////// Ayuda Menu ////////////////////////////////