#include "esp_err.h"
#include "esp_log.h"

#include "http_server.h"
#include "app.h"

const char * html_main_page =
"<!DOCTYPE html>\n"
"<html>\n"
"<head>\n"
"<title>Dominion Node</title>\n"
"<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\n"
"<style>\n"
"body { font-family: sans-serif; font-size: 1.3em; padding: 20px; background-color: #f9f9f9; }\n"
".timer { font-size: 2em; font-weight: bold; margin: 10px 0; }\n"
".red { color: #d32f2f; }\n"
".blue { color: #1976d2; }\n"
"button { font-size: 1em; padding: 12px 20px; margin: 10px 5px 0 0; border: none; border-radius: 5px; cursor: pointer; }\n"
".start { background-color: #4caf50; color: white; }\n"
".stop { background-color: #f44336; color: white; }\n"
".reset { background-color: #9e9e9e; color: white; }\n"
"</style>\n"
"</head>\n"
"<body>\n"
"<h2>Dominion Control Panel</h2>\n"
"<div class=\"timer red\">Red Timer: <span id=\"red\">0</span>s</div>\n"
"<div class=\"timer blue\">Blue Timer: <span id=\"blue\">0</span>s</div>\n"
"<button id=\"startBtn\" class=\"start\" onclick=\"startGame()\">Start Game</button>\n"
"<button class=\"endBtn\" onclick=\"endGame()\">End Game</button>\n"
"<script>\n"
"function updateTimers() {\n"
"  fetch('/status')\n"
"    .then(response => response.json())\n"
"    .then(data => {\n"
"      document.getElementById('red').textContent = data.red_timer;\n"
"      document.getElementById('blue').textContent = data.blue_timer;\n"
"    });\n"
"}\n"
"function startGame() {\n"
"  fetch('/start', { method: 'POST' })\n"
"}\n"
"function endGame() {\n"
"  fetch('/end', { method: 'POST' })\n"
"}\n"
"setInterval(updateTimers, 500);\n"
"</script>\n"
"</body>\n"
"</html>\n";

int get_red_time()
{
    return app_get_timer(APP_TEAM_RED);
}

int get_blue_time()
{
    return app_get_timer(APP_TEAM_BLUE);
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
    
    httpd_resp_set_type(req, "text/html");
    httpd_resp_send(req, html_main_page, HTTPD_RESP_USE_STRLEN);
    
    return ESP_OK;

}

esp_err_t start_post_handler(httpd_req_t *req) 
{
    
    ESP_LOGI(__func__, "GAME START BUTTON PRESSED");
    if(app_get_state() == APP_STATE_FINISHED || app_get_state() == APP_STATE_INIT) 
    {
        app_start_game();
        httpd_resp_sendstr(req, "OK");
    }
    else
    {
        httpd_resp_sendstr(req, "FAIL");
    }
    
    return ESP_OK;

}

esp_err_t end_post_handler(httpd_req_t *req) 
{
    
    ESP_LOGI(__func__, "GAME END BUTTON PRESSED");
    if(app_get_state() == APP_STATE_RUNNING_RED || app_get_state() == APP_STATE_RUNNING_BLUE)
    {
        app_end_game(); 
        httpd_resp_sendstr(req, "OK");
    }
    else
    {
        httpd_resp_sendstr(req, "FAIL");
    }
    
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

    httpd_uri_t start_uri = 
    {
        .uri = "/start",
        .method = HTTP_POST,
        .handler = start_post_handler
    };
    
    httpd_register_uri_handler(server, &start_uri);

    httpd_uri_t end_uri = 
    {
        .uri = "/end",
        .method = HTTP_POST,
        .handler = end_post_handler
    };
    
    httpd_register_uri_handler(server, &end_uri);

}