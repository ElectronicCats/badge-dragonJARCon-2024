#include "general/general_screens.h"
#include "general/bitmaps_general.h"
#ifndef SBATTERY_H
#define SBATTERY_H

enum {
  SBATTERY_CHANGE,
  MENU_ITEM_COUNT
} sbattery_menu_item = SBATTERY_CHANGE;
char* sbattery_menu_items[MENU_ITEM_COUNT] = {"Bateria"};

void sbattery_set_status();
void sbattery_begin();
#endif  // SBATTERY_H