#include "settings_types.h"

const char * control_point_to_string(ControlPoint_t control_point) 
{
    switch (control_point) 
    {
        case CONTROL_POINT_ALPHA:   return "Alpha";
        case CONTROL_POINT_BRAVO:   return "Bravo";
        case CONTROL_POINT_CHARLIE: return "Charlie";
        case CONTROL_POINT_DELTA:   return "Delta";
        case CONTROL_POINT_ECHO:    return "Echo";
        case CONTROL_POINT_NONE:    return "None";
        default:                    return "Unknown";
    }
}

const char * generic_enable_to_string(bool enable)
{
    if(enable)
    {
        return "ON";
    }
    else
    {
        return "OFF";
    }
}