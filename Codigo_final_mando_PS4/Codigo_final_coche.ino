#include <PS4Controller.h>

// MOTORES
#define MI1 14      // Dirección motor izquierdo (pin 14)
#define MI2 2       // Dirección motor izquierdo (pin 2)
#define MIPWM 13    // PWM motor izquierdo (pin 13)
#define MD1 12      // Dirección motor derecho (pin 12)
#define MD2 15      // Dirección motor derecho (pin 15)
#define MDPWM 4     // PWM motor derecho (pin 4)

#define freq 5000 
#define MD1_PWM 2   // Canal PWM motor derecho
#define MI1_PWM 4   // Canal PWM motor izquierdo

int Rpwm = 0, Lpwm = 0;
int speed_rc = 255;
int minspeed_rc = 30;

void setup() {
    // Configuración de PWM
    ledcSetup(MD1_PWM, freq, 8); // Config de PWM MD1
    ledcSetup(MI1_PWM, freq, 8); // Config de PWM MI1
    
    // Configuración de pines
    pinMode(MD1, OUTPUT);
    pinMode(MD2, OUTPUT);
    pinMode(MI1, OUTPUT);
    pinMode(MI2, OUTPUT);
    
    // Asignar PWM a los pines
    ledcAttachPin(MDPWM, MD1_PWM);
    ledcAttachPin(MIPWM, MI1_PWM);
    
    mstop(1);
    PS4.attachOnConnect(onConnect);
    PS4.begin();
    mstop(80);
}

void loop() {
    // Controles de dirección con el joystick izquierdo (avance/retroceso progresivo)
    if (PS4.LStickY() >= 5) {  // Joystick arriba (avanzar)
        Lpwm = map(PS4.LStickY(), 6, 127, minspeed_rc, speed_rc);
        Rpwm = Lpwm;  // Ambos motores deben avanzar al mismo ritmo
        forward(Rpwm, Lpwm, 2);
    }
    if (PS4.LStickY() <= -5) {  // Joystick abajo (retroceder)
        Lpwm = map(PS4.LStickY(), -6, -128, minspeed_rc, speed_rc);
        Rpwm = Lpwm;  // Ambos motores deben retroceder al mismo ritmo
        backward(Rpwm, Lpwm, 2);
    }

    // Controles de dirección con el joystick derecho (girar)
    if (PS4.RStickX() >= 5) {  // Joystick derecho hacia la derecha (girar)
        Lpwm = map(PS4.RStickX(), 6, 127, minspeed_rc, speed_rc);
        Rpwm = -Lpwm;  // Un lado avanza y el otro retrocede
        right(Rpwm, Lpwm, 2);
    }
    if (PS4.RStickX() <= -5) {  // Joystick derecho hacia la izquierda (girar)
        Lpwm = map(PS4.RStickX(), -6, -128, minspeed_rc, speed_rc);
        Rpwm = -Lpwm;  // Un lado avanza y el otro retrocede
        left(Rpwm, Lpwm, 2);
    }

    // Controles de botones sin PWM (máxima velocidad)
    if (PS4.Up()) {
        forward(255, 255, 5);  // Avanzar sin PWM
    }
    if (PS4.Down()) {
        backward(255, 255, 5);  // Retroceder sin PWM
    }
    if (PS4.Right()) {
        right(255, 255, 5);  // Girar sin PWM hacia la derecha
    }
    if (PS4.Left()) {
        left(255, 255, 5);  // Girar sin PWM hacia la izquierda
    }

    // Detener motores cuando los joysticks estén en el centro
    if ((PS4.RStickX() > -10 && PS4.RStickX() < 10) && (PS4.LStickY() > -10 && PS4.LStickY() < 10)) {
        mstop(1);
    }
}

void onConnect() {
    PS4.setLed(70, 0, 70);
    PS4.sendToController();
}

// Función para avanzar
void forward(byte PWM1, byte PWM2, int T) {
    ledcWrite(MD1_PWM, PWM1);
    ledcWrite(MI1_PWM, PWM2);
    digitalWrite(MD1, LOW);  // Ambos motores derecho avanzan
    digitalWrite(MD2, HIGH);
    digitalWrite(MI1, LOW);  // Ambos motores izquierdo avanzan
    digitalWrite(MI2, HIGH);
    delay(T);
}

// Función para retroceder
void backward(byte PWM1, byte PWM2, int T) {
    ledcWrite(MD1_PWM, PWM1);
    ledcWrite(MI1_PWM, PWM2);
    digitalWrite(MD1, HIGH);   // Motores derecho retroceden (correcto)
    digitalWrite(MD2, LOW);    // Cambiado: MD2 debe ser LOW para retroceder
    digitalWrite(MI1, HIGH);   // Motores izquierdo retroceden (correcto)
    digitalWrite(MI2, LOW);    // Cambiado: MI2 debe ser LOW para retroceder
    delay(T);
}

// Función para girar a la derecha
void right(byte PWM1, byte PWM2, int T) {
    ledcWrite(MD1_PWM, PWM1);
    ledcWrite(MI1_PWM, PWM2);
    digitalWrite(MD1, HIGH);   // Motores derecho avanzan
    digitalWrite(MD2, LOW);
    digitalWrite(MI1, LOW);  // Motores izquierdo retroceden
    digitalWrite(MI2, HIGH);
    delay(T);
}

// Función para girar a la izquierda
void left(byte PWM1, byte PWM2, int T) {
    ledcWrite(MD1_PWM, PWM1);
    ledcWrite(MI1_PWM, PWM2);
    digitalWrite(MD1, LOW);  // Motores derecho retroceden
    digitalWrite(MD2, HIGH);
    digitalWrite(MI1, HIGH);   // Motores izquierdo avanzan
    digitalWrite(MI2, LOW);
    delay(T);
}

// Función para detener los motores
void mstop(int T) {
    ledcWrite(MD1_PWM, 0);
    ledcWrite(MI1_PWM, 0);
    digitalWrite(MD1, LOW);
    digitalWrite(MD2, LOW);
    digitalWrite(MI1, LOW);
    digitalWrite(MI2, LOW);
    delay(T);
}
