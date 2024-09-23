#include "badge_pairing.h"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <string.h>

static badge_pairing_ctx_t* ctx = NULL;
volatile bool is_task_running;
volatile bool waiting_task;

void badge_pairing_reset();

static void on_badge_connect(uint8_t* src_addr) {
  // printf("on_badge_connect\n");
  ctx->state = true;
  ctx->ping_response = true;
  ctx->ping_retries = 0;
  memcpy(ctx->friend_addr, src_addr, 6);
  if (ctx->on_connect_cb) {
    ctx->on_connect_cb();
  }
}

static void on_badge_disconnect() {
  // printf("on_badge_disconnect\n");
  badge_pairing_reset();
  if (ctx->on_disconnect_cb) {
    ctx->on_disconnect_cb();
  }
}

static void on_pairing_end() {}
static void send_ping() {
  ctx->ping_response = 0;
  pairing_ping_cmd_t ping_cmd;
  ping_cmd.ping_salt = PING_SALT;
  ping_cmd.ping_hash = PING_HASH;
  ping_cmd.is_request = true;
  ping_cmd.blue_team = ctx->blue_team;
  badge_connect_send(ctx->state ? ctx->friend_addr : ESPNOW_ADDR_BROADCAST,
                     &ping_cmd, sizeof(pairing_ping_cmd_t));
}

static void send_ping_response(uint8_t* src_addr) {
  pairing_ping_cmd_t ping_cmd;
  ping_cmd.ping_salt = PING_SALT;
  ping_cmd.ping_hash = PING_HASH;
  ping_cmd.is_request = false;
  ping_cmd.blue_team = ctx->blue_team;
  badge_connect_send(src_addr, &ping_cmd, sizeof(pairing_ping_cmd_t));
}

static void get_ping_response(badge_connect_recv_msg_t* msg) {
  if (!ctx->state) {
    on_badge_connect(msg->src_addr);
  } else if (memcmp(ctx->friend_addr, msg->src_addr, 6) == 0) {
    ctx->ping_response = true;
    ctx->ping_retries = 0;
  }
}

static void get_ping_request(badge_connect_recv_msg_t* msg) {
  pairing_ping_cmd_t* cmd = (pairing_ping_cmd_t*) msg->data;
  if (!ctx->state && cmd->blue_team != ctx->blue_team) {
    send_ping_response(msg->src_addr);
  } else if (memcmp(ctx->friend_addr, msg->src_addr, 6) == 0 &&
             cmd->blue_team != ctx->blue_team) {
    send_ping_response(msg->src_addr);
  }
}

void ping_handler(badge_connect_recv_msg_t* msg) {
  if (!is_task_running) {
    return;
  }
  if (msg->rx_ctrl->rssi < (ctx->state ? CONNECTED_RSSI_FILTER : RSSI_FILTER)) {
    return;
  }
  pairing_ping_cmd_t* cmd = (pairing_ping_cmd_t*) msg->data;
  if (cmd->ping_hash != PING_HASH || cmd->ping_salt != PING_SALT) {
    return;
  }
  if (cmd->is_request) {
    get_ping_request(msg);
  } else {
    get_ping_response(msg);
  }
}

static void pairing_task() {
  waiting_task = true;
  while (is_task_running) {
    send_ping();
    vTaskDelay(pdMS_TO_TICKS(ctx->ping_retries ? PING_TIMEOUT : PING_RATE));
    if (!ctx->ping_response && ctx->state) {
      ctx->ping_retries++;
      if (ctx->ping_retries >= PING_RETRIES) {
        on_badge_disconnect();
      }
    }
  }
  waiting_task = false;
  vTaskDelete(NULL);
}

void badge_pairing_deinit() {
  is_task_running = false;
  while (waiting_task) {
    vTaskDelay(pdMS_TO_TICKS(5));
  };
  badge_pairing_reset();
  // free(ctx);
}

void badge_pairing_begin() {
  ctx = calloc(1, sizeof(badge_pairing_ctx_t));
}

void badge_pairing_init() {
  is_task_running = true;
  xTaskCreate(pairing_task, "pairing_task", 2048, NULL, 10, NULL);
}

uint8_t* badge_pairing_get_friend_addr() {
  return ctx->friend_addr;
}

void badge_pairing_reset() {
  // printf("badge_pairing_reset\n");
  memset(&ctx->friend_addr, 0, 6);
  ctx->state = false;
  ctx->ping_retries = 0;
}

void badge_pairing_set_callbacks(badge_pairing_event_cb_t on_connect_cb,
                                 badge_pairing_event_cb_t on_disconnect_cb,
                                 badge_pairing_event_cb_t on_pairing_end_cb) {
  ctx->on_connect_cb = on_connect_cb;
  ctx->on_disconnect_cb = on_disconnect_cb;
  ctx->on_pairing_end_cb = on_pairing_end_cb;
}

void badge_pairing_set_blue_team() {
  if (ctx == NULL) {
    return;
  }
  ctx->blue_team = true;
}

void badge_pairing_set_red_team() {
  if (ctx == NULL) {
    return;
  }
  ctx->blue_team = false;
}