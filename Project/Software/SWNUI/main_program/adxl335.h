#ifndef ADXL335_h
  #define ADXL335_h 

  #include <Arduino.h>

  #define SENS_A 102.3 // Tenemos incrementos de 330mV por cada G de aceleración (330mV / 1023cuentas = 102.3) valor para convertir mV a G's
  #define ZERO_G 512.0 // Una conversión ADC va de 0 a 1023, siendo 512 la mitad del rango y por lo tanto el 0
  #define SENSIBILIDAD_CLICK 1.5
  #define SAMPLES_VARIATIONS_A 0.5
  #define SAMPLES_VARIATIONS_G 0.5
  #define MIDDLE_ACCELERATION 4.5
  #define MIDDLE_POSITION 0.15
  #define POSITIVE 1
  #define NEGATIVE -1
  #define STOP 0
  #define GRAVITY 9.80665
  
  #define TSIDE 'Q' // Top Side
  #define BSIDE 'W' // Bottom Side
  #define RSIDE 'E' // Right Side
  #define LSIDE 'R' // Left Side
  #define USIDE 'T' // Up Side
  #define DSIDE 'Y' // Down Side
  #define RMOVE 'U' // Right Move
  #define LMOVE 'I' // Left Move
  #define UMOVE 'O' // Up Move
  #define DMOVE 'P' // Down Move
  #define BMOVE 'A' // Back Move
  #define FMOVE 'S' // Forward Move
  #define NMOVE 'D' // No Move
  
  #define NUL 'N'
  #define CON 'Z'
  #define CLI 'C'
  
  struct ADXL335_DATA
  {
     int x, y, z;
     float ax, ay, az;
     float gx, gy, gz;
     char mx, my, mz;
     char pos;
     char mov;
     char gesture;
  };

  class ADXL335
  {
      public:
          ADXL335(int pin_x, int pin_y, int pin_z);
          ADXL335_DATA sampling();                   // Muestrear
          int x(); int ax(); int gx(); int mx();
          int y(); int ay(); int gy(); int my();
          int z(); int az(); int gz(); int mz();
          
      private:
          ADXL335_DATA data, data_old, data_new;
          int p_x, p_y, p_z;
           
          void read_samples(); // Leer muestras
          void get_as();
          void get_gs();
          void get_moves();
          void get_axisMoves();
          void filters();
          void position_filter();
          void variations_filter(); // Filtrar muestras de ruido
          void aChanges_filter(); // Filtrar cambios de aceleracion
          void changes_filter();
          void gravityCorrection();
          void floatsTruncate();
          void dataInit();
          void dataOld();
          void dataNew();
  };
  
#endif

