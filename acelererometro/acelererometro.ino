#include <SPI.h>
#include <Wire.h>
#include <TimerThree.h>

////////////////////////definicion del giro del motor///////////////////////////////////
#define MPU 0x68  // Direccion I2C del MPU-6050
#define relay1 7  // pines analogicos utilizados para los relevadores
#define relay2 6  // pines analogicos utilizados para los relevadores

////////////////////////definicion del giro del motor///////////////////////////////////
//definicion del giro del motor
#define izquierda LOW //definicion del giro del motor
#define derecha HIGH

////////////////////////////// variables globales del programa /////////////////////////////////
volatile bool direccion; //bandera que decide el giro del motor
double AcX,AcY,AcZ;   //varibles que sirven para leer el acelerometro
double Pitch, Roll;      //varibles que te ayudan a medir el pitch y roll del acelerometro

void setup() {
  Serial.begin(9600);
  pinMode(relay1, OUTPUT);            //configuramos el pin en modo de salida
  pinMode(relay2, OUTPUT);            //configuramos el pin en modo de salida
  Timer3.initialize(25);              //inicializamos el timer para un tieempo de muestreo
  Timer3.attachInterrupt(ISR_Blink);  //Activa la interrupcion y la asocia a ISR_Blink

  init_MPU(); // Inicializamos el MPU6050
  interrupts();                 // Autoriza las interrupciones
}

void loop() {
  FunctionsMPU(); // Adquirimos los ejex AcX, AcY, AcZ. 
  Roll = FunctionsPitchRoll(AcX, AcZ, AcY);   //Calculo del angulo del Roll
  Pitch = FunctionsPitchRoll(AcY, AcX, AcZ);  //Calculo del angulo del Pitch
  digitalWrite(relay1, direccion);  // Asignamos el valor del status a la salida
  digitalWrite(relay2, direccion);  // Asignamos el valor del status a la salida
  
//  Serial.print("Pitch:   "); Serial.print(Pitch);
//  Serial.print("\n");
//  Serial.print("Roll:   "); Serial.print(Roll);
//  Serial.print("\n");
//  Serial.print("Direccion:   "); Serial.print(direccion);
//  Serial.print("\n");
  
  //delay(2500);
}

// ================================================================
// ===                    init mpu                             ===
// ================================================================
void init_MPU(){
  Wire.begin();
  Wire.beginTransmission(MPU);
  Wire.write(0x6B);  // PWR_MGMT_1 register
  Wire.write(0);     // Seteamos a cero (wakes up - Despertamos el MPU-6050)
  Wire.endTransmission(true);
  delay(1000); /** revisar si se puede quitar, es una inicializacion pero tengo miedo de que el delay 
                   le de en su madre al codigo bloqueante del brushles **/
}

// ================================================================
// ===                FunctionsPitchRoll                        ===
// ================================================================
//Funcion para el calculo del angulo Pitch y Roll
double FunctionsPitchRoll(double A, double B, double C){
  double DatoA, DatoB, Value;
  DatoA = A;
  DatoB = (B*B) + (C*C);
  DatoB = sqrt(DatoB);
  
  Value = atan2(DatoA, DatoB);
  Value = Value * 180/3.14;
  
  return Value;
}

// ================================================================
// ===               FunctionsAngleToGyroscopio                 ===
// ================================================================
//Funcion para el calculo de los angulos del giroscopio
double FunctionsAngle(double A){
  double DatoA, Value;
  double G_R = 131.0;
  DatoA = A;
  Value = DatoA / G_R;
  return Value;
}


// ================================================================
// ===                     FunctionsMPU                         ===
// ================================================================
void FunctionsMPU(){
  //Para los valores del acelerometro
  Wire.beginTransmission(MPU);
  Wire.write(0x3B);  // Empezamos con el registro 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU,6,true);  // requerimos un total de 6 registers
  AcX=Wire.read()<<8|Wire.read();  // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)     
  AcY=Wire.read()<<8|Wire.read();  // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
  AcZ=Wire.read()<<8|Wire.read();  // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)

  //Leer los valores del Giroscopio
//   Wire.beginTransmission(MPU);
//   Wire.write(0x43);
//   Wire.endTransmission(false);
//   Wire.requestFrom(MPU,4,true); //A diferencia del Acelerometro, solo se piden 4 registros
//   GyX=Wire.read()<<8|Wire.read();
//   GyY=Wire.read()<<8|Wire.read();
  
}

// ================================================================
// ===                    LED blink                             ===
// ================================================================
void ISR_Blink()
   {  

      if (Pitch < 0 )
        direccion = izquierda ;
      if (Pitch > 0 )
        direccion = derecha ;
   }
