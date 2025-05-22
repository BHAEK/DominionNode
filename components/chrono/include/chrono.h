#ifndef CHRONO_H
#define CHRONO_H

#include "stdint.h"
#include "stdbool.h"

/**
 * @file chrono.h
 * @brief Simple high-resolution stopwatch utility for tracking elapsed time.
 */

#include <stdint.h>
#include <stdbool.h>

/**
 * @brief Chrono structure to track elapsed time.
 *
 * This structure is used to measure time intervals with microsecond precision.
 * It keeps track of the start time, total accumulated time, and running state.
 */
typedef struct
{
    int64_t time_start_us;  /**< Timestamp when the stopwatch was last started (in microseconds). */
    int64_t time_total_us;  /**< Accumulated elapsed time (in microseconds). */
    bool is_running;        /**< Indicates whether the stopwatch is currently running. */
} Chrono_t;

/**
 * @brief Macro to initialize a Chrono_t instance with default values.
 *
 * @return A Chrono_t structure with zeroed time and not running.
 *
 * Example:
 * @code
 * Chrono_t my_timer = CHRONO_DEFAULT();
 * @endcode
 */
#define CHRONO_DEFAULT() (Chrono_t){ .time_start_us = 0, .time_total_us = 0, .is_running = false }

/**
 * @brief Start the stopwatch.
 *
 * If the stopwatch is already running, this function has no effect.
 *
 * @param chrono Pointer to the Chrono_t instance to start.
 */
void chrono_start(Chrono_t *chrono);

/**
 * @brief Stop the stopwatch and accumulate elapsed time.
 *
 * If the stopwatch is not running, this function has no effect.
 *
 * @param chrono Pointer to the Chrono_t instance to stop.
 */
void chrono_stop(Chrono_t *chrono);

/**
 * @brief Reset the stopwatch to zero.
 *
 * This clears all accumulated time and sets the stopwatch to a stopped state.
 *
 * @param chrono Pointer to the Chrono_t instance to reset.
 */
void chrono_reset(Chrono_t *chrono);

/**
 * @brief Get the total elapsed time in seconds.
 *
 * If the stopwatch is running, it also includes the current running time.
 *
 * @param chrono Pointer to the Chrono_t instance to query.
 * @return Total elapsed time in seconds.
 */
int chrono_get_seconds(Chrono_t *chrono);

#endif //CHRONO_H