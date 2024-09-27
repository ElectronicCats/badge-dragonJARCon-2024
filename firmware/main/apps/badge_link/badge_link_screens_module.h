#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "badge_link_bitmaps.h"

typedef enum {
  BADGE_LINK_SCANNING = 0,
  BADGE_LINK_BRING_IT_CLOSER,
  BADGE_LINK_FOUND_TEXT,
  BADGE_LINK_FOUND_LOGO,
  BADGE_LINK_NOT_FOUND,
  BADGE_LINK_UNLOCK_FEATURE,
} badge_link_screens_status_t;

const char* badge_link_status_strings[] = {"Scanning",     "Bring it closer",
                                           "Found (Text)", "Found (logo)",
                                           "Not found",    "Feature unlocked"};

TaskHandle_t badge_link_screens_module_scan_task_handle = NULL;

/**
 * @brief Display the status screen
 *
 * @param status
 * @return void
 */
void badge_link_screens_module_display_status(
    badge_link_screens_status_t status);

void badge_link_screens_module_set_found_badge_logo(
    epd_bitmap_logo_index_t new_found_badge_logo_index);

void badge_link_screens_module_show_help();

/**
 * @brief Display the number of badges found of every event
 */
void badge_link_screens_module_display_count();
