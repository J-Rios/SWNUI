/****************************************************************************************************************/
/* Programa: SWNUI_keyboard                                                                                     */
/* Descripcion: Programa que recibe por el puerto serie (modulo BLE) muestras relacionadas con el movimiento    */
/* del SWNUI y las interpreta para gestionar el control del teclado del ordenador (dispositivo HID-USB mediante */
/* V-USB).                                                                                                      */
/* Version: 01                                                                                                  */
/****************************************************************************************************************/

// LIBRERIAS
#include <TinyPinChange.h>
#include <SoftSerial.h>
#include "DigiKeyboard.h"

// CONSTANTES

#define P_RX 0                        // Pin de recepcion (puerto serie)
#define P_TX 1                        // Pin de transmision (puerto serie)
#define PPM 420                       // Pulsaciones Por Minuto
#define PPS PPM/60                    // Pulsaciones Por Segundo
#define TEP 1000/PPS                  // Tiempo entre pulsaciones
#define BLE_TIMEOUT 10000             // Tiempo de conexion del modulo bluetooth
#define GEST_OK '1'                   // Comando OK correspondiente a la tecla F5
#define GEST_CANCEL '2'               // Comando OK correspondiente a la tecla escape
#define GEST_NEXT '3'                 // Comando NEXT correspondiente a la flecha derecha
#define GEST_PREV '4'                 // Comando PREV correspondiente a la flecha izquierda
#define CON 'A'                       // Comando de conexion establecida
#define NUL 'N'                       // Comando nulo

// VARIABLES
char cmd;                             // Comando (gesto)

// OBJETOS
SoftSerial BLE(P_RX, P_TX);           // Puerto serie software para el control del modulo BLE

/****************************************************************************************************************/

// PROTOTIPO DE FUNCIONES
void conectar_BLE();                  // Inicializa el puerto serie y se conecta al modulo esclavo
bool datos_disponibles();             // Determina si hay datos disponibles en el puerto serie
void leer_dato();                     // Lee el dato del puerto serie
void determinar_tecla();              // Determina y simula la tecla a presionar, a partir del comando recibido
void tecla(unsigned int t);           // Realiza la pulsacion de la tecla t
void actualizar_teclado();            // Mantiene con vida el periferico USB

/****************************************************************************************************************/

// PROGRAMA PRINCIPAL
void setup()
{
    conectar_BLE();
}


void loop()
{
    if(datos_disponibles())
    {
        leer_dato();
        determinar_tecla();
    }
    actualizar_teclado();
}

/****************************************************************************************************************/

// FUNCIONES

void conectar_BLE()
{    
    BLE.begin(9600);

    BLE.print("at+conn00,15,83,00,12,51\r\n");
    DigiKeyboard.delay(BLE_TIMEOUT);
    BLE.write(CON);
}

bool datos_disponibles()
{
    bool md;
    
    if(BLE.available())
        md = true;
    else
        md = false;
        
    return md;
}

void leer_dato()
{
    cmd = BLE.read();
}

void actualizar_teclado()
{
    DigiKeyboard.update();
}

void determinar_tecla()
{
    if(cmd == GEST_NEXT)
        tecla(KEY_ARROW_RIGHT);
    else if(cmd == GEST_PREV)
        tecla(KEY_ARROW_LEFT);
    else if(cmd == GEST_OK)
        tecla(KEY_F5);
    else if(cmd == GEST_CANCEL)
        tecla(KEY_ESC);
}

void tecla(unsigned int t)
{
    DigiKeyboard.sendKeyStroke(t);
    DigiKeyboard.delay(TEP);
}
