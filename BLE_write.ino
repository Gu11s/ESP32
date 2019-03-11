/*
    Based on Neil Kolban example for IDF: https://github.com/nkolban/esp32-snippets/blob/master/cpp_utils/tests/BLE%20Tests/SampleWrite.cpp
    Ported to Arduino ESP32 by Evandro Copercini
*/

//std::string value;
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

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

class MyCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      std::string value = pCharacteristic->getValue();

      int fregInt = Freg;
      float litros = (0.0004028)*Freg + 0.01664;
      

      if (value.length() > 0) {
        Serial.println("*********");
        Serial.print("New value: ");
        for (int i = 0; i < value.length(); i++){
          Serial.print(value[i]);
        }            
        Serial.println();
        Serial.println("*********");
        if(value=="A"){
          Serial.println("hola");
          pCharacteristic->setValue(litros);
          Serial.println(fregInt);
        }else if(value=="B"){
          Serial.println("adios");
          pCharacteristic->setValue("OFF");
//          digitalWrite(led,LOW);
        }
      }
    }
};

void setup() {
  Serial.begin(115200);

  pinMode(interruptPin, INPUT_PULLUP);                                            // sets pin high
  attachInterrupt(digitalPinToInterrupt(interruptPin), handleInterrupt, RISING); // attaches pin to interrupt on Falling Edge
  timer = timerBegin(0, 80, true);

  timerStart(timer); 

  Serial.println("\n\n");
  Serial.println("1- Download and install an BLE scanner app in your phone");
  Serial.println("2- Scan for BLE devices in the app");
  Serial.println("3- Connect to MyESP32");
  Serial.println("4- Go to CUSTOM CHARACTERISTIC in CUSTOM SERVICE and write something");
  Serial.println("5- See the magic =)");
//  pinMode(led, OUTPUT);
//  digitalWrite(led, LOW);

  BLEDevice::init("MyESP32");
  BLEServer *pServer = BLEDevice::createServer();

  BLEService *pService = pServer->createService(SERVICE_UUID);

  BLECharacteristic *pCharacteristic = pService->createCharacteristic(
                                         CHARACTERISTIC_UUID,
                                         BLECharacteristic::PROPERTY_READ |
                                         BLECharacteristic::PROPERTY_WRITE
                                       );

  pCharacteristic->setCallbacks(new MyCallbacks());

  pCharacteristic->setValue("Hello World");
  pService->start();
  
  BLEAdvertising *pAdvertising = pServer->getAdvertising();
  pAdvertising->start();

}

void loop() {
  portENTER_CRITICAL(&mux);
  Freg =1000000.00/PeriodCount;                       // PeriodCount in 0.000001 of a second
  portEXIT_CRITICAL(&mux);
//  Serial.print("Frequency   ");Serial.println(Freg);
  // put your main code here, to run repeatedly: 
  delay(500);
}
