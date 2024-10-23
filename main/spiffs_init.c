#include "esp_log.h"
#include "esp_spiffs.h"

static const char *TAG = "SPIFFS";

void init_spiffs(void)
{
    esp_vfs_spiffs_conf_t conf = {
        .base_path = "/spiffs",
        .partition_label = NULL,
        .max_files = 5,
        .format_if_mount_failed = true};

    esp_err_t ret = esp_vfs_spiffs_register(&conf);
    if (ret != ESP_OK)
    {
        if (ret == ESP_FAIL)
        {
            ESP_LOGE(TAG, "Failed to mount or format SPIFFS");
        }
        else if (ret == ESP_ERR_NOT_FOUND)
        {
            ESP_LOGE(TAG, "Failed to find SPIFFS partition");
        }
        else
        {
            ESP_LOGE(TAG, "Failed to initialize SPIFFS (%s)", esp_err_to_name(ret));
        }
        return;
    }

    size_t total = 0, used = 0;
    ret = esp_spiffs_info(NULL, &total, &used);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to get SPIFFS partition information (%s)", esp_err_to_name(ret));
    }
    else
    {
        ESP_LOGI(TAG, "SPIFFS mounted successfully. Total: %d bytes, Used: %d bytes", total, used);
    }
}

// void list_spiffs_files(void)
// {
//     DIR *dir = opendir("/spiffs");
//     if (dir == NULL)
//     {
//         ESP_LOGE("SPIFFS_LIST", "Failed to open SPIFFS directory");
//         return;
//     }

//     struct dirent *entry;
//     while ((entry = readdir(dir)) != NULL)
//     {
//         ESP_LOGI("SPIFFS_LIST", "Found file: %s", entry->d_name);
//     }

//     closedir(dir);
// }
