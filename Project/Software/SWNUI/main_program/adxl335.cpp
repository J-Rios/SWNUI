#include "adxl335.h"

ADXL335::ADXL335(int pin_x, int pin_y, int pin_z)
{
    p_x = pin_x;
    p_y = pin_y;
    p_z = pin_z;
    
    dataInit();
}

ADXL335_DATA ADXL335::sampling()
{
    dataInit();
    read_samples();
    get_gs();
    get_as();
    floatsTruncate();
    filters();
    get_axisMoves();
    get_moves();
    dataOld();
    
    return data;
}

void ADXL335::dataInit()
{
    data.x = 0;
    data.y = 0;
    data.z = 0;
    data.ax = 0;
    data.ay = 0;
    data.az = 0;
    data.gx = 0;
    data.gy = 0;
    data.gz = 0;
    data.mx = NUL;
    data.my = NUL;
    data.mz = NUL;
    
    dataOld();
    dataNew();
}

void ADXL335::read_samples()
{
    int m;
    
    data_new.x = 0;
    data_new.y = 0;
    data_new.z = 0;
    
    for(m = 0; m < 3; m++)
    {
        data_new.x = data_new.x + analogRead(p_x); // Leemos el valor de la tensión en el pin x
        data_new.y = data_new.y + analogRead(p_y); // Leemos el valor de la tensión en el pin y
        data_new.z = data_new.z + analogRead(p_z); // Leemos el valor de la tensión en el pin Z
    }
    
    data_new.x = data_new.x/3;
    data_new.y = data_new.y/3;
    data_new.z = data_new.z/3;
}

void ADXL335::get_gs()
{
    data_new.gx = (data_new.x-ZERO_G)/SENS_A;
    data_new.gy = (data_new.y-ZERO_G)/SENS_A;
    data_new.gz = (data_new.z-ZERO_G)/SENS_A;
}

void ADXL335::get_as()
{
    data_new.ax = data_new.gx*GRAVITY;
    data_new.ay = data_new.gy*GRAVITY;
    data_new.az = data_new.gz*GRAVITY;
}

void ADXL335::floatsTruncate()
{
    data_new.ax = (float)(((int)(data_new.ax*1000)) / 1000.0);
    data_new.ay = (float)(((int)(data_new.ay*1000)) / 1000.0);
    data_new.az = (float)(((int)(data_new.az*1000)) / 1000.0);
    data_new.gx = (float)(((int)(data_new.gx*1000)) / 1000.0);
    data_new.gy = (float)(((int)(data_new.gy*1000)) / 1000.0);
    data_new.gz = (float)(((int)(data_new.gz*1000)) / 1000.0);
}

void ADXL335::filters()
{
    variations_filter();
    position_filter();
    changes_filter();
}

void ADXL335::variations_filter() // Si la nueva muestra no ha variado SAMPLES_VARIATIONS respecto a la anterior, no se detecta
{
    // As
    if((data_new.ax >= data_old.ax-SAMPLES_VARIATIONS_A) && (data_new.ax <= data_old.ax+SAMPLES_VARIATIONS_A))
        data_new.ax = data_old.ax;
    if((data_new.ay >= data_old.ay-SAMPLES_VARIATIONS_A) && (data_new.ay <= data_old.ay+SAMPLES_VARIATIONS_A))
        data_new.ay = data_old.ay;
    if((data_new.az >= data_old.az-SAMPLES_VARIATIONS_A) && (data_new.az <= data_old.az+SAMPLES_VARIATIONS_A))
        data_new.az = data_old.az;
        
    // Gs
    if((data_new.gx >= data_old.gx-SAMPLES_VARIATIONS_G) && (data_new.gx <= data_old.gx+SAMPLES_VARIATIONS_G))
        data_new.gx = data_old.gx;
    if((data_new.gy >= data_old.gy-SAMPLES_VARIATIONS_G) && (data_new.gy <= data_old.gy+SAMPLES_VARIATIONS_G))
        data_new.gy = data_old.gy;
    if((data_new.gz >= data_old.gz-SAMPLES_VARIATIONS_G) && (data_new.gz <= data_old.gz+SAMPLES_VARIATIONS_G))
        data_new.gz = data_old.gz;
}

