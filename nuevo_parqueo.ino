const int sensorPinluzPuesto1 = 4; // señal del detector de obstaculos #1 conectar en el puerto 4
 const int sensorPinluzPuesto2  = 5;// señal del detector de obstaculos #2   conectar en el puerto 5
 int luzverde1 = 6; //   luces...                          // conectar los ...... conectar en el puerto 6
 int luzverde2 = 7; /// que ...                           // anodos de .......... conectar en el puerto 7
 int luzroja1 = 8; //   indican...                       //cada led ............. conectar en el puerto 8
 int luzroja2 =9; //   si esta estacionado en el puesto.// en estos puertos:..... conectar en el puerto 9
int libre = 2;
int ocupado = 0;
long t; //timepo que demora en llegar el eco
long d; //distancia en centimetros
  
#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>
#include <Wire.h>
#include <LCD.h>
#include <LiquidCrystal_I2C.h>

#define RST_PIN_1         44          // rst para la tarjeta rfid 1 
#define SS_PIN_1          53         // sda para la tarjeta rfid 1

#define RST_PIN_2         49           // rst para la tarjeta rfid 2 
#define SS_PIN_2          47         // sda para la tarjeta rfid 2

const int Trigger = 12;   //Pin digital 2 para el Trigger del sensor
const int Echo = 11;   //Pin digital 3 para el Echo del sensor
const int pinRFID=13; // activo y desactivo el rfid
byte ActualUID[4];
byte Usuario1[4]={0xAB, 0x80, 0xAA, 0x21};
boolean  compareArray(byte array1[],byte array2[]);

LiquidCrystal_I2C lcd (0x27, 2, 1, 0, 4, 5, 6, 7); 
MFRC522 rfid_1(SS_PIN_1, RST_PIN_1);  // Crear instancia para la rfid_1
MFRC522 rfid_2(SS_PIN_2, RST_PIN_2);  // Crear instancia para la rfid_2
Servo servo_in; //motor para la entrada
Servo servo_out; //motor para la entrada
 int tarjeta();
 void proximidad();

// TARJETA 1 ES PARA ENTRADA, TARJETA 2 ES PARA Salida

void setup() {
 
  Serial.begin(9600);   //iniciar puerto serie
  pinMode(sensorPinluzPuesto1  , INPUT); //defino pin como entrada
  pinMode(sensorPinluzPuesto2  , INPUT); //defino pin como entrada
  pinMode(luzverde1,OUTPUT); //defino pin como salida
  pinMode(luzverde2,OUTPUT); //defino pin como salida
  pinMode(luzroja1,OUTPUT); //defino pin como salida
  pinMode(luzroja2,OUTPUT); //defino pin como salida
  pinMode(pinRFID,OUTPUT); //defino pin como salida
  digitalWrite(pinRFID, HIGH); // enciendo el rfid
  
  Serial.begin(9600);    // Inicializar comunicaciones en serie con la PC
  while (!Serial);    // No haga nada si no se abre un puerto serie
  SPI.begin();      // Inicia bus SPI
  rfid_1.PCD_Init();    // inicia tarjeta rfid1
  rfid_1.PCD_DumpVersionToSerial(); // Mostrar detalles de PCD - Detalles del lector de tarjetas MFRC522

  rfid_2.PCD_Init();    // inicia tarjeta rfid1
  rfid_2.PCD_DumpVersionToSerial(); // Mostrar detalles de PCD - Detalles del lector de tarjetas MFRC522

  
  Serial.println(F("Escanee PICC para ver bloques UID, SAK, tipo y datos..."));
  servo_in.attach(3); //servo pin
  servo_in.write(LOW); //servo start position
  servo_out.attach(10); //servo pin
  servo_out.write(LOW); //servo start position

  pinMode(Trigger, OUTPUT); //pin como salida
  pinMode(Echo, INPUT);  //pin como entrada
  digitalWrite(Trigger, LOW);//Inicializamos el pin con 0

  pinMode(Trigger, OUTPUT); //pin como salida
  pinMode(Echo, INPUT);  //pin como entrada
  digitalWrite(Trigger, LOW);//Inicializamos el pin con 0
  
  lcd.setBacklightPin(3,POSITIVE);
  lcd.setBacklight(HIGH);
  lcd.begin(16, 2);
  lcd.print("Instituto Coello");
  delay(3000);
  lcd.clear();
}
 
