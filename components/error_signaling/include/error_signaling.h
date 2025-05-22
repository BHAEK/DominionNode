#ifndef ERROR_SIGNALING_H
#define ERROR_SIGNALIG_H

typedef enum 
{
    INIT_ERROR,
    BUTTON_ERROR,
} App_error_t ;

void signal_fatal_error(App_error_t error);

#endif // ERROR_SIGNALING_H