idf_component_register(SRCS "main.c"
                    PRIV_REQUIRES   driver error_signaling 
                                    buttons leds app storage
                                    settings wifi http_server
                                    nvs_flash display buzzer                             
                    INCLUDE_DIRS "./../config")
