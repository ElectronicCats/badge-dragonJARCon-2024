#pragma once

#include <stdbool.h>
#include <stdio.h>

#include "virus.h"

#define MAC_SIZE 6

#define LIFE_TIME 900

#define VIRUS_MEM         "virus"
#define LIFETIME_MEM      "lifetime"
#define STATE_MEM         "pat_state"
#define FRIENDS_SAVED_MEM "saved_friends"

typedef enum {
  HEALTY,
  IDLE,
  VACCINATED,
  INFECTED,  // Fase 1
  CRITICAL,  // Fase 2
  TERMINAL   // Fase 3
} patient_state_t;

typedef struct {
  uint8_t mac[MAC_SIZE];
  patient_state_t state;
  virus_t virus;
  uint16_t remaining_time;
  uint16_t friends_saved_count;
} patient_t;

typedef struct {
  patient_t* patient;
  vaccine_t* vaccine;
} infection_ctx_t;

char* patient_states_str[] = {"Sano",      "",        "Vacunado",
                              "Infectado", "Critico", "Terminal"};

void infection_begin();
void infection_scenes_begin();
void infection_display_status();

void get_infected();

void infection_start_pairing();
void infection_stop_pairing();

void infection_vaccine_builder_mRNA();
void infection_vaccine_builder_viral_code();
void infection_vaccine_builder_Lipid_layer();

patient_state_t infection_get_patient_state();