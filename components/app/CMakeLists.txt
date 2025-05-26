idf_component_register(SRCS "app.c"
                    PRIV_REQUIRES   error_signaling leds 
                                    chrono storage settings 
                                    display
                    INCLUDE_DIRS "include")
