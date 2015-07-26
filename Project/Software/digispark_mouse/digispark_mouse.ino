/****************************************************************************************************************/
/* Programa: SWNUI_mouse                                                                                        */
/* Descripcion: Programa que recibe por el puerto serie (modulo BLE) muestras relacionadas con el movimiento    */
/* del SWNUI y las interpreta para gestionar el control del cursor del ordenador (dispositivo HID-USB mediante  */
/* V-USB).                                                                                                      */
/* Version: 04                                                                                                  */
/****************************************************************************************************************/

// LIBRERIAS
#include <TinyPinChange.h>
#include <SoftSerial.h>
#include <DigiMouse.h>

// CONSTANTES
#define BLE_TIMEOUT 10000             // Tiempo de conexion del modulo bluetooth
#define MOUSE_VELOCITY 1              // Paso (pixeles) con el que se incrementa la velocidad del cursor
#define MAX_MOUSE_VELOCITY 10         // Velocidad maxima que se alcanza en el desplazamiento del cursor
#define TIEMPO_ENTRE_CLICKS 100       // Tiempo de espera entre clicks (en ms)
#define P_RX 0                        // Pin de recepcion (puerto serie)
#define P_TX 1                        // Pin de transmision (puerto serie)
#define CON 'A'                       // Comando de conexion establecida
#define NUL 'N'                       // Comando de movimiento nulo (no movimiento)
#define UMOVE 'O'                     // Comando de movimiento hacia arriba
#define DMOVE 'P'                     // Comando de movimiento hacia abajo
#define LMOVE 'I'                     // Comando de movimiento hacia la izquierda
#define RMOVE 'U'                     // Comando de movimiento hacia la derecha
#define CLI 'C'                       // Comando de click izquierdo

#define BMOVE 'A' // Back Move
#define FMOVE 'S' // Forward Move

#define RSIDE 'E' // Right Side
#define LSIDE 'R' // Left Side
#define USIDE 'T' // Up Side
#define DSIDE 'Y' // Down Side

  
// VARIABLES
char mx, my, mz, mov;                 // Comandos de movimiento
int c_mx, c_my;                       // Cantidad de movimiento
bool c_click;                         // Click del cursor
unsigned long t0;                     // Tiempo cero

// OBJETOS
SoftSerial BLE(P_RX, P_TX);           // Puerto serie software para el control del modulo BLE

/****************************************************************************************************************/

// PROTOTIPO DE FUNCIONES
void inicializar_datos();             // Inicializa las variables (de recepcion y movimiento)
void conectar_BLE();                  // Inicializa el puerto serie y se conecta al modulo esclavo
void inicializar_raton();             // Inicializa el control del cursor (HID-USB)
bool muestras_disponibles();          // Determina si hay muestras disponibles en el puerto serie
void leer_muestras();                 // Lee las muestras del puerto serie
void determinar_movimiento();         // Determina el movimiento a realizar a partir de las muestras adquiridas
void determinar_click();              // Determina si ha de producirse un click
void actualizar_cursor();             // Actualiza la posicion del cursor y mantiene con vida el periferico USB
void click_izquierdo();               // Realiza un click izquierdo (presiona y suelta)
void tiempo_cero();                   // Establece el momento inicial
unsigned long tiempo_transcurrido();  // Determina el tiempo transcurrido desde el momento inicial

/****************************************************************************************************************/

// PROGRAMA PRINCIPAL
void setup()
{
    inicializar_datos();
    inicializar_raton();
    conectar_BLE();
}

void loop()
{
    if(muestras_disponibles())
    {
        leer_muestras();
        determinar_movimiento();
        determinar_click();
    }
    actualizar_cursor();
}

/****************************************************************************************************************/

// FUNCIONES
void inicializar_datos()
{
    mov = NUL;
    mx = NUL;
    my = NUL;
    mz = NUL;
    c_mx = 0;
    c_my = 0;
    c_click = false;
    t0 = 0;
}

void conectar_BLE()
{
    BLE.begin(9600);

    BLE.print("at+conn00,15,83,00,12,51\r\n");
    DigiMouse.delay(BLE_TIMEOUT);
    BLE.write(CON);
}

void inicializar_raton()
{
    DigiMouse.begin();
}

bool muestras_disponibles()
{
    bool md;
    
    if(BLE.available() >= 3)
        md = true;
    else
        md = false;
        
    return md;
}

void leer_muestras()
{
    mx = BLE.read();
    my = BLE.read();
    mz = BLE.read();
}

void determinar_movimiento()
{
    if(mx == RSIDE)
    {
        if(c_mx < MAX_MOUSE_VELOCITY)
            c_mx = c_mx + MOUSE_VELOCITY;
    }
    else if(mx == LSIDE)
    {
        if(c_mx > -MAX_MOUSE_VELOCITY)
            c_mx = c_mx - MOUSE_VELOCITY;
    }
    else
        c_mx = 0;
    
    if(my == DSIDE)
    {
        if(c_my < MAX_MOUSE_VELOCITY)
            c_my = c_my + MOUSE_VELOCITY;
    }
    else if(my == USIDE)
    {
        if(c_my > -MAX_MOUSE_VELOCITY)
            c_my = c_my - MOUSE_VELOCITY;
    }
    else
        c_my = 0;
}

void determinar_click()
{
    if((mz == CLI) && (tiempo_transcurrido() >= TIEMPO_ENTRE_CLICKS))
    {
        c_click = true;
        tiempo_cero();
    }
    else
        c_click = false;
}

void actualizar_cursor()
{
    if(c_click == true)
    {
        click_izquierdo();
        c_click = false;
        tiempo_cero();
    }
    
    DigiMouse.move(c_mx, c_my, c_click);
    
    DigiMouse.update();
}

void click_izquierdo()
{
    DigiMouse.setButtons(1<<0); // Click
    DigiMouse.delay(10);
    DigiMouse.setButtons(0); // Unclick
}

void tiempo_cero()
{
    t0 = millis();
}

unsigned long tiempo_transcurrido()
{
    return (millis() - t0);
}

/****************************************************************************************************************/
