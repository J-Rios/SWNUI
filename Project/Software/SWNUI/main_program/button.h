#ifndef BUTTON_h
  #define BUTTON_h

  #include <Arduino.h>
  
  class BUTTON
  {
      public:
          BUTTON(int pin_button, int button_mode);
          int state();
          bool pressed();
          
      private:
          int pin, mode;
          
          void config();
  };
  
#endif

