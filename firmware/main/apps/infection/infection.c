#include "infection.h"
#include "engine.h"

#include "badge_connect.h"
#include "badge_pairing.h"
#include "esp_log.h"
#include "esp_random.h"
#include "esp_timer.h"
#include "esp_wifi.h"
#include "espnow.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "general_submenu.h"
#include "infection_cmd.h"
#include "infection_scenes.h"
#include "infection_screens.h"
#include "menus_module.h"
#include "nvs_flash.h"
#include "preferences.h"
#include "vaccination.h"

#define INFECTION_PROBABILITY       100
#define INMUNITY_DAMAGE_PROBABILITY 2
#define INMUNITY_MIN_VALUE          100
#define TRANSMISSION_PROBABILITY    3

static infection_ctx_t* ctx = NULL;

static void infection_task();
static void infection_show_screen(infection_event_t event, void* ctx);
static void save_patient_state();
void send_vaccine_req_cmd();
static void send_vaccine_res_cmd();

static int get_random_int() {
  uint32_t entropy = esp_random();
  uint64_t time_since_boot = esp_timer_get_time();
  uint32_t seed = (entropy ^ time_since_boot) & 0xFFFFFFFF;
  srand(seed);

  return rand();
}

uint8_t get_random_uint8() {
  return get_random_int() % 256;
}

static bool get_random_bool() {
  return get_random_int() % 2;
}

static uint8_t get_random_virus() {
  return get_random_int() % VIRUS_NUM;
}

void infection_display_status() {
  infection_show_screen(SHOW_INFECTION_STATE_EV, ctx);
}

void infection_get_infected() {
  ctx->patient->state = INFECTED;
  ctx->patient->virus = get_random_virus();
  ctx->patient->remaining_time = LIFE_TIME;
  xTaskCreate(infection_task, "infection_task", 4096, NULL, 10, NULL);
  infection_scenes_vaccines_receiver_help();
}

static void virus_cmd_handler(badge_connect_recv_msg_t* msg) {
  if (ctx->patient->inmunity > 0) {
    if (!(get_random_uint8() % INMUNITY_DAMAGE_PROBABILITY)) {
      ctx->patient->inmunity--;
    }
    // printf("INMUNITY: %d\n", ctx->patient->inmunity);
    return;
  }
  if (!ctx->patient->init) {
    ctx->patient->init = true;
    save_patient_state();
  }
  if (ctx->patient->state == HEALTY) {
    if (!(get_random_uint8() % INFECTION_PROBABILITY)) {
      ctx->patient->state = INFECTED;
      ctx->patient->virus = get_random_virus();
      save_patient_state();
      vaccination_exit();
      engine_infection_alert();
    }
    // infection_get_infected();
  }
}
static void print_vaccine(vaccine_t vaccine) {
  printf("Vaccine: \n");
  printf("\t- ARN: %s\n", arn_str[vaccine.arn]);
  printf("\t- Viral Code: %s\n", viral_code_str[vaccine.viral_code]);
  printf("\t- Lipid Layer: %s\n", lipid_layer_str[vaccine.lipid_layer]);
}

void infection_get_vaccinated() {
  vaccination_exit();
  ctx->patient->state = VACCINATED;
  ctx->patient->remaining_time = LIFE_TIME;
  preferences_put_ushort(STATE_MEM, ctx->patient->state);
  preferences_put_ushort(LIFETIME_MEM, ctx->patient->remaining_time);
  genera_screen_display_card_information("Curado", "Ayuda a otros");
  vTaskDelay(pdMS_TO_TICKS(2500));
  infection_scenes_vaccines_builder_help();
}

static void vaccine_req_cmd_handler(badge_connect_recv_msg_t* msg) {
  if (!(ctx->patient->state >= INFECTED)) {
    return;
  }
  vaccine_req_cmd_t cmd = *((vaccine_req_cmd_t*) msg->data);
  uint8_t* addr = msg->src_addr;
  vaccine_t vaccine = cmd.vaccine;
  // print_vaccine(vaccine);
  if (memcmp(&vaccine, &cure_1, sizeof(vaccine_t)) == 0 &&
      ctx->patient->virus == VIRUS_1) {
    send_vaccine_res_cmd(addr);
    infection_get_vaccinated();
  } else if (memcmp(&vaccine, &cure_2, sizeof(vaccine_t)) == 0 &&
             ctx->patient->virus == VIRUS_2) {
    send_vaccine_res_cmd(addr);
    infection_get_vaccinated();
  } else if (memcmp(&vaccine, &cure_3, sizeof(vaccine_t)) == 0 &&
             ctx->patient->virus == VIRUS_3) {
    send_vaccine_res_cmd(addr);
    infection_get_vaccinated();
  } else {
    vaccination_exit();
    genera_screen_display_card_information("Sin efecto", "");
    vTaskDelay(pdMS_TO_TICKS(2500));
    infection_scenes_vaccines_receiver_menu();
  }
}

static void vaccine_res_cmd_handler(badge_connect_recv_msg_t* msg) {
  if (ctx->patient->state >= INFECTED) {
    return;
  }
  // printf("%s: %d\n", __func__, __LINE__);
  ctx->patient->friends_saved_count++;
  save_patient_state();
}

