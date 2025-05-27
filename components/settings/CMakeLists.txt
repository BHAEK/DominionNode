idf_component_register(SRCS "settings.c"
                    PRIV_REQUIRES storage settings_types display
                    INCLUDE_DIRS "include" "./../settings_types/include")
