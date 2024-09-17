#pragma once

#include "infection.h"
#include "virus.h"

typedef enum { VIRUS_CMD, VACCINE_REQ_CMD, VACCINE_RES_CMD } infection_cmds_t;

typedef struct {
  infection_cmds_t cmd;
} virus_cmd_t;

typedef struct {
  infection_cmds_t cmd;
  vaccine_t vaccine;
} vaccine_req_cmd_t;

typedef struct {
  infection_cmds_t cmd;
} vaccine_res_cmd_t;
