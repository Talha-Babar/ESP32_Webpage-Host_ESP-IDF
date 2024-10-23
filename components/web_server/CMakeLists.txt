idf_component_register(SRCS "src/web_server.c" "src/wifi_ap.c" "src/settings_manager.c"
INCLUDE_DIRS "include"
PRIV_REQUIRES esp_http_server esp_wifi nvs_flash mdns)
