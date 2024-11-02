#include "esp_http_server.h"
#include "esp_log.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static const char *TAG = "WebServer";

// Global variable to store the RPM value
static int rpm_value = 1000; // Initial RPM value, can be changed by user

// Handler to serve index.html with dynamic RPM value
static esp_err_t index_get_handler(httpd_req_t *req)
{
    const char *filepath = "/spiffs/data/index.html";
    FILE *f = fopen(filepath, "r");
    if (f == NULL)
    {
        ESP_LOGE(TAG, "Failed to open file : %s", filepath);
        httpd_resp_send_404(req);
        return ESP_FAIL;
    }

    char line[256];
    while (fgets(line, sizeof(line), f))
    {
        // Check if the line contains the {{rpm}} placeholder
        char *placeholder_pos = strstr(line, "{{rpm}}");
        if (placeholder_pos)
        {
            // Create a buffer for the replaced line
            char temp[512];
            // Replace {{rpm}} with the actual value of rpm_value
            *placeholder_pos = '\0'; // Temporarily terminate the string before {{rpm}}
            snprintf(temp, sizeof(temp), "%s%d%s", line, rpm_value, placeholder_pos + strlen("{{rpm}}"));
            httpd_resp_sendstr_chunk(req, temp); // Send the replaced line
        }
        else
        {
            // Send the line as it is
            httpd_resp_sendstr_chunk(req, line);
        }
    }

    fclose(f);
    httpd_resp_sendstr_chunk(req, NULL); // End of response
    return ESP_OK;
}

// Handler for POST requests to update RPM
static esp_err_t update_post_handler(httpd_req_t *req)
{
    char buf[100];
    int received = httpd_req_recv(req, buf, req->content_len);
    if (received <= 0)
    {
        httpd_resp_send_500(req);
        return ESP_FAIL;
    }

    buf[received] = '\0'; // Null-terminate the received data
    ESP_LOGI(TAG, "Received POST data: %s", buf);

    // Extract the value of RPM from the received data (assuming it's in the format "rpm=value")
    char *rpm_param = strstr(buf, "rpm=");
    if (rpm_param)
    {
        // Convert the received string value to an integer
        int new_rpm_value = atoi(rpm_param + strlen("rpm="));

        // Store the new RPM value in the global variable
        rpm_value = new_rpm_value;

        ESP_LOGI(TAG, "Updated RPM: %d", rpm_value);
    }

    // Respond to the client
    httpd_resp_send(req, "Data is pushed to ESP32", HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

// Handler to serve style.css
static esp_err_t style_get_handler(httpd_req_t *req)
{
    const char *filepath = "/spiffs/data/style.css";
    FILE *f = fopen(filepath, "r");
    if (f == NULL)
    {
        httpd_resp_send_404(req);
        return ESP_FAIL;
    }

    char line[256];
    httpd_resp_set_type(req, "text/css"); // Set the response type to CSS
    while (fgets(line, sizeof(line), f))
    {
        httpd_resp_sendstr_chunk(req, line);
    }
    fclose(f);
    httpd_resp_sendstr_chunk(req, NULL); // End of response
    return ESP_OK;
}

// Start the webserver and register URI handlers
esp_err_t start_webserver(void)
{
    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();

    if (httpd_start(&server, &config) == ESP_OK)
    {
        // Serve the index page
        httpd_uri_t uri_index = {
            .uri = "/",
            .method = HTTP_GET,
            .handler = index_get_handler,
            .user_ctx = NULL};
        httpd_register_uri_handler(server, &uri_index);

        // Serve the CSS file
        httpd_uri_t uri_style = {
            .uri = "/style.css",
            .method = HTTP_GET,
            .handler = style_get_handler,
            .user_ctx = NULL};
        httpd_register_uri_handler(server, &uri_style);

        // Handle POST requests to update the RPM setting
        httpd_uri_t uri_post = {
            .uri = "/update",
            .method = HTTP_POST,
            .handler = update_post_handler,
            .user_ctx = NULL};
        httpd_register_uri_handler(server, &uri_post);

        ESP_LOGI(TAG, "Webserver started successfully");
    }

    return ESP_OK;
}
