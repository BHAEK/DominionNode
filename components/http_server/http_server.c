#include "esp_err.h"
#include "esp_log.h"

#include "http_server.h"

const char * html_main_page =
"<!DOCTYPE html>\
<html>\
<head>\
<title>Dominion Node</title>\
<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\
<style>\
body { font-family: sans-serif; font-size: 1.3em; padding: 20px; }\
button { font-size: 1em; padding: 10px; margin: 5px 0; }\
</style>\
</head>\
<body>\
<h2>Dominion Control Panel</h2>\
<p>Red Timer: <span id=\"red\">0</span></p>\
<p>Blue Timer: <span id=\"blue\">0</span></p>\
<form action=\"/start_red\" method=\"post\"><button>Start Red Timer</button></form>\
<form action=\"/reset\" method=\"post\"><button>Reset Timers</button></form>\
<script>\
function updateTimers() {\
  fetch('/status')\
    .then(response => response.json())\
    .then(data => {\
      document.getElementById('red').textContent = data.red_timer;\
      document.getElementById('blue').textContent = data.blue_timer;\
    });\
}\
setInterval(updateTimers, 1000);\
</script>\
</body>\
</html>";

#include "esp_random.h"
int get_red_time()
{
    return esp_random()%100;
}

int get_blue_time()
{
    return esp_random()%100;
}

esp_err_t status_get_handler(httpd_req_t *req) 
{
    char response[64];
    
    snprintf(response, sizeof(response),
             "{\"red_timer\": %d, \"blue_timer\": %d}",
             get_red_time(), get_blue_time());

    httpd_resp_set_type(req, "application/json");
    httpd_resp_send(req, response, HTTPD_RESP_USE_STRLEN);
    
    return ESP_OK;

}

esp_err_t main_page_get_handler(httpd_req_t *req) 
{
    char html_buffer[1024];
    
    snprintf(html_buffer, sizeof(html_buffer), html_main_page, get_red_time(), get_blue_time());
    httpd_resp_set_type(req, "text/html");
    httpd_resp_send(req, html_buffer, HTTPD_RESP_USE_STRLEN);
    
    return ESP_OK;

}

esp_err_t start_red_post_handler(httpd_req_t *req) 
{
    
    // logica per avviare timer rosso
    ESP_LOGI(__func__, "TIMER STARTED");
    httpd_resp_sendstr(req, "OK");
    
    return ESP_OK;

}

esp_err_t reset_post_handler(httpd_req_t *req) 
{
    
    ESP_LOGI(__func__, "TIMER RESETTED");
    httpd_resp_sendstr(req, "Timers reset");
    
    return ESP_OK;

}

void start_http_server(void) 
{
    
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    httpd_handle_t server = NULL;
    httpd_start(&server, &config);

    httpd_uri_t main_page_uri = 
    {
        .uri = "/",
        .method = HTTP_GET,
        .handler = main_page_get_handler
    };
    
    httpd_register_uri_handler(server, &main_page_uri);

    httpd_uri_t status_uri = 
    {
        .uri = "/status",
        .method = HTTP_GET,
        .handler = status_get_handler
    };
    
    httpd_register_uri_handler(server, &status_uri);

    httpd_uri_t start_red_uri = 
    {
        .uri = "/start_red",
        .method = HTTP_POST,
        .handler = start_red_post_handler
    };
    
    httpd_register_uri_handler(server, &start_red_uri);

    httpd_uri_t reset_uri = 
    {
        .uri = "/reset",
        .method = HTTP_POST,
        .handler = reset_post_handler
    };
    
    httpd_register_uri_handler(server, &reset_uri);

}