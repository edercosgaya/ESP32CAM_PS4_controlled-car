#include "BluetoothSerial.h"

// Activar sin ajuste de linea

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run 'make menuconfig' to and enable it
#endif

BluetoothSerial SerialBT;

//////////////////////// COMUNICACION ///////////
String estado = "";

///////////////////PARAMETROS////////////////
int velocidadMax = 220;

//////////////////////MOTOR DERECHO///////////////////////////////
int velocidadDerecha = velocidadMax; // Ciclo de trabajo 0 - 255
int canal1MotorD = 0;
int canal2MotorD = 1;

//// Puente H L298N ////
const int    IN1 = 14;                 
const int    IN2 = 2;         

//////////////////////MOTOR IZQUIERDO///////////////////////////////
int velocidadIzquierda = velocidadMax; // Ciclo de trabajo 0 - 255
int canal1MotorI = 5;
int canal2MotorI = 3;

//// Puente H L298N ////
const int    IN3 = 12;                  
const int    IN4 = 15;                  

// Configuracion de las salidas pwm
const int frecuencia = 10000;
const int resolucion= 8;


void setup()
{
  SerialBT.begin("Robot_Diferencial_Controlado");

  ledcSetup(canal1MotorD, frecuencia, resolucion);
  ledcAttachPin(IN1,canal1MotorD);
  
  ledcSetup(canal2MotorD, frecuencia, resolucion);
  ledcAttachPin(IN2,canal2MotorD); 
  
  ledcSetup(canal1MotorI, frecuencia, resolucion);
  ledcAttachPin(IN3,canal1MotorI);

  ledcSetup(canal2MotorI, frecuencia, resolucion);
  ledcAttachPin(IN4,canal2MotorI);

  pararMotor(canal1MotorD,canal2MotorD);
  pararMotor(canal1MotorI,canal2MotorI);
     
}
void loop() 
{
  
  if(SerialBT.available()) serialEvent();

 
  if (estado.length() > 0) {

    SerialBT.println(estado);
    
    if(estado == "F") // Adelante
    {
      adelante();
    }

    if(estado == "B") // Atras
    {
      atras();
    }
    
    if(estado == "R") // Derecha
    {
      derecha();
    }
    
    if(estado == "L") // Izquierda
    {
      izquierda();
    }
    if(estado == "S") // Parar
    {
      detener();
    }
    
    estado ="";
    }

  delay(100);
}
/////////////// RECEPCION DE DATOS /////////////////////
void serialEvent() {
  while (SerialBT.available()) {
    char inChar = (char)SerialBT.read();
    estado += inChar;
  }
}

void adelante()
{
  giroHorario(canal1MotorD,canal2MotorD,velocidadDerecha);
  giroAntihorario(canal1MotorI,canal2MotorI,velocidadIzquierda);
}

void atras()
{
  giroAntihorario(canal1MotorD,canal2MotorD,velocidadDerecha);
  giroHorario(canal1MotorI,canal2MotorI,velocidadIzquierda);
}

void derecha()
{
  giroAntihorario(canal1MotorD,canal2MotorD,velocidadDerecha);
  giroAntihorario(canal1MotorI,canal2MotorI,velocidadIzquierda);
}

void izquierda()
{
  giroHorario(canal1MotorD,canal2MotorD,velocidadDerecha);
  giroHorario(canal1MotorI,canal2MotorI,velocidadIzquierda);
}

void detener()
{
  pararMotor(canal1MotorD,canal2MotorD);
  pararMotor(canal1MotorI,canal2MotorI);
}


void giroHorario(int canal1, int canal2, int cv)
{     
  ledcWrite(canal1,cv);
  ledcWrite(canal2,0);
}

void giroAntihorario(int canal1, int canal2, int cv)
{     
  ledcWrite(canal1,0);
  ledcWrite(canal2,cv);
}

void pararMotor(int canal1, int canal2)
{     
  ledcWrite(canal1,0);
  ledcWrite(canal2,0);
}