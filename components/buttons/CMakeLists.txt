idf_component_register(SRCS "buttons.c"
                    PRIV_REQUIRES esp_driver_gpio app error_signaling
                    INCLUDE_DIRS "include" "./../../config")
