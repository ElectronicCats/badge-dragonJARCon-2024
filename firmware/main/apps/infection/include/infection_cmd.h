#pragma once

#include "infection.h"
#include "virus.h"

#define MASTER_CODE 1234567890123456789

typedef enum {
  VIRUS_CMD,
  VACCINE_REQ_CMD,
  VACCINE_RES_CMD,
  MASTER_CURE_CMD
} infection_cmds_t;

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

typedef struct {
  infection_cmds_t cmd;
  uint64_t master_code;
} master_cure_req_cmd_t;
