#pragma once

#include <stdio.h>

typedef enum {
  INFECTION_MAIN_SCENE,
  INFECTION_MAIN_HELP_SCENE,
  INFECTION_VACCINES_BUILDER_SCENE,
  INFECTION_VACCINES_BUILDER_HELP_SCENE,
  INFECTION_VACCINES_RECEIVER_SCENE,
  INFECTION_VACCINES_RECEIVER_HELP_SCENE,
  INFECTION_STATE_SCENE
} infection_scenes_t;

uint8_t infection_scenes_get_scene();

void infection_scenes_main_menu();
void infection_scenes_vaccines_builder_menu();
void infection_scenes_vaccines_builder_help();
void infection_scenes_vaccines_receiver_help();
void infection_scenes_vaccines_receiver_menu();
void infection_scenes_help();
void infection_scenes_state_menu();