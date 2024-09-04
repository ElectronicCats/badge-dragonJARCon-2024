#pragma once

#include <stdio.h>

typedef enum { SHOW_INFECTION_STATE_EV } infection_event_t;

void infection_screens_handler(infection_event_t event, void* context);