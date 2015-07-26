#include "SWNUI.h"
  
/***********************************************************************************/

volatile unsigned long t0 = 0;
volatile unsigned long t1 = 0;
volatile bool change_m = false;

ISR(INT0_vect)
{
    t0 = t1;
    t1 = millis();
    
    if((t1-t0) < CHANGE_MODE_TIME) // Si el tiempo transcurrido entre 2 pulsaciones del boton es menor que CHANGE_MODE_TIME: cambio de modo
        change_m = true;
}

/***********************************************************************************/

SWNUI::SWNUI()
{
    mode = M_CONTINUOUS;
    state = INIT;
}

void SWNUI::init()
{
    ble = new BLE();
    led = new LED(P_LED, LOW);
    button = new BUTTON(P_BUTTON, INPUT_PULLUP);
    adxl335 = new ADXL335(P_AXIS_X, P_AXIS_Y, P_AXIS_Z);
    
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    analogReference(EXTERNAL);
    button_setInterrupt();
}

void SWNUI::update()
{
    if(mode == M_CONTINUOUS)
        mode_continuous();
    else
        mode_gesture();
}

void SWNUI::mode_continuous()
{
    changeMode_management();
    samples = adxl335->sampling();
    continuosSend();
    led_blink(1000);
    delay(T_SAMPLE);
}

void SWNUI::mode_gesture()
{
    switch(state)
    {
        default:
            state = INIT;
        break;
        
        case INIT:
                button_enableInterrupt();
                sleep();
                delay(CHANGE_MODE_TIME);
                changeMode_management();
                if(mode == M_GESTURE)
                {
                    state = SAMPLE;
                    button_disableInterrupt();
                }
        break;

        case SAMPLE:
            refreshGestureArrays();
            while(button_press())
            {
                samples = adxl_sampling();
                gestures();
                delay(T_SAMPLE);
            }
            state = SEND;
        break;

        case SEND:
            gestureRecognition();
            gestureSend();
            state = INIT;
        break;
    }
}

bool SWNUI::posVariation()
{
    bool variation;
    
    if(pos_old != samples.pos)
        variation = true;
    else
        variation = false;
    
    pos_old = samples.pos;
    
    return variation;
}

bool SWNUI::movVariation()
{
    bool variation;
    
    if(mov_old != samples.mov)
        variation = true;
    else
        variation = false;
    
    mov_old = samples.mov;
    
    return variation;
}

void SWNUI::gestures()
{
    if(posVariation() == true)
    {
        if(num_gest_pos < MAX_GEST)
        {
            gest_p[num_gest_pos] = samples.pos;
            
            /*if(DEBUG)
            {
                Serial.print("gest_p[");
                Serial.print(num_gest_pos);
                Serial.print("]: ");
                Serial.println(gest_p[num_gest_pos]);
            }*/
            
            num_gest_pos++;
        }
    }
    
    if(movVariation() == true)
    {
        if(num_gest_mov < MAX_GEST)
        {
            gest_m[num_gest_mov] = samples.mov;
            
            /*if(DEBUG)
            {
                Serial.print("gest_m[");
                Serial.print(num_gest_mov);
                Serial.print("]: ");
                Serial.println(gest_m[num_gest_mov]);
            }*/
            
            num_gest_mov++;
        }
    }
}

