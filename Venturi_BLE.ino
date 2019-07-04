//This example code is in the Public Domain (or CC0 licensed, at your option.)
//By Evandro Copercini - 2018
//
//This example creates a bridge between Serial and Classical Bluetooth (SPP)
//and also demonstrate that SerialBT have the same functionalities of a normal Serial

#include "BluetoothSerial.h"

//#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
//#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
//#endif

BluetoothSerial SerialBT;

int entrada;

//PWM
const int ledPin = 13;  // 16 corresponds to GPIO16
const int freq = 10000;
const int ledChannel = 0;
const int resolution = 12;
int duty;

//FRECUENCIA
const byte        interruptPin = 23;              // Assign the interrupt pin
volatile uint64_t StartValue;                     // First interrupt value
volatile uint64_t PeriodCount;                    // period in counts of 0.000001 of a second
float             Freg;                           // frequency     
char              str[21];                        // for printing uint64_t values
 
hw_timer_t * timer = NULL;                        // pointer to a variable of type hw_timer_t 
portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;  // synchs between maon cose and interrupt?


void IRAM_ATTR handleInterrupt() 
{
  portENTER_CRITICAL_ISR(&mux);
      uint64_t TempVal= timerRead(timer);         // value of timer at interrupt
      PeriodCount= TempVal - StartValue;          // period count between rising edges in 0.000001 of a second
      StartValue = TempVal;                       // puts latest reading as start for next calculation
  portEXIT_CRITICAL_ISR(&mux);
}

char * uintToStr( const uint64_t num, char *str )
{
  uint8_t i = 0;
  uint64_t n = num;
  do
    i++;
  while ( n /= 10 );
  
  str[i] = '\0';
  n = num;
 
  do
    str[--i] = ( n % 10 ) + '0';
  while ( n /= 10 );

  return str;
}

//SETUP

void setup() {
  Serial.begin(115200);

  //PWM
  ledcSetup(ledChannel, freq, resolution);
  ledcAttachPin(ledPin, ledChannel);
  

  //PARA FRECUENCIA
  pinMode(interruptPin, INPUT_PULLUP);                                            // sets pin high
  attachInterrupt(digitalPinToInterrupt(interruptPin), handleInterrupt, RISING); // attaches pin to interrupt on Falling Edge
  timer = timerBegin(0, 80, true);

  timerStart(timer); 

  //SETUP NORMAL  
  SerialBT.begin("Venturi5"); //Bluetooth device name
  Serial.println("The device started, now you can pair it with bluetooth!");

  
}

void loop() {
  
  //FRECUENCIA
  portENTER_CRITICAL(&mux);
  Freg =1000000.00/PeriodCount;                       // PeriodCount in 0.000001 of a second
  portEXIT_CRITICAL(&mux);

  //SENSOR
  int fregInt = Freg;
  float litros = (0.0004028)*Freg + 0.01664;
  litros = litros * 60.0;

  //BLUETOOTH
  if (SerialBT.available()) {
    entrada = SerialBT.read();
    Serial.print("Entrando: "); Serial.println(entrada);

    if(entrada == 49){
      //SerialBT.println("UNO");
      Serial.println("1 SERIAL");
      SerialBT.println(litros);
      Serial.println(fregInt);
      duty = 70;
      for(int dutyCycle = 0; dutyCycle <= duty; dutyCycle++){   
        // changing the LED brightness with PWM
        ledcWrite(ledChannel, dutyCycle);
        
      }
    }

    if(entrada == 50){
      //SerialBT.println("2");
      Serial.println("2 SERIAL");
      SerialBT.println(litros);
      Serial.println(fregInt);
      duty = 50;
      for(int dutyCycle = 0; dutyCycle <= duty; dutyCycle++){   
        // changing the LED brightness with PWM
        ledcWrite(ledChannel, dutyCycle);
      }
    }

    if(entrada == 51){
      //SerialBT.println("3");
      Serial.println("3 SERIAL");
      SerialBT.println(litros);
      Serial.println(fregInt);
      duty = 30;
      for(int dutyCycle = 0; dutyCycle <= duty; dutyCycle++){   
        // changing the LED brightness with PWM
        ledcWrite(ledChannel, dutyCycle);
      }
    }

    if(entrada == 52){
      //SerialBT.println("4");
      Serial.println("4 SERIAL");
      SerialBT.println(litros);
      Serial.println(fregInt);
      duty = 10;
      for(int dutyCycle = 0; dutyCycle <= duty; dutyCycle++){   
        // changing the LED brightness with PWM
        ledcWrite(ledChannel, dutyCycle);
      }
    }

    if(entrada == 53){
      //SerialBT.println("5");
      Serial.println("5 SERIAL");
      SerialBT.println(litros);
      Serial.println(fregInt);
      duty = 5;
      for(int dutyCycle = 0; dutyCycle <= duty; dutyCycle++){   
        // changing the LED brightness with PWM
        ledcWrite(ledChannel, dutyCycle);
      }
    }

    if(entrada == 54){
      //SerialBT.println("6");
      Serial.println("6 SERIAL");
      SerialBT.println(litros);
      Serial.println(fregInt);
      duty = 0.5;
      for(int dutyCycle = 0; dutyCycle <= duty; dutyCycle++){   
        // changing the LED brightness with PWM
        ledcWrite(ledChannel, dutyCycle);
      }
    }
    
  }
  
  delay(20);
}
