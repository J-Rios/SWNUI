#ifndef SWNUI_h
  #define SWNUI_h

  #include <avr/sleep.h>
  #include <avr/wdt.h>
  #include "adxl335.h"
  #include "button.h"
  #include "led.h"
  #include "ble.h"

  #define DEBUG 0
  #define P_AXIS_X A0
  #define P_AXIS_Y A1
  #define P_AXIS_Z A2
  #define P_BUTTON 2
  #define P_LED 6
  #define M_CONTINUOUS 0
  #define M_GESTURE 1
  #define TIMEOUT 500
  #define F_SAMPLE 30 // Hz
  #define T_SAMPLE 1000/F_SAMPLE // ms
  #define BOUNCE_TIME 500
  #define CHANGE_MODE_TIME 1000
  #define MAX_GEST 5
  #define GEST_TUPLE 2 // Dupla de gestos
  
  #define GEST_OK '1'
  #define GEST_CANCEL '2'
  #define GEST_NEXT '3'
  #define GEST_PREV '4'
  
  // Gesture[positions][moves]
  const char gest_1[GEST_TUPLE][MAX_GEST] = {{TSIDE, RSIDE, NUL, NUL, NUL}, {NUL, NUL, NUL, NUL, NUL}}; // NEXT
  const char gest_2[GEST_TUPLE][MAX_GEST] = {{RSIDE, TSIDE, NUL, NUL, NUL}, {NUL, NUL, NUL, NUL, NUL}}; // PREV
  const char gest_3[GEST_TUPLE][MAX_GEST] = {{TSIDE, RSIDE, BSIDE, NUL, NUL}, {NUL, NUL, NUL, NUL, NUL}}; // OK
  const char gest_4[GEST_TUPLE][MAX_GEST] = {{BSIDE, RSIDE, TSIDE, NUL, NUL}, {NUL, NUL, NUL, NUL, NUL}}; // CANCEL

  class SWNUI
  {
      public:
          SWNUI();
          void init();
          void update();
          void mode_continuous();
          void mode_gesture();
          void gestures();
          void gestureRecognition();
          void change_mode();
          void gestureSend();
          void continuosSend();
          ADXL335_DATA adxl_sampling();
          void sleep();
          void led_on();
          void led_off();
          void led_turn();
          void led_test(unsigned long time);
          void led_blink(unsigned long time);
          bool button_press();
          void button_test();
          void button_enableInterrupt();
          void button_disableInterrupt();
          
      private:
          BLE* ble;
          LED* led;
          BUTTON* button;
          ADXL335* adxl335;
          ADXL335_DATA samples;
          char mov_old;
          char pos_old;
          char gest_p[MAX_GEST], gest_m[MAX_GEST];
          char gest_pm[GEST_TUPLE][MAX_GEST];
          char command;
          int mode, num_gest_pos, num_gest_mov;
          
          enum estados
          {
              INIT,
              SAMPLE,
              SEND
          } state;
          
          void timer2_setup();
          bool posVariation();
          bool movVariation();
          void refreshGestureArrays();
          void changeMode_management();
          void buttonDebounce_management();
          void button_setInterrupt();
  };
  
#endif

