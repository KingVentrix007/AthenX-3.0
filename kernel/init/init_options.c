#include "string.h"
#include "stdint.h"
#include "stdbool.h"

bool tui_enabled = false;


bool get_tui_state()
{
    return tui_enabled;
}
bool set_tui(bool state)
{
    tui_enabled = state;
    return tui_enabled;
}