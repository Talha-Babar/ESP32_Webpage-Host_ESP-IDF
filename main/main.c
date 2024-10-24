#include <stdio.h>
#include "esp_log.h"
#include "nvs_flash.h"
#include "web_server.h"
#include "wifi_ap.h"
#include "spiffs_init.h"

static const char *TAG = "app_main";

void app_main(void)
{
    // Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // Initialize SPIFFS
    init_spiffs();
    list_spiffs_files();

    // Initialize Wi-Fi Access Point
    wifi_init_softap();

    // Start the Web Server
    start_webserver();

    ESP_LOGI(TAG, "Application started successfully");
}
