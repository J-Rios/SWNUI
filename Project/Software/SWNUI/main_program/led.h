#ifndef LED_h
  #define LED_h

  #include <Arduino.h>
  
  #define MAX_TIME 5000

  class LED
  {
      public:
          LED(int pin_led, int stat_led);
          void configure();
          void turn();
          void on();
          void off();
          int state();
          void flash(int time);
          void test(unsigned long time);
          
      private:
          int pin, stat;
          unsigned long t0, t1;
          bool pv; 
  };
  
#endif

