idf_component_register(SRCS "main.c"
                    PRIV_REQUIRES driver error_signaling buttons leds app storage
                    INCLUDE_DIRS "./../config")
