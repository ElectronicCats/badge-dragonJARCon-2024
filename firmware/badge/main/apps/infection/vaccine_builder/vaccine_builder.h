#pragma once

#include <stdio.h>
#include "virus.h"

typedef struct {
  uint8_t* comp_ptr;
  uint8_t comp;
} vaccine_builder_ctx_t;

void vaccine_builder_begin(uint8_t comp, void* comp_ptr);