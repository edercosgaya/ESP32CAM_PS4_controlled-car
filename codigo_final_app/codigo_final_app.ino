#include <WiFi.h>
#include <WebServer.h>

// Configuración de Wi-Fi
const char* WIFI_SSID = "WiFi Externa"; // Cambia esto por el SSID de tu red Wi-Fi
const char* WIFI_PASS = "qwertyui"; // Cambia esto por la contraseña de tu red Wi-Fi

//////////////////////// COMUNICACION ///////////
String estado = "";

///////////////////PARAMETROS////////////////
int velocidadMax = 220;

//////////////////////MOTOR DERECHO/////////////////////////////// 
int velocidadDerecha = velocidadMax; // Ciclo de trabajo 0 - 255
int canal1MotorD = 0;
int canal2MotorD = 1;

//// Puente H L298N ////
const int IN1 = 13;                 
const int IN2 = 15;         

//////////////////////MOTOR IZQUIERDO/////////////////////////////// 
int velocidadIzquierda = velocidadMax; // Ciclo de trabajo 0 - 255
int canal1MotorI = 5;
int canal2MotorI = 3;

//// Puente H L298N ////
const int IN3 = 14;                  
const int IN4 = 2;                  

// Configuración de las salidas PWM
const int frecuencia = 10000;
const int resolucion = 8;

WebServer server(80); // Crear un servidor en el puerto 80

void setup() {
    Serial.begin(115200);
    Serial.println();

    // Conectar a la red Wi-Fi
    WiFi.persistent(false);
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASS);

    Serial.print("Conectando a Wi-Fi");
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.print(".");
    }
    Serial.println("\nConectado a Wi-Fi");
    Serial.print("Dirección IP: ");
    Serial.println(WiFi.localIP());

    // Configuración de pines
    ledcSetup(canal1MotorD, frecuencia, resolucion);
    ledcAttachPin(IN1, canal1MotorD);
  
    ledcSetup(canal2MotorD, frecuencia, resolucion);
    ledcAttachPin(IN2, canal2MotorD); 
  
    ledcSetup(canal1MotorI, frecuencia, resolucion);
    ledcAttachPin(IN3, canal1MotorI);

    ledcSetup(canal2MotorI, frecuencia, resolucion);
    ledcAttachPin(IN4, canal2MotorI);

    pararMotor(canal1MotorD, canal2MotorD);
    pararMotor(canal1MotorI, canal2MotorI);

    // Configurar rutas del servidor
    server.on("/F", HTTP_GET, adelante);
    server.on("/B", HTTP_GET, atras);
    server.on("/R", HTTP_GET, derecha);
    server.on("/L", HTTP_GET, izquierda);
    server.on("/S", HTTP_GET, detener);

    server.begin(); // Iniciar el servidor
}

void loop() {
    server.handleClient(); // Manejar las solicitudes del cliente
}

void adelante() {
    giroHorario(canal1MotorD, canal2MotorD, velocidadDerecha);
    giroAntihorario(canal1MotorI, canal2MotorI, velocidadIzquierda);
    estado = "F";
    server.send(200, "text/plain", "Avanzando");
}

void atras() {
    giroAntihorario(canal1MotorD, canal2MotorD, velocidadDerecha);
    giroHorario(canal1MotorI, canal2MotorI, velocidadIzquierda);
    estado = "B";
    server.send(200, "text/plain", "Retrocediendo");
}

void derecha() {
    giroAntihorario(canal1MotorD, canal2MotorD, velocidadDerecha);
    giroAntihorario(canal1MotorI, canal2MotorI, velocidadIzquierda);
    estado = "R";
    server.send(200, "text/plain", "Girando a la derecha");
}

void izquierda() {
    giroHorario(canal1MotorD, canal2MotorD, velocidadDerecha);
    giroHorario(canal1MotorI, canal2MotorI, velocidadIzquierda);
    estado = "L";
    server.send(200, "text/plain", "Girando a la izquierda");
}

void detener() {
    pararMotor(canal1MotorD, canal2MotorD);
    pararMotor(canal1MotorI, canal2MotorI);
    estado = "S";
    server.send(200, "text/plain", "Detenido");
}

void giroHorario(int canal1, int canal2, int cv) {     
    ledcWrite(canal1, cv);
    ledcWrite(canal2, 0);
}

void giroAntihorario(int canal1, int canal2, int cv) {     
    ledcWrite(canal1, 0);
    ledcWrite(canal2, cv);
}

void pararMotor(int canal1, int canal2) {     
    ledcWrite(canal1, 0);
    ledcWrite(canal2, 0);
}