/*void SWNUI::gestureRecognition()
{
    int coinc_cmd1 = 0; int coinc_cmd2 = 0;
    int coinc_cmd3 = 0; int coinc_cmd4 = 0;
    int coinc_cmd11 = 0; int coinc_cmd12 = 0;
    int coinc_cmd21 = 0; int coinc_cmd22 = 0;
    int coinc_cmd31 = 0; int coinc_cmd32 = 0;
    int coinc_cmd41 = 0; int coinc_cmd42 = 0;
    int max_coinc = 0; int max_coinc1 = 0;  int max_coinc2 = 0;
    int cmd1 = 0; int cmd2 = 0; int cmd3 = 0; int cmd4 = 0;
    int num_gest, i;
    
    for(i = 0; i < MAX_GEST; i++)
    {
        gest_pm[0][i] = gest_p[i];
        gest_pm[1][i] = gest_m[i];
    }

    num_gest = max(num_gest_pos, num_gest_mov);

    for(i = 0; i <= MAX_GEST-1; i++)
    {            
        if(gest_pm[0][i] == gest_1[0][i])
        {
            coinc_cmd11++;
            if(i < MAX_GEST-1)
            {
                if(gest_pm[0][i+1] == gest_1[0][i+1])
                    coinc_cmd12++;
            }
        }
        if(gest_pm[0][i] == gest_2[0][i])
        {
            coinc_cmd21++;
            if(i < MAX_GEST-1)
            {
                if(gest_pm[0][i+1] == gest_2[0][i+1])
                    coinc_cmd22++;
            }
        }
        if(gest_pm[0][i] == gest_3[0][i])
        {
            coinc_cmd31++;
            if(i < MAX_GEST-1)
            {
                if(gest_pm[0][i+1] == gest_3[0][i+1])
                    coinc_cmd32++;
            }
        }
        if(gest_pm[0][i] == gest_4[0][i])
        {
            coinc_cmd41++;
            if(i < MAX_GEST-1)
            {
                if(gest_pm[0][i+1] == gest_4[0][i+1])
                    coinc_cmd42++;
            }
        }
        
        Serial.print("gest_pm[0][");
        Serial.print(i);
        Serial.print("]: ");
        Serial.println(gest_pm[0][i]);
        Serial.print("gest_pm[1][");
        Serial.print(i);
        Serial.print("]: ");
        Serial.println(gest_pm[1][i]);
    }
    
    coinc_cmd1 = coinc_cmd11 + coinc_cmd12;
    coinc_cmd2 = coinc_cmd21 + coinc_cmd22;
    coinc_cmd3 = coinc_cmd31 + coinc_cmd32;
    coinc_cmd4 = coinc_cmd41 + coinc_cmd42;
    
    max_coinc = max(coinc_cmd1, coinc_cmd2);
    max_coinc = max(max_coinc, coinc_cmd3);
    max_coinc = max(max_coinc, coinc_cmd4);
    
    if(max_coinc == coinc_cmd1)
        cmd1 = 1;
    else if(max_coinc == coinc_cmd2)
        cmd2 = 1;
    else if(max_coinc == coinc_cmd3)
        cmd3 = 1;
    else if(max_coinc == coinc_cmd4)
        cmd4 = 1;
    
    if(cmd1 == 1)
    {
        if(coinc_cmd1 == MAX_GEST)
            command = GEST_NEXT;
        else
            command = NUL;
    }
    else if(cmd2 == 1)
    {
        if(coinc_cmd2 == MAX_GEST)
            command = GEST_PREV;
        else
            command = NUL;
    }
    else if(cmd3 == 1)
    {
        if(coinc_cmd3 > (int)(0.8*MAX_GEST))
            command = GEST_OK;
        else
            command = NUL;
    }
    else if(cmd4 == 1)
    {
        if(coinc_cmd4 > (int)(0.8*MAX_GEST))
            command = GEST_CANCEL;
        else
                command = NUL;
    }
    
    Serial.println(" ");
    Serial.print("num_gest: ");
    Serial.println(num_gest);
    Serial.print("command: ");
    if(command == GEST_NEXT)
        Serial.println("NEXT");
    else if(command == GEST_PREV)
        Serial.println("PREV");
    else if(command == GEST_OK)
        Serial.println("OK");
    else if(command == GEST_CANCEL)
        Serial.println("CANCEL");
    else
        Serial.println("UNKNOWN");
    Serial.println(" ");
}*/

