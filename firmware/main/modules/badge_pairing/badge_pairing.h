#pragma once

#include <stdbool.h>
#include <stdio.h>
#include "badge_connect.h"

#define PING_RETRIES 5
#define PING_RATE    1000
#define PING_TIMEOUT 150

#define PING_SALT 255
#define PING_HASH 212

#define RSSI_FILTER           -100
#define CONNECTED_RSSI_FILTER -150

typedef enum {
  ON_CONNECT_EV,
  ON_DISCONECT_EV,
} badge_pairing_events_t;

typedef void (*badge_pairing_event_cb_t)(badge_pairing_ctx_t);

typedef struct {
  badge_pairing_event_cb_t on_connect_cb;
  badge_pairing_event_cb_t on_disconnect_cb;
  badge_pairing_event_cb_t on_pairing_end_cb;
  uint8_t friend_addr[6];
  uint8_t ping_retries;
  bool ping_response;
  bool state;
  bool blue_team;
} badge_pairing_ctx_t;

typedef struct {
  uint8_t ping_salt;
  uint8_t ping_hash;
  bool is_request;
  bool blue_team;
} pairing_ping_cmd_t;

void badge_pairing_begin();
void badge_pairing_init();
void badge_pairing_deinit();

void badge_pairing_set_callbacks(badge_pairing_event_cb_t on_connect_cb,
                                 badge_pairing_event_cb_t on_disconnect_cb,
                                 badge_pairing_event_cb_t on_pairing_end_cb);
uint8_t* badge_pairing_get_friend_addr();
void ping_handler(badge_connect_recv_msg_t* msg);
void badge_pairing_reset();

void badge_pairing_set_blue_team();
void badge_pairing_set_red_team();