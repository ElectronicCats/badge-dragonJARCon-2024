idf_component_register(SRCS "leds_d.c" "leds.c"
                    REQUIRES "driver" ledc_controller
                    INCLUDE_DIRS "include")
