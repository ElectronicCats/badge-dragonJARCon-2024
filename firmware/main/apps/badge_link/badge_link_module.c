#include "esp_log.h"
#include "esp_wifi.h"
#include "nvs_flash.h"

#include "badge_connect.h"
#include "badge_link_bitmaps.h"
#include "badge_link_module.h"
#include "badge_link_screens_module.h"
#include "menus_screens.h"
#include "preferences.h"

#define SEND_DATA_DELAY_MS 100
#define SEND_DATA_TIMEOUT  SEND_DATA_DELAY_MS * 15 / 10  // 15 Seconds

static const char* TAG = "badge_link_module";
TaskHandle_t badge_link_state_machine_task_handle;
TaskHandle_t badge_link_stop_badge_connect_task_handle;
uint8_t send_data_timeout = SEND_DATA_TIMEOUT;
bool badge_link_send_data = false;

badge_link_screens_status_t badge_link_status = BADGE_LINK_SCANNING;
badge_link_screens_status_t badge_link_status_previous =
    BADGE_LINK_UNLOCK_FEATURE;

void nvs_init() {
  esp_err_t ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES ||
      ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
  }
  ESP_ERROR_CHECK(ret);
}

void wifi_init() {
  esp_event_loop_create_default();

  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  nvs_init();

  ESP_ERROR_CHECK(esp_wifi_init(&cfg));
  ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
  ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
  ESP_ERROR_CHECK(esp_wifi_set_ps(WIFI_PS_NONE));
  ESP_ERROR_CHECK(esp_wifi_start());
}

void badge_link_stop_badge_connect_task(void* pvParameters) {
  badge_link_send_data = true;
  vTaskDelay(200 / portTICK_PERIOD_MS);
  badge_link_send_data = false;
  vTaskDelay(200 / portTICK_PERIOD_MS);
  badge_connect_deinit();

  while (true) {
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }

  vTaskDelete(NULL);
}

void stop_badge_connect_after_delay() {
  xTaskCreate(badge_link_stop_badge_connect_task,
              "badge_link_stop_badge_connect_task", 2048, NULL, 5,
              &badge_link_stop_badge_connect_task_handle);
}

void badge_link_update_found_badge_logo(badge_connect_recv_msg_t* msg) {
  if (msg->badge_type.is_bsides) {
    badge_link_screens_module_set_found_badge_logo(LOGO_BSIDES);
  } else if (msg->badge_type.is_dragonjar) {
    badge_link_screens_module_set_found_badge_logo(LOGO_DRAGONJAR);
  } else if (msg->badge_type.is_ekoparty) {
    badge_link_screens_module_set_found_badge_logo(LOGO_EKOPARTY);
  } else if (msg->badge_type.is_bugcon) {
    badge_link_screens_module_set_found_badge_logo(LOGO_BUGCON);
  }
}

/**
 * @brief Save the badge info to a list in preferences
 *
 * @param msg
 *
 * @return void
 */
void save_badge_info(badge_connect_recv_msg_t* msg) {
  badge_info_t badge_info;
  if (msg->badge_type.is_bsides) {
    badge_info.badge_event = BADGE_BSIDES;
  } else if (msg->badge_type.is_dragonjar) {
    badge_info.badge_event = BADGE_DRAGONJAR;
  } else if (msg->badge_type.is_ekoparty) {
    badge_info.badge_event = BADGE_EKOPARTY;
  } else if (msg->badge_type.is_bugcon) {
    badge_info.badge_event = BADGE_BUGCON;
  }
  memcpy(badge_info.mac_addr, msg->src_addr, 6);

  uint16_t found_badges_count = preferences_get_ushort("badges_count", 0);
  badge_info_t badge_info_list[found_badges_count + 1];

  preferences_get_bytes("badges_list", (void*) &badge_info_list,
                        sizeof(badge_info_list));

  // Verify if the badge is already in the list
  bool badge_already_found = false;
  for (int i = 0; i < found_badges_count; i++) {
    if (memcmp(badge_info_list[i].mac_addr, badge_info.mac_addr, 6) == 0) {
      badge_already_found = true;
      break;
    }
  }

  if (!badge_already_found || true) {
    badge_info_list[found_badges_count] = badge_info;
    preferences_put_bytes("badges_list", (void*) &badge_info_list,
                          sizeof(badge_info_list));
    found_badges_count++;
  } else {
    ESP_LOGW(TAG, "Badge already found");
  }

  ESP_LOGI(TAG, "Badges found: %d, badge list size: %u", found_badges_count,
           sizeof(badge_info_list));
  for (int i = 0; i < found_badges_count; i++) {
    ESP_LOGI(TAG, "Badge %d: %02X:%02X:%02X:%02X:%02X:%02X", i,
             badge_info_list[i].mac_addr[0], badge_info_list[i].mac_addr[1],
             badge_info_list[i].mac_addr[2], badge_info_list[i].mac_addr[3],
             badge_info_list[i].mac_addr[4], badge_info_list[i].mac_addr[5]);
  }

  preferences_put_ushort("badges_count", found_badges_count);
}

