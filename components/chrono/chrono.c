#include "esp_timer.h"
#include "chrono.h"

void chrono_start(Chrono_t * chrono)
{
    if (!chrono->is_running)
    {
        chrono->time_start_us = esp_timer_get_time();
        chrono->is_running = true;
    }
}

void chrono_stop(Chrono_t * chrono)
{
    if (chrono->is_running)
    {
        int64_t now = esp_timer_get_time();
        chrono->time_total_us += (now - chrono->time_start_us);
        chrono->is_running = false;
    }
}

void chrono_reset(Chrono_t * chrono)
{
    chrono->time_total_us = 0;
    if (chrono->is_running)
        chrono->time_start_us = esp_timer_get_time();
}

int chrono_get_seconds(Chrono_t * chrono)
{
    if (chrono->is_running)
    {
        int64_t now = esp_timer_get_time();
        return (chrono->time_total_us + (now - chrono->time_start_us)) / 1000000;
    }
    else
    {
        return chrono->time_total_us / 1000000;
    }
}
