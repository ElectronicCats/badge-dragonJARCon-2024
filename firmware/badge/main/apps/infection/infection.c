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
#include "infection_cmd.h"
#include "infection_screens.h"
#include "menus_module.h"
#include "nvs_flash.h"
#include "preferences.h"
#include "vaccine_builder/vaccine_builder.h"

static infection_ctx_t* ctx = NULL;

static void infection_task();
static void infection_show_screen(infection_event_t event, void* ctx);
static void save_patient_state();
static void send_vaccine_req_cmd(uint8_t* addr);

static int get_random_int() {
  uint32_t entropy = esp_random();
  uint64_t time_since_boot = esp_timer_get_time();
  uint32_t seed = (entropy ^ time_since_boot) & 0xFFFFFFFF;
  srand(seed);

  return rand();
}

static uint8_t get_random_uint8() {
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

void get_infected() {
  printf("INFECTED\n");
  ctx->patient->state = INFECTED;
  ctx->patient->virus = get_random_virus();
  ctx->patient->remaining_time = LIFE_TIME;
  menus_module_hide_menu(MENU_INFECTION_VACCINES);
  menus_module_reveal_menu(MENU_INFECTION_VACCINES_GET);
  if (menus_module_get_current_menu() != MENU_INFECTION_STATE) {
    menus_module_set_menu(MENU_INFECTION_STATE);
  }
  xTaskCreate(infection_task, "infection_task", 4096, NULL, 10, NULL);
}

static void virus_cmd_handler(badge_connect_recv_msg_t* msg) {
  if (ctx->patient->state == HEALTY) {
    ctx->patient->state = IDLE;
    engine_infection_alert();
    // get_infected();
  }
}

static void print_vaccine(vaccine_t vaccine) {
  printf("Vaccine: \n");
  printf("\t- ARN: %s\n", arn_str[vaccine.arn]);
  printf("\t- Viral Code: %s\n", viral_code_str[vaccine.viral_code]);
  printf("\t- Lipid Layer: %s\n", lipid_layer_str[vaccine.lipid_layer]);
}

static void vaccine_req_cmd_handler(badge_connect_recv_msg_t* msg) {
  vaccine_req_cmd_t cmd = *((vaccine_req_cmd_t*) msg->data);
  vaccine_t vaccine = cmd.vaccine;
  print_vaccine(vaccine);
  if (memcmp(&vaccine, &cure_1, sizeof(vaccine_t)) == 0) {
    printf("Cura 1\n");
  } else if (memcmp(&vaccine, &cure_2, sizeof(vaccine_t)) == 0) {
    printf("Cura 2\n");
  } else if (memcmp(&vaccine, &cure_3, sizeof(vaccine_t)) == 0) {
    printf("Cura 3\n");
  } else {
    printf("Sin efecto\n");
  }
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

static void send_vaccine_req_cmd() {
  vaccine_req_cmd_t vaccine_cmd;
  vaccine_cmd.cmd = VACCINE_REQ_CMD;
  vaccine_cmd.vaccine = *ctx->vaccine;
  badge_connect_send(badge_pairing_get_friend_addr(), &vaccine_cmd,
                     sizeof(vaccine_req_cmd_t));
}

static void infection_task() {
  while (1) {
    vTaskDelay(pdMS_TO_TICKS(1000));
    infection_display_status();
    save_patient_state();
    if (ctx->patient->state >= INFECTED) {
      send_virus_cmd();
      if(ctx->patient->remaining_time > 0){
        ctx->patient->remaining_time--;
      }
    }
  }
  vTaskDelete(NULL);
}

static void save_patient_state() {
  preferences_put_ushort(STATE_MEM, ctx->patient->state);
  preferences_put_ushort(VIRUS_MEM, ctx->patient->virus);
  preferences_put_ushort(LIFETIME_MEM, ctx->patient->remaining_time);
  preferences_put_ushort(FRIENDS_SAVED_MEM, ctx->patient->friends_saved_count);
}

static void load_patient_state() {
  ctx = malloc(sizeof(infection_ctx_t));
  ctx->patient = calloc(1, sizeof(patient_t));
  ctx->vaccine = calloc(1, sizeof(vaccine_t));

  ctx->patient->state = preferences_get_ushort(STATE_MEM, 0);
  ctx->patient->virus = preferences_get_ushort(VIRUS_MEM, 0);
  ctx->patient->remaining_time =
      preferences_get_ushort(LIFETIME_MEM, LIFE_TIME);
  ctx->patient->friends_saved_count =
      preferences_get_ushort(FRIENDS_SAVED_MEM, 0);
  esp_wifi_get_mac(WIFI_IF_STA, ctx->patient->mac);
  if (ctx->patient->state >= INFECTED) {
    xTaskCreate(infection_task, "infection_task", 4096, NULL, 10, NULL);
  }

  if (ctx->patient->state >= INFECTED) {
    menus_module_hide_menu(MENU_INFECTION_VACCINES);
    menus_module_reveal_menu(MENU_INFECTION_VACCINES_GET);
  } else {
    menus_module_reveal_menu(MENU_INFECTION_VACCINES);
    menus_module_hide_menu(MENU_INFECTION_VACCINES_GET);
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
  // get_infected();
}

static void infection_show_screen(infection_event_t event, void* ctx) {
  infection_screens_handler(event, ctx);
}

void infection_start_pairing() {
  badge_pairing_set_callbacks(send_vaccine_req_cmd, NULL, NULL);
  badge_pairing_init();
}

void infection_stop_pairing() {
  badge_pairing_set_callbacks(NULL, NULL, NULL);
  badge_pairing_deinit();
}

void infection_vaccine_builder_mRNA() {
  vaccine_builder_begin(mRNA_COMP, &ctx->vaccine->arn);
}
void infection_vaccine_builder_viral_code() {
  vaccine_builder_begin(VIRAL_CODE_COMP, &ctx->vaccine->viral_code);
}
void infection_vaccine_builder_Lipid_layer() {
  vaccine_builder_begin(LIPID_LAYER_COMP, &ctx->vaccine->lipid_layer);
}