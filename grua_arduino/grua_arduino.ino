#include <AccelStepper.h>

// --- Definición de Pines ---
// Joysticks
#define JOY_X A0 // Carro
#define JOY_Y A1 // Elevación
#define JOY_Z A2 // Giro

// Driver TB6612FNG (Motores DC N20)
#define PWMA 3 // Motor A (Carro)
#define AIN1 2
#define AIN2 4

#define PWMB 5 // Motor B (Elevación)
#define BIN1 7
#define BIN2 8

// Driver DRV8825 (Motor a Pasos Nema 17)
#define STEP_PIN 9
#define DIR_PIN 10

// --- Parámetros de Control ---
#define JOY_DEADZONE 50
#define MOTOR_SPEED 200 // PWM (0-255)
#define WEB_TIMEOUT_MS 500

// --- Instancias ---
// Interfaz 1 para driver que usa STEP y DIR
AccelStepper stepperRotation(1, STEP_PIN, DIR_PIN);

// --- Variables de Estado ---
char lastWebCmd = 'S';
unsigned long lastWebCmdTime = 0;

void setup() {
  Serial.begin(9600);
  
  // Configuración de pines de entrada
  pinMode(JOY_X, INPUT);
  pinMode(JOY_Y, INPUT);
  pinMode(JOY_Z, INPUT);

  // Configuración de pines de salida para TB6612FNG
  pinMode(PWMA, OUTPUT);
  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
  
  pinMode(PWMB, OUTPUT);
  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);

  // Configuración inicial del motor a pasos
  stepperRotation.setMaxSpeed(1000);
  stepperRotation.setAcceleration(500);
}

void loop() {
  readWebCommand();
  checkWebTimeout();
  executeCombinedIntent();
  
  // Ejecutar el paso si es necesario (no bloqueante)
  stepperRotation.runSpeed();
}

void readWebCommand() {
  if (Serial.available() > 0) {
    char cmd = Serial.read();
    // Validar el comando
    if (cmd == 'F' || cmd == 'B' || cmd == 'U' || cmd == 'D' || cmd == 'L' || cmd == 'R' || cmd == 'S') {
      lastWebCmd = cmd;
      lastWebCmdTime = millis();
    }
  }
}

void checkWebTimeout() {
  // Si no se ha recibido comando en el tiempo de timeout, volver a estado 'Stop'
  if (lastWebCmd != 'S' && (millis() - lastWebCmdTime > WEB_TIMEOUT_MS)) {
    lastWebCmd = 'S';
  }
}

void executeCombinedIntent() {
  // Leer valores de Joysticks (0 a 1023, centro ~512)
  int joyX = analogRead(JOY_X) - 512;
  int joyY = analogRead(JOY_Y) - 512;
  int joyZ = analogRead(JOY_Z) - 512;
  
  // Intenciones (0: Stop, 1: Positivo, -1: Negativo)
  int intentCarro = 0;
  int intentElevacion = 0;
  int intentGiro = 0;

  // Evaluar Joysticks con Deadzone
  if (joyX > JOY_DEADZONE) intentCarro = 1;
  else if (joyX < -JOY_DEADZONE) intentCarro = -1;

  if (joyY > JOY_DEADZONE) intentElevacion = 1;
  else if (joyY < -JOY_DEADZONE) intentElevacion = -1;

  if (joyZ > JOY_DEADZONE) intentGiro = 1;
  else if (joyZ < -JOY_DEADZONE) intentGiro = -1;

  // Sumar la intención Web
  switch (lastWebCmd) {
    case 'F': intentCarro = 1; break;
    case 'B': intentCarro = -1; break;
    case 'U': intentElevacion = 1; break;
    case 'D': intentElevacion = -1; break;
    case 'L': intentGiro = -1; break;
    case 'R': intentGiro = 1; break;
    case 'S': /* No añade intención */ break;
  }

  // --- Aplicar acciones a Motores ---
  
  // Carro (Motor A)
  if (intentCarro == 1) {
    digitalWrite(AIN1, HIGH);
    digitalWrite(AIN2, LOW);
    analogWrite(PWMA, MOTOR_SPEED);
  } else if (intentCarro == -1) {
    digitalWrite(AIN1, LOW);
    digitalWrite(AIN2, HIGH);
    analogWrite(PWMA, MOTOR_SPEED);
  } else {
    digitalWrite(AIN1, LOW);
    digitalWrite(AIN2, LOW);
    analogWrite(PWMA, 0);
  }

  // Elevación (Motor B)
  if (intentElevacion == 1) {
    digitalWrite(BIN1, HIGH);
    digitalWrite(BIN2, LOW);
    analogWrite(PWMB, MOTOR_SPEED);
  } else if (intentElevacion == -1) {
    digitalWrite(BIN1, LOW);
    digitalWrite(BIN2, HIGH);
    analogWrite(PWMB, MOTOR_SPEED);
  } else {
    digitalWrite(BIN1, LOW);
    digitalWrite(BIN2, LOW);
    analogWrite(PWMB, 0);
  }

  // Giro (Motor a Pasos)
  if (intentGiro == 1) {
    stepperRotation.setSpeed(800); // Pasos por segundo hacia la derecha
  } else if (intentGiro == -1) {
    stepperRotation.setSpeed(-800); // Pasos por segundo hacia la izquierda
  } else {
    stepperRotation.setSpeed(0);
  }
}
