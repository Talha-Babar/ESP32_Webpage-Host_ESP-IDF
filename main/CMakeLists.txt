idf_component_register(SRCS "main.c"
                          "spiffs_init.c"
                      INCLUDE_DIRS "."
                      REQUIRES web_server nvs_flash spiffs)

                      # Add SPIFFS image generation
# spiffs_create_partition_image(spiffs spiffs/data FLASH_IN_PROJECT)