void ADXL335::position_filter() // Determinamos la posicion de la mano segun las muestras
{
    // Gs
    if(data_new.gx > MIDDLE_POSITION)
        data_new.gx = POSITIVE;
    else if(data_new.gx < -MIDDLE_POSITION)
        data_new.gx = NEGATIVE;
    else
        data_new.gx = STOP;
    
    if(data_new.gy > MIDDLE_POSITION)
        data_new.gy = POSITIVE;
    else if(data_new.gy < -MIDDLE_POSITION)
        data_new.gy = NEGATIVE;
    else
        data_new.gy = STOP;
    
    if(data_new.gz > MIDDLE_POSITION)
        data_new.gz = POSITIVE;
    else if(data_new.gz < -MIDDLE_POSITION)
        data_new.gz = NEGATIVE;
    else
        data_new.gz = STOP;
    
    gravityCorrection();
    
    // As
    if(data_new.ax > MIDDLE_ACCELERATION)
        data_new.ax = POSITIVE;
    else if(data_new.ax < -MIDDLE_ACCELERATION)
        data_new.ax = NEGATIVE;
    else
        data_new.ax = STOP;
    
    if(data_new.ay > MIDDLE_ACCELERATION)
        data_new.ay = POSITIVE;
    else if(data_new.ay < -MIDDLE_ACCELERATION)
        data_new.ay = NEGATIVE;
    else
        data_new.ay = STOP;
    
    if(data_new.az > MIDDLE_ACCELERATION)
        data_new.az = POSITIVE;
    else if(data_new.az < -MIDDLE_ACCELERATION)
        data_new.az = NEGATIVE;
    else
        data_new.az = STOP;
}

void ADXL335::gravityCorrection()
{  
    if((data_new.gx == STOP) && (data_new.gy == STOP) && (data_new.gz == NEGATIVE)) // FACEUP
        data_new.az = data_new.az + GRAVITY;
        
    if((data_new.gx == STOP) && (data_new.gy == STOP) && (data_new.gz == POSITIVE)) // FACEDOWN
        data_new.az = data_new.az - GRAVITY;
        
    if((data_new.gx == STOP) && (data_new.gy == NEGATIVE) && (data_new.gz == STOP)) // RSIDE
        data_new.ay = data_new.ay + GRAVITY;
        
    if((data_new.gx == STOP) && (data_new.gy == POSITIVE) && (data_new.gz == STOP)) // LSIDE
        data_new.ay = data_new.ay - GRAVITY;
        
    if((data_new.gx == POSITIVE) && (data_new.gy == STOP) && (data_new.gz == STOP)) // USIDE
        data_new.ax = data_new.ax - GRAVITY;
        
    if((data_new.gx == NEGATIVE) && (data_new.gy == STOP) && (data_new.gz == STOP)) // DSIDE
        data_new.ax = data_new.ax + GRAVITY;
}

void ADXL335::changes_filter() // Si la muestra es distinta a la anterior, se detecta
{
    // As
    if(data_new.ax != data_old.ax)
        data.ax = data_new.ax;
    if(data_new.ay != data_old.ay)
        data.ay = data_new.ay;
    if(data_new.az != data_old.az)
        data.az = data_new.az;
    
    // Gs
    if(data_new.gx != data_old.gx)
        data.gx = data_new.gx;
    if(data_new.gy != data_old.gy)
        data.gy = data_new.gy;
    if(data_new.gz != data_old.gz)
        data.gz = data_new.gz;
}

