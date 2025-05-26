idf_component_register(SRCS "main.c"
                    PRIV_REQUIRES   driver error_signaling 
                                    buttons leds app storage 
                                    nvs_flash wifi tm1637                                    
                    INCLUDE_DIRS "./../config")
