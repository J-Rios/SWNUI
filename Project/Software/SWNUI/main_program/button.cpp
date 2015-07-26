#include "button.h"

BUTTON::BUTTON(int pin_button, int button_mode)
{
    pin = pin_button;
    mode = button_mode;
    config();
}
  
void BUTTON::config()
{
    pinMode(pin, mode);
}

int BUTTON::state()
{
    return (digitalRead(pin));
}

bool BUTTON::pressed()
{
    bool pres;
    
    if(state() == LOW)
        pres = true;
    else
        pres = false;
    
    return pres;
}