void ADXL335::get_moves()
{    
    // STATIC POSITION
    if((data.gx == STOP) && (data.gy == STOP) && (data.gz == NEGATIVE))
        data.pos = TSIDE;
    else if(((data.gx == STOP) && (data.gy == STOP) && (data.gz == POSITIVE)) || ((data.gx == NEGATIVE) && (data.gy == STOP) && (data.gz == POSITIVE)) || 
    ((data.gx == POSITIVE) && (data.gy == STOP) && (data.gz == POSITIVE)))
        data.pos = BSIDE;
    else if(((data.gx == STOP) && (data.gy == POSITIVE) && (data.gz == STOP)) || ((data.gx == STOP) && (data.gy == POSITIVE) && (data.gz == POSITIVE)) ||
    ((data.gx == NEGATIVE) && (data.gy == POSITIVE) && (data.gz == STOP)))
        data.pos = RSIDE;
    else if(((data.gx == STOP) && (data.gy == NEGATIVE) && (data.gz == STOP)) || ((data.gx == STOP) && (data.gy == NEGATIVE) && (data.gz == POSITIVE)) || 
    ((data.gx == NEGATIVE) && (data.gy == NEGATIVE) && (data.gz == STOP)))
        data.pos = LSIDE;
    else if(((data.gx == NEGATIVE) && (data.gy == STOP) && (data.gz == STOP)) || ((data.gx == NEGATIVE) && (data.gy == POSITIVE) && (data.gz == NEGATIVE)))
        data.pos = USIDE;
    else if(((data.gx == POSITIVE) && (data.gy == STOP) && (data.gz == STOP)) || ((data.gx == POSITIVE) && (data.gy == POSITIVE) && (data.gz == STOP)))
        data.pos = DSIDE;
    else
        data.pos = TSIDE;
    
    // MOVE
    if(((data.ax == NEGATIVE) && (data.pos == TSIDE)) || ((data.ax == NEGATIVE) && (data.pos == BSIDE)))
        data.mov = FMOVE;
    else if(((data.ax == POSITIVE) && (data.pos == TSIDE)) || ((data.ax == POSITIVE) && (data.pos == BSIDE)))
        data.mov = BMOVE;
    else if(((data.ay == NEGATIVE) && (data.pos == TSIDE)) || ((data.ay == POSITIVE) && (data.pos == BSIDE)))
        data.mov = RMOVE;
    else if(((data.ay == POSITIVE) && (data.pos == TSIDE)) || ((data.ay == NEGATIVE) && (data.pos == BSIDE)))
        data.mov = LMOVE;
    else if(((data.az == POSITIVE) && (data.pos == TSIDE)) || ((data.az == NEGATIVE) && (data.pos == BSIDE)))
        data.mov = UMOVE;
    else if(((data.az == NEGATIVE) && (data.pos == TSIDE)) || ((data.az == POSITIVE) && (data.pos == BSIDE)))
        data.mov = DMOVE;
    else
        data.mov = NMOVE;
}

void ADXL335::get_axisMoves()
{
    // EJE X
    if(data.gy == POSITIVE)
        data.mx = RSIDE;
    else if(data.gy == NEGATIVE)
        data.mx = LSIDE;
    else
        data.mx = NUL;

    // EJE Y
    if(data.gx == POSITIVE)
        data.my = USIDE;
    else if(data.gx == NEGATIVE)
        data.my = DSIDE;
    else
        data.my = NUL;
        
    if(data.az == POSITIVE)
        data.mz = CLI;
    else
        data.mz = NUL;
}

void ADXL335::dataOld()
{
    data_old.x = data.x;
    data_old.y = data.y;
    data_old.z = data.z;
    data_old.ax = data.ax;
    data_old.ay = data.ay;
    data_old.az = data.az;
    data_old.gx = data.gx;
    data_old.gy = data.gy;
    data_old.gz = data.gz;
    data_old.mx = data.mx;
    data_old.my = data.my;
    data_old.mz = data.mz;
}

int ADXL335::x()
{
    return data.x;
}

int ADXL335::y()
{
    return data.y;
}

int ADXL335::z()
{
    return data.z;
}

int ADXL335::ax()
{
    return data.ax;
}

int ADXL335::ay()
{
    return data.ay;
}

int ADXL335::az()
{
    return data.az;
}

int ADXL335::gx()
{
    return data.gx;
}

int ADXL335::gy()
{
    return data.gy;
}

int ADXL335::gz()
{
    return data.gz;
}

int ADXL335::mx()
{
    return data.mx;
}

int ADXL335::my()
{
    return data.my;
}

int ADXL335::mz()
{
    return data.mz;
}

void ADXL335::dataNew()
{
    data_new.x = data_old.x;
    data_new.y = data_old.y;
    data_new.z = data_old.z;
    data_new.ax = data_old.ax;
    data_new.ay = data_old.ay;
    data_new.az = data_old.az;
    data_new.gx = data_old.gx;
    data_new.gy = data_old.gy;
    data_new.gz = data_old.gz;
    data_new.mx = data_old.mx;
    data_new.my = data_old.my;
    data_new.mz = data_old.mz;
}
