idf_component_register(SRCS "storage.c"
                    PRIV_REQUIRES nvs_flash settings_types
                    INCLUDE_DIRS "include" "./../settings_types/include")