// Check badge_connect_recv_msg_t struct in badge_connect.h to see what you can
// get from the received message
void badge_link_receive_data_cb(badge_connect_recv_msg_t* msg) {
  char* data = (char*) msg->data;
  data[msg->data_size] = '\0';
  ESP_LOGI(TAG, "Received data: %s, RSSI: %d", data, msg->rx_ctrl->rssi);

  bool is_hello_world = strcmp(data, "Hello world") == 0;

  if (is_hello_world && badge_link_status != BADGE_LINK_FOUND_TEXT) {
    if (msg->rx_ctrl->rssi > -100 && msg->rx_ctrl->rssi <= -60) {
      badge_link_status = BADGE_LINK_BRING_IT_CLOSER;
    } else if (msg->rx_ctrl->rssi > -60) {
      badge_link_status = BADGE_LINK_FOUND_TEXT;
      vTaskSuspend(badge_link_screens_module_scan_task_handle);
      stop_badge_connect_after_delay();
      badge_link_update_found_badge_logo(msg);
      preferences_put_bool("badge_found", true);  // Unlock feature
      save_badge_info(msg);
    }
  }
}

void badge_link_module_send_data() {
  // Decreased every SEND_DATA_DELAY_MS
  send_data_timeout = send_data_timeout > 0 ? send_data_timeout - 1 : 0;

  if (send_data_timeout / 10 == 0 &&
      (badge_link_status == BADGE_LINK_SCANNING ||
       badge_link_status == BADGE_LINK_BRING_IT_CLOSER)) {
    badge_link_status = BADGE_LINK_NOT_FOUND;
    vTaskSuspend(badge_link_screens_module_scan_task_handle);
    badge_connect_deinit();
    return;
  }

  if (badge_link_status == BADGE_LINK_SCANNING ||
      badge_link_status == BADGE_LINK_BRING_IT_CLOSER || badge_link_send_data) {
    char* data = "Hello world";
    uint8_t* addr = ESPNOW_ADDR_BROADCAST;  // Send to all badges
    badge_connect_send(addr, data, strlen(data));
  }

  vTaskDelay(SEND_DATA_DELAY_MS / portTICK_PERIOD_MS);
}

void badge_link_keyboard_cb(uint8_t button_name, uint8_t button_event) {
  ESP_LOGI(TAG, "Button name: %d, event: %d", button_name, button_event);
  if (button_event != BUTTON_PRESS_DOWN) {
    return;
  }

  switch (button_name) {
    case BUTTON_LEFT:
      if (badge_link_status != BADGE_LINK_FOUND_TEXT &&
          badge_link_status != BADGE_LINK_FOUND_LOGO) {
        badge_link_module_exit();
      }
      break;
    case BUTTON_RIGHT:
      switch (badge_link_status) {
        case BADGE_LINK_FOUND_TEXT:
          badge_link_status = BADGE_LINK_FOUND_LOGO;
          break;
        case BADGE_LINK_FOUND_LOGO:
          badge_link_status = BADGE_LINK_UNLOCK_FEATURE;
          break;
        case BADGE_LINK_NOT_FOUND:
        case BADGE_LINK_UNLOCK_FEATURE:
          badge_link_module_exit();
          break;
        default:
          break;
      }
      break;
    default:
      break;
  }
}

void badge_link_reset_status() {
  badge_link_status = BADGE_LINK_SCANNING;
  badge_link_status_previous = BADGE_LINK_UNLOCK_FEATURE;
  send_data_timeout = SEND_DATA_TIMEOUT;
  badge_link_send_data = false;
}

void badge_link_state_machine_task(void* pvParameters) {
  badge_link_reset_status();

  while (true) {
    if (badge_link_status != badge_link_status_previous) {
      ESP_LOGI(TAG, "Badge link status: %s",
               badge_link_status_strings[badge_link_status]);
      badge_link_screens_module_display_status(badge_link_status);
      badge_link_status_previous = badge_link_status;
    }
    badge_link_module_send_data();
  }
}

void badge_link_module_begin() {
  ESP_LOGI(TAG, "Badge link module begin");
  menus_module_set_app_state(true, badge_link_keyboard_cb);
  wifi_init();  // Needed to work with espnow
  badge_connect_init();
  badge_connect_register_recv_cb(badge_link_receive_data_cb);
  // Set the badge type to BSides, DragonJAR, Ekoparty, or BugCon
  // See README.md or badge_connect.h for more information
  // badge_connect_set_bsides_badge();
  badge_connect_set_dragonjar_badge();
  // badge_connect_set_ekoparty_badge();
  // badge_connect_set_bugcon_badge();
  xTaskCreate(badge_link_state_machine_task, "badge_link_state_machine_task",
              4096, NULL, 4, &badge_link_state_machine_task_handle);
}

void badge_link_module_exit() {
  ESP_LOGI(TAG, "Badge link module exit");
  menus_module_set_reset_screen(MENU_BADGE_FINDER_SCAN);
  esp_restart();
}