void SWNUI::gestureRecognition()
{
    int coinc_cmd1 = 0;
    int coinc_cmd2 = 0;
    int coinc_cmd3 = 0;
    int coinc_cmd4 = 0;
    int num_gest, i;
    
    for(i = 0; i < MAX_GEST; i++)
    {
        gest_pm[0][i] = gest_p[i];
        gest_pm[1][i] = gest_m[i];
    }

    num_gest = max(num_gest_pos, num_gest_mov);

    for(i = 0; i < MAX_GEST; i++)
    {            
        if(gest_pm[0][i] == gest_1[0][i])// && (gest_pm[1][i] == gest_1[1][i]))
            coinc_cmd1++;

        if(gest_pm[0][i] == gest_2[0][i])// && (gest_pm[1][i] == gest_2[1][i]))
            coinc_cmd2++;

        if(gest_pm[0][i] == gest_3[0][i])// && (gest_pm[1][i] == gest_3[1][i]))
            coinc_cmd3++;

        if(gest_pm[0][i] == gest_4[0][i])// && (gest_pm[1][i] == gest_4[1][i]))
            coinc_cmd4++;
        
        if(DEBUG)
        {
            Serial.println();
            Serial.print("gest_pm[0][");
            Serial.print(i);
            Serial.print("]: ");
            Serial.println(gest_pm[0][i]);
            Serial.print("gest_pm[1][");
            Serial.print(i);
            Serial.print("]: ");
            Serial.println(gest_pm[1][i]);
        }
    }
    
    if(coinc_cmd1 == MAX_GEST)
        command = GEST_NEXT;
    else if(coinc_cmd2 == MAX_GEST)
        command = GEST_PREV;
    else if(coinc_cmd3 == MAX_GEST)
        command = GEST_OK;
    else if(coinc_cmd4 == MAX_GEST)
        command = GEST_CANCEL;
    else
        command = NUL;
        
    if(DEBUG)
    {
        Serial.println(" ");
        Serial.print("num_gest: ");
        Serial.println(num_gest);
        Serial.print("command: ");
        if(command == GEST_NEXT)
            Serial.println("NEXT");
        else if(command == GEST_PREV)
            Serial.println("PREV");
        else if(command == GEST_OK)
            Serial.println("OK");
        else if(command == GEST_CANCEL)
            Serial.println("CANCEL");
        else
            Serial.println("UNKNOWN");
        Serial.println(" ");
    }
}

void SWNUI::refreshGestureArrays()
{
    int x, y;
    
    num_gest_pos = 0;
    num_gest_mov = 0;
    mov_old = NUL;
    pos_old = NUL;
            
    for(x = 0; x < MAX_GEST; x++)
    {
        gest_p[x] = NUL;
        gest_m[x] = NUL;
    }
    
    for(x = 0; x < GEST_TUPLE; x++)
    {
        for(y = 0; y < MAX_GEST; y++)
        {
            gest_pm[x][y] = NUL;
        }
    }
}

void SWNUI::gestureSend()
{
    ble->send(command);
}

void SWNUI::continuosSend()
{
    button_disableInterrupt();
    ble->send(samples.mx);
    ble->send(samples.my);
    ble->send(samples.mz);
    button_enableInterrupt();
}

void SWNUI::sleep()
{
    delay(1000);
    ble->flush();
    led->off();
    sleep_mode();
    led->on();
}

ADXL335_DATA SWNUI::adxl_sampling()
{
    return adxl335->sampling();
}

bool SWNUI::button_press()
{
    return button->pressed();
}

void SWNUI::changeMode_management()
{
    if(change_m == true)
    {
        button_disableInterrupt();
        change_mode();
        change_m = false;
        delay(CHANGE_MODE_TIME);
        button_enableInterrupt();
    }
}

void SWNUI::change_mode()
{
    if(mode == M_CONTINUOUS)
    {
        mode = M_GESTURE;
    }
    else
    {
        mode = M_CONTINUOUS;
        state = INIT;
    }
}

void SWNUI::led_blink(unsigned long time)
{
    led->flash(time);
}

void SWNUI::led_on()
{
    led->on();
}

void SWNUI::led_off()
{
    led->off();
}

void SWNUI::led_turn()
{
    led->turn();
}

void SWNUI::led_test(unsigned long time)
{
    led->on();
    delay(time);
    led->off();
    delay(time);
}

void SWNUI::button_test()
{
    if(button->pressed())
        led->on();
    else
        led->off();
}

void SWNUI::button_setInterrupt()
{
    EICRA |= (1<<ISC01);  // Trigger INT0 on falling edge
    EIMSK |= (1 << INT0);     // Enable INT0 interrupt

    button_enableInterrupt();
}

void SWNUI::button_enableInterrupt()
{
    //sei(); //Enable Global Interrupt
    EIMSK |= (1 << INT0);     // Enable INT0 interrupt
}

void SWNUI::button_disableInterrupt()
{
    //cli(); //Disable Global Interrupt
    EIMSK &= ~(1 << INT0);
}

/***********************************************************************************/