void loop(){
  
 pantalla();
 validacion();
 luzpuesto1();
 luzpuesto2();
 proximidad();
 if(libre > 0){
 tarjeta();
 }
 else{
 // Serial.println("esta lleno el parqueo");
  tarjeta();
   
 //delay(1000);
 }
  if (ocupado >= 2)
 {
  tope();
  digitalWrite(3,LOW );
  return;
  }
}
 void tope(){
  lcd.clear();
   lcd.setCursor(0,0);
    lcd.print("PARQUEO");
    lcd.setCursor(1,1);
    lcd.print("LLENO!!!");
    
    delay(3000);
}

void pantalla(){
   lcd.clear();
   lcd.setCursor(0,0);
    lcd.print("Libres ");
    lcd.setCursor(1,1);
    lcd.print(libre);
    
    lcd.setCursor(7,0);
    lcd.print("Ocupados ");
    lcd.setCursor(8,1);
    lcd.print(ocupado);
    delay(1000);
}



void  luzpuesto1(){

 int valor = 0;
  valor = digitalRead(sensorPinluzPuesto1 );  //lectura digital de pin
 
  if (valor == LOW) {
      Serial.println("Detectado obstaculo1");
      digitalWrite(luzroja1,HIGH);
      digitalWrite(luzverde1,LOW);
  }
  else{
     digitalWrite(luzroja1,LOW);
      digitalWrite(luzverde1,HIGH);
  }
  //delay(200);
}
void  luzpuesto2(){

 int valor = 0;
  valor = digitalRead(sensorPinluzPuesto2 );  //lectura digital de pin
 
  if (valor == LOW) {
      Serial.println("Detectado obstaculo2");
      digitalWrite(luzroja2,HIGH);
      digitalWrite(luzverde2,LOW);
  }
  else{
     digitalWrite(luzroja2,LOW);
      digitalWrite(luzverde2,HIGH);
  }
  //delay(200);
}

int tarjeta()
{
   
  if ( rfid_1.PICC_IsNewCardPresent())
  {
    if ( rfid_1.PICC_ReadCardSerial())
   {
    
     Serial.print(F("Card UID:" ));
     for (byte i = 0; i < rfid_1.uid.size; i++) {
      Serial.print(rfid_1.uid.uidByte[i] < 0x10 ? " 0" : " ");
      Serial.print(rfid_1.uid.uidByte[i], HEX);
      ActualUID[i]= rfid_1.uid.uidByte[i];
     }
     Serial.print("   ");
     if (compareArray(ActualUID,Usuario1)) // que acepte el ingreso
     {
       return 1;
     }
      else
      Serial.println("Acceso denegado...");
      rfid_1.PICC_HaltA();
    }
  }
}

boolean  compareArray(byte array1[],byte array2[])
{
  if(array1[0] != array2[0])return(false);
  if(array1[1] != array2[1])return(false);
  if(array1[2] != array2[2])return(false);
  if(array1[3] != array2[3])return(false);
  return(true);
}

void salida()
{
  servo_in.write(90);
  delay(4000);
  servo_in.write(0);
  delay(1000);
  libre=libre+1;
  ocupado=ocupado-1;
  Serial.println("libre");
  Serial.println(libre);
  Serial.println("ocupado");
  Serial.println(ocupado);
}

void entrada()
{
  servo_in.write(90);
  delay(4000);
  servo_in.write(0);
  delay(1000);
  libre=libre-1;
  ocupado=ocupado+1;
  Serial.println("libre");
  Serial.println(libre);
  Serial.println("ocupado");
  Serial.println(ocupado);
}

void validacion()
{
  
 for(int i=0; i<=2; i++)
 {
  if (tarjeta()==1 && libre>0)
  {
    entrada();
    delay(1000);
    if(ocupado==2)
    {
      break;
      }
    }
  if (ocupado>0 && d<7)
  {
    salida();
    delay(2000);
    if(libre==2)
    {
      return;
      }
    return;
   }

  if (i==2)
  {
    break;
    }
}
}

  // Volcar información de depuración sobre la tarjeta; PICC_HaltA () se llama automáticamente
  
  //rfid_2.PICC_DumpToSerial(&(rfid_2.uid));


void proximidad()
{
  

  digitalWrite(Trigger, HIGH);
  delayMicroseconds(10);          //Enviamos un pulso de 10us
  digitalWrite(Trigger, LOW);
  
  t = pulseIn(Echo, HIGH); //obtenemos el ancho del pulso
  d = t/59;             //escalamos el tiempo a una distancia en cm
  
  Serial.print("Distancia: ");
  Serial.print(d);      //Enviamos serialmente el valor de la distancia
  Serial.print("cm");
  Serial.println();
  delay(100);          //Hacemos una pausa de 100ms
  
  }
