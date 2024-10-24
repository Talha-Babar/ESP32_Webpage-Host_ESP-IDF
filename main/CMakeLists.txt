idf_component_register(SRCS "main.c"
                          "spiffs_init.c"
                      INCLUDE_DIRS "."
                      REQUIRES web_server nvs_flash spiffs)

                      spiffs_create_partition_image(storage ../spiffs FLASH_IN_PROJECT)