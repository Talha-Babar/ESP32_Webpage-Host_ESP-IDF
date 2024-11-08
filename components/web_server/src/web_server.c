#include "esp_http_server.h"
#include "esp_log.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static const char *TAG = "WebServer";

// Variables for 5 buttons and 5 sliders
static int button_rpm[5] = {1000, 1000, 1000, 1000, 1000};
static int slider_rpm[5] = {1000, 1000, 1000, 1000, 1000};

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

    char line[512];
    while (fgets(line, sizeof(line), f))
    {
        char temp[1024];
        snprintf(temp, sizeof(temp), line,
                 button_rpm[0], slider_rpm[0],
                 button_rpm[1], slider_rpm[1],
                 button_rpm[2], slider_rpm[2],
                 button_rpm[3], slider_rpm[3],
                 button_rpm[4], slider_rpm[4]);
        httpd_resp_sendstr_chunk(req, temp); // Send each line with replaced values
    }

    fclose(f);
    httpd_resp_sendstr_chunk(req, NULL); // End of response
    return ESP_OK;
}

static esp_err_t style_get_handler(httpd_req_t *req)
{
    const char *filepath = "/spiffs/data/style.css"; // Ensure this is the correct path
    FILE *f = fopen(filepath, "r");
    if (f == NULL)
    {
        ESP_LOGE(TAG, "Failed to open file : %s", filepath);
        httpd_resp_send_404(req);
        return ESP_FAIL;
    }

    // Set the correct content type for CSS
    httpd_resp_set_type(req, "text/css");

    // Send the file content as the response
    char line[512];
    while (fgets(line, sizeof(line), f))
    {
        httpd_resp_sendstr_chunk(req, line);
    }

    fclose(f);
    httpd_resp_sendstr_chunk(req, NULL); // End of response
    return ESP_OK;
}

static esp_err_t update_button_handler(httpd_req_t *req)
{
    char buf[100];
    int received = httpd_req_recv(req, buf, req->content_len);
    if (received <= 0)
    {
        httpd_resp_send_500(req);
        return ESP_FAIL;
    }

    buf[received] = '\0';
    int button_id, new_rpm;
    sscanf(buf, "button=%d&rpm=%d", &button_id, &new_rpm);

    if (button_id >= 1 && button_id <= 5)
    {
        button_rpm[button_id - 1] = new_rpm;
        ESP_LOGI(TAG, "Updated Button %d RPM: %d", button_id, new_rpm);
    }

    httpd_resp_send(req, "Button RPM updated", HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

// Handler for POST requests to update slider RPMs
static esp_err_t update_slider_handler(httpd_req_t *req)
{
    char buf[100];
    int received = httpd_req_recv(req, buf, req->content_len);
    if (received <= 0)
    {
        httpd_resp_send_500(req);
        return ESP_FAIL;
    }

    buf[received] = '\0';
    int slider_id, new_rpm;
    sscanf(buf, "slider=%d&rpm=%d", &slider_id, &new_rpm);

    if (slider_id >= 1 && slider_id <= 5)
    {
        slider_rpm[slider_id - 1] = new_rpm;
        ESP_LOGI(TAG, "Updated Slider %d RPM: %d", slider_id, new_rpm);
    }

    httpd_resp_send(req, "Slider RPM updated", HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

static esp_err_t get_values_handler(httpd_req_t *req)
{
    char response[512];
    snprintf(response, sizeof(response),
             "{"
             "\"button_rpm_1\": %d, \"slider_rpm_1\": %d, "
             "\"button_rpm_2\": %d, \"slider_rpm_2\": %d, "
             "\"button_rpm_3\": %d, \"slider_rpm_3\": %d, "
             "\"button_rpm_4\": %d, \"slider_rpm_4\": %d, "
             "\"button_rpm_5\": %d, \"slider_rpm_5\": %d"
             "}",
             button_rpm[0], slider_rpm[0],
             button_rpm[1], slider_rpm[1],
             button_rpm[2], slider_rpm[2],
             button_rpm[3], slider_rpm[3],
             button_rpm[4], slider_rpm[4]);

    httpd_resp_set_type(req, "application/json");
    httpd_resp_send(req, response, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

// Start the webserver and register URI handlers
esp_err_t start_webserver(void)
{
    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();

    if (httpd_start(&server, &config) == ESP_OK)
    {
        httpd_uri_t uri_index = {
            .uri = "/",
            .method = HTTP_GET,
            .handler = index_get_handler,
            .user_ctx = NULL};
        httpd_register_uri_handler(server, &uri_index);

        httpd_uri_t uri_style = {
            .uri = "/style.css",
            .method = HTTP_GET,
            .handler = style_get_handler,
            .user_ctx = NULL};
        httpd_register_uri_handler(server, &uri_style);

        httpd_uri_t uri_update_button = {
            .uri = "/update_button",
            .method = HTTP_POST,
            .handler = update_button_handler,
            .user_ctx = NULL};
        httpd_register_uri_handler(server, &uri_update_button);

        httpd_uri_t uri_update_slider = {
            .uri = "/update_slider",
            .method = HTTP_POST,
            .handler = update_slider_handler,
            .user_ctx = NULL};
        httpd_register_uri_handler(server, &uri_update_slider);

        httpd_uri_t uri_get_values = {
            .uri = "/get_values",
            .method = HTTP_GET,
            .handler = get_values_handler,
            .user_ctx = NULL};
        httpd_register_uri_handler(server, &uri_get_values);

        ESP_LOGI(TAG, "Webserver started successfully");
    }

    return ESP_OK;
}
