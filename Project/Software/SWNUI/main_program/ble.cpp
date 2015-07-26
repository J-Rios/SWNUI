#include "ble.h"

BLE::BLE()
{
    _bauds = BLE_BAUDS;
    Serial.begin(_bauds);
}

void BLE::conect()
{
    char conectado = NUL;
    
    while(conectado != CON)
    {
        if(Serial.available())
            conectado = Serial.read();
        delay(200);
    }
    Serial.write(conectado);
}

void BLE::flush()
{
    Serial.flush();
}

void BLE::send(char c)
{
    Serial.write(c);
}

void BLE::print(String s)
{
    Serial.print(s);
}

void BLE::println(String s)
{
    Serial.println(s);
}

