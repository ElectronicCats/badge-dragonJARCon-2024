#pragma once

#include <stdbool.h>
#include <stdio.h>

#include "virus.h"

#define MAC_SIZE 6

#define LIFE_TIME 1200

#define VIRUS_MEM         "virus"
#define LIFETIME_MEM      "lifetime"
#define STATE_MEM         "pat_state"
#define FRIENDS_SAVED_MEM "saved_friends"
#define INIT_MEM          "init"
#define ENCRYPT_MEM       "encrypt"
#define TYPOGRAPHY_MEM    "tipography"

#define CREATOR 0

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
  uint8_t inmunity;
  bool init;
} patient_t;

typedef struct {
  patient_t* patient;
  vaccine_t* vaccine;
} infection_ctx_t;

char* patient_states_str[] = {"Saludable", "",        "Vacunado",
                              "Infectado", "Critico", "Terminal"};

void infection_begin();
void infection_scenes_begin();
void infection_display_status();

void infection_get_infected();

patient_state_t infection_get_patient_state();

void send_vaccine_req_cmd();
void infection_set_inmunity_time();
void infection_set_patient_state(patient_state_t state);
infection_ctx_t* infection_get_context();
void infection_get_vaccinated();
uint8_t get_random_uint8();
bool get_random_bool();