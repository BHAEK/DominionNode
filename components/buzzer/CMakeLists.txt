idf_component_register(SRCS "buzzer.c"
                    PRIV_REQUIRES esp_driver_gpio settings
                    INCLUDE_DIRS "include" "./../../config")
