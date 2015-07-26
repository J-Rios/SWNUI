#include "led.h"

LED::LED(int pin_led, int stat_led)
{
    pin = pin_led;
    stat = stat_led;
    pv = true;
    configure();
}
  
void LED::configure()
{
    pinMode(pin, OUTPUT);    
    digitalWrite(pin, !stat);
}

void LED::on()
{
    if(stat == 0)
    {
        digitalWrite(pin, LOW);
        stat = 1;
    }
}

void LED::off()
{
    if(stat == 1)
    {
        digitalWrite(pin, HIGH);
        stat = 0;
    }
}

void LED::test(unsigned long time)
{
    on();
    delay(time/4);
    off();
    
    delay(time/2);
    
    on();
    delay(time/4);
    off();
}

void LED::turn()
{
    stat = !stat;
    digitalWrite(pin, stat);
}

void LED::flash(int time)
{ 
    if(pv == true)
    {
        t0 = millis();
        pv = false;
    }
    
    t1 = millis();
    if(t1 >= t0+time)
    {
        turn();
        t0 = millis();
    }
}

int LED::state()
{
    return stat;
}


