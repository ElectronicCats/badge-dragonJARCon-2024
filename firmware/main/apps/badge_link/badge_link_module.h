#pragma once

typedef enum {
  BADGE_BSIDES = 0,
  BADGE_DRAGONJAR,
  BADGE_EKOPARTY,
  BADGE_BUGCON,
} badge_event_t;

typedef struct {
  badge_event_t badge_event;
  uint8_t mac_addr[6];
} badge_info_t;

const char* badge_event_to_name[] = {"BSides", "DragonJAR", "EkoParty",
                                     "BugCON"};

/**
 * @brief Initialize badge link module
 *
 * @return void
 */
void badge_link_module_begin();

/**
 * @brief Exit badge link module
 *
 * @return void
 */
void badge_link_module_exit();
