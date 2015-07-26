#ifndef BLE_h
  #define BLE_h

  #include <Arduino.h>
  
  #define BLE_BAUDS 9600
  #define NUL 'N'
  #define CON 'A'
  
  class BLE
  {
      public:
          BLE();
          
          void conect();
          void flush();
          void send(char c);
          void print(String s);
          void println(String s);
          
      private:
          int _bauds;
          
          void config();
  };
  
#endif

