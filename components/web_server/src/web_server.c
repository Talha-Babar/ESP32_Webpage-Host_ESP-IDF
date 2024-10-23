#include "esp_http_server.h"
#include "esp_log.h"
#include "web_server.h"
#include <stdio.h>

static const char *TAG = "WebServer";

// Handler to serve index.html
static esp_err_t index_get_handler(httpd_req_t *req)
{
    const char *filepath = "/spiffs/index.html";
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
        httpd_resp_sendstr_chunk(req, line);
    }
    fclose(f);
    httpd_resp_sendstr_chunk(req, NULL); // End of response
    return ESP_OK;
}

// Handler for POST requests to update settings
static esp_err_t update_post_handler(httpd_req_t *req)
{
    char buf[100];
    int received = httpd_req_recv(req, buf, req->content_len);
    if (received <= 0)
    {
        httpd_resp_send_500(req);
        return ESP_FAIL;
    }

    buf[received] = '\0';
    ESP_LOGI(TAG, "Received POST data: %s", buf);

    // Respond to client
    httpd_resp_send(req, "Settings updated", HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

// Handler to serve style.css
static esp_err_t style_get_handler(httpd_req_t *req)
{
    const char *filepath = "/spiffs/style.css";
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

// In start_webserver(), add the URI handler for style.css
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

        // Handle POST requests
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