static void infection_cmd_handler(badge_connect_recv_msg_t* msg) {
  infection_cmds_t cmd = *((infection_cmds_t*) msg->data);
  // printf("CMD: %d\n", cmd);
  switch (cmd) {
    case VIRUS_CMD:
      virus_cmd_handler(msg);
      break;
    case VACCINE_REQ_CMD:
      vaccine_req_cmd_handler(msg);
      break;
    case VACCINE_RES_CMD:
      vaccine_res_cmd_handler(msg);
      break;
    default:
      ping_handler(msg);
      break;
  }
}

static void nvs_init() {
  esp_err_t ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES ||
      ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
  }
  ESP_ERROR_CHECK(ret);
}
static void wifi_init() {
  esp_event_loop_create_default();

  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  nvs_init();

  ESP_ERROR_CHECK(esp_wifi_init(&cfg));
  ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
  ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
  ESP_ERROR_CHECK(esp_wifi_set_ps(WIFI_PS_NONE));
  ESP_ERROR_CHECK(esp_wifi_start());
}

static void send_virus_cmd() {
  virus_cmd_t virus_cmd;
  virus_cmd.cmd = VIRUS_CMD;
  badge_connect_send(ESPNOW_ADDR_BROADCAST, &virus_cmd, sizeof(virus_cmd_t));
}

void send_vaccine_req_cmd() {
  if (ctx->patient->state >= INFECTED) {
    return;
  }
  // printf("%s: %d\n", __func__, __LINE__);
  vaccine_req_cmd_t vaccine_cmd;
  vaccine_cmd.cmd = VACCINE_REQ_CMD;
  vaccine_cmd.vaccine = *ctx->vaccine;
  badge_connect_send(badge_pairing_get_friend_addr(), &vaccine_cmd,
                     sizeof(vaccine_req_cmd_t));
  vaccination_exit();
  genera_screen_display_notify_information("Vacuna enviada", "");
  vTaskDelay(pdMS_TO_TICKS(2000));
  infection_scenes_vaccines_builder_menu();
}

static void send_vaccine_res_cmd(uint8_t* addr) {
  // printf("%s: %d\n", __func__, __LINE__);
  vaccine_res_cmd_t vaccine_cmd;
  vaccine_cmd.cmd = VACCINE_RES_CMD;
  badge_connect_send(addr, &vaccine_cmd, sizeof(vaccine_res_cmd_t));
}

static void infection_task() {
  while (ctx->patient->state >= INFECTED) {
    vTaskDelay(pdMS_TO_TICKS(1000));
    infection_display_status();
    save_patient_state();
    if (!(get_random_uint8() % TRANSMISSION_PROBABILITY)) {
      send_virus_cmd();
    }
    if (ctx->patient->remaining_time > 0) {
      ctx->patient->remaining_time--;
    }
  }
  vTaskDelete(NULL);
}

static void save_patient_state() {
  preferences_put_ushort(STATE_MEM, ctx->patient->state);
  preferences_put_ushort(VIRUS_MEM, ctx->patient->virus);
  preferences_put_ushort(LIFETIME_MEM, ctx->patient->remaining_time);
  preferences_put_ushort(FRIENDS_SAVED_MEM, ctx->patient->friends_saved_count);
  preferences_put_bool(INIT_MEM, ctx->patient->init);
}

static void load_patient_state() {
  ctx = malloc(sizeof(infection_ctx_t));
  ctx->patient = calloc(1, sizeof(patient_t));
  ctx->vaccine = calloc(1, sizeof(vaccine_t));

  ctx->patient->init = preferences_get_bool(INIT_MEM, false);
  ctx->patient->state = preferences_get_ushort(STATE_MEM, 0);
  ctx->patient->virus = preferences_get_ushort(VIRUS_MEM, 0);
  ctx->patient->remaining_time =
      preferences_get_ushort(LIFETIME_MEM, LIFE_TIME);
  ctx->patient->friends_saved_count =
      preferences_get_ushort(FRIENDS_SAVED_MEM, 0);
  esp_wifi_get_mac(WIFI_IF_STA, ctx->patient->mac);
  ctx->patient->inmunity = 0;
  if (ctx->patient->state >= INFECTED) {
    xTaskCreate(infection_task, "infection_task", 4096, NULL, 10, NULL);
  }
}

void infection_exit() {
  save_patient_state();
  free(ctx->patient);
  free(ctx->vaccine);
  free(ctx);
}

void infection_begin() {
  wifi_init();
  badge_connect_init();
  badge_connect_register_recv_cb(infection_cmd_handler);
  badge_connect_set_bsides_badge();
  load_patient_state();
  badge_pairing_begin();
  // infection_get_infected();
}

static void infection_show_screen(infection_event_t event, void* ctx) {
  infection_screens_handler(event, ctx);
}

void infection_scenes_begin() {
  if (!ctx->patient->init) {
    genera_screen_display_card_information("Calma, todo", "a su tiempo");
    vTaskDelay(pdMS_TO_TICKS(2000));
    menus_module_exit_app();
    return;
  }
  infection_scenes_main_menu();
}

patient_state_t infection_get_patient_state() {
  return ctx->patient->state;
}

void infection_set_inmunity_time() {
  uint8_t inmunity = get_random_uint8();
  ctx->patient->inmunity = MAX(inmunity, INMUNITY_MIN_VALUE);
}

void infection_set_patient_state(patient_state_t state) {
  ctx->patient->state = state;
  preferences_put_ushort(STATE_MEM, state);
}

infection_ctx_t* infection_get_context() {
  return ctx;
}