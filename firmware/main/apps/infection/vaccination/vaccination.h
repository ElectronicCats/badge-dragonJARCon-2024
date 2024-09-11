#pragma once

#include <stdbool.h>
#include <stdio.h>

#define RECIPIENT  0
#define VACCINATOR 1

typedef enum {
  WAITING_STATE,
  IN_PROGREES_STATE,
  FINISH_STATE
} vaccination_states_t;

typedef struct {
  uint8_t state;
  uint8_t progress;
  bool role;
  volatile bool is_task_running;
} vaccination_ctx_t;

void vaccination_begin();
void vaccination_exit();