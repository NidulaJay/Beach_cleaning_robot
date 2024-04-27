#include <HX711_ADC.h>
#if defined(ESP8266)|| defined(ESP32) || defined(AVR)
#include <EEPROM.h>
#endif
#include <AFMotor.h>
#include <QMC5883LCompass.h>

//#include <SoftwareSerial.h>

// #define RX_PIN_A9G 32 // Receive pin on Arduino Mega
// #define TX_PIN_A9G 33 // Transmit pin on Arduino Mega

//#define TX_PINfornodemcu_MEGA 24 // Transmit pin on Arduino Mega
//#define RX_PINfornodemcu_MEGA 25 // Receive pin on Arduino Mega

//SoftwareSerial nodemcuserial(TX_PINfornodemcu_MEGA, RX_PINfornodemcu_MEGA);
//SoftwareSerial A9G(RX_PIN_A9G, TX_PIN_A9G);

//We use Hardware serial insteed of software serial because when using library for load cell the software serial is not working fine and also arduino mega has multiple hardware serial ports.

#define trig1 40  //1
#define echo1 41  //1
#define trig2 42  //2
#define echo2 43  //2
#define trig3 44  //3
#define echo3 45  //3
#define trig4 46  //4
#define echo4 47  //4
#define trig5 48  //5
#define echo5 49  //5
#define trig6 50  //6
#define echo6 51  //6

QMC5883LCompass compass;

AF_DCMotor motor(1); // front crusher
AF_DCMotor motor1(2); // left 
AF_DCMotor motor2(3);  // right
AF_DCMotor motor3(4);  // belt

const int HX711_dout = 36; //mcu > HX711 dout pin
const int HX711_sck = 37; //mcu > HX711 sck pin

//HX711 constructor:
HX711_ADC LoadCell(HX711_dout, HX711_sck);

const int calVal_eepromAdress = 0;
unsigned long t = 0;

#define xPin A12
#define yPin A13
#define zPin A14

const int samples = 10;

unsigned long currentMillis = 0;

unsigned long accPreviousMillis = 0;
unsigned long comPreviousMillis = 0;
unsigned long scalePreviousMillis = 0;
unsigned long locationPreviousMillis = 0;

const unsigned long accblinkperiod = 500;
const unsigned long comblinkperiod = 400;
const unsigned long scaleblinkperiod = 450;
const unsigned long locationblinkperiod = 550;

void setup() {
  pinMode(trig1, OUTPUT);
  pinMode(echo1, INPUT);
  pinMode(trig2, OUTPUT);
  pinMode(echo2, INPUT);
  pinMode(trig3, OUTPUT);
  pinMode(echo3, INPUT);
  pinMode(trig4, OUTPUT);
  pinMode(echo4, INPUT);
  pinMode(trig5, OUTPUT);
  pinMode(echo5, INPUT);
  pinMode(trig6, OUTPUT);
  pinMode(echo6, INPUT);
  Serial.begin(9600);
  LoadCell.begin();
  //nodemcuserial.begin(9600);


  //Hardware Serial
  Serial1.begin(115200);
  Serial2.begin(9600);
  float calibrationValue; 
  calibrationValue = 696.0; 
#if defined(ESP8266)|| defined(ESP32)
  
#endif
  EEPROM.get(calVal_eepromAdress, calibrationValue); 

  unsigned long stabilizingtime = 2000; 
  boolean _tare = true; 
  LoadCell.start(stabilizingtime, _tare);
  if (LoadCell.getTareTimeoutFlag()) {
    Serial.println("Timeout, check MCU>HX711 wiring and pin designations");
    while (1);
  }
  else {
    LoadCell.setCalFactor(calibrationValue); 
    Serial.println("Startup is complete");
  }
  compass.init();
  
  compass.setCalibrationOffsets(-59.00, 328.00, 20.00);
  compass.setCalibrationScales(0.90, 0.94, 1.21);

  //send at command to A9G module to turn on gps
  Serial1.println("AT+GPS=1");
}


void readScale(){

  //use millis to feels liike multitasking and improve speed and efficiecy
  if ( currentMillis - scalePreviousMillis >= scaleblinkperiod ) {
   scalePreviousMillis = currentMillis;
        static boolean newDataReady = 0;
  const int serialPrintInterval = 0; //increase value to slow down serial print activity


  if (LoadCell.update()) newDataReady = true;

  if (newDataReady) {
    if (millis() > t + serialPrintInterval) {
      float i = LoadCell.getData();
      Serial.print("Load_cell output val: ");
      Serial.println(i);
      String name1 = "scale";
      float value1 = i;
      String name2 = "nuull";
      float value2 = 0;
  

  // send value to nodemcu using hardware serial
  String message = name1 + ":" + String(value1) + "," + name2 + ":" + String(value2);
    Serial2.println(message);
      newDataReady = 0;
      t = millis();
    }
  }

  // receive command from serial terminal, send 't' to initiate tare operation:
  if (Serial.available() > 0) {
    char inByte = Serial.read();
    if (inByte == 't') LoadCell.tareNoDelay();
  }

  // check if last tare operation is complete:
  if (LoadCell.getTareStatus() == true) {
    Serial.println("Tare complete");
  }

  delay(1000);
  }
  }
   
  


void readAccelerometer(){
  
//use millis to feels liike multitasking and improve speed and efficiecy
  if ( currentMillis - accPreviousMillis >= accblinkperiod ) {
   accPreviousMillis = currentMillis;
  int xRaw=0,yRaw=0,zRaw=0;
  for(int i=0;i<samples;i++)
  {
    xRaw+=analogRead(xPin);
    yRaw+=analogRead(yPin);
    zRaw+=analogRead(zPin);
  }
  xRaw/=samples;
  yRaw/=samples;
  zRaw/=samples;

 // send value to nodemcu using hardware serial

  String name1 = "accelerox";
  String name2 = "acceleroy";
    String message = name1 + ":" + String(xRaw) + "," + name2 + ":" + String(yRaw);

  Serial2.println(message);
 
  Serial.print(xRaw);
  Serial.print("\t");
  Serial.print(yRaw);
  Serial.print("\t");
  Serial.print(zRaw);
  Serial.println();

  delay(1000);
}
}

void readCompass(){

  if ( currentMillis - comPreviousMillis >= comblinkperiod ) {
   comPreviousMillis = currentMillis;
      int x, y, z;
  
  // Read compass values
  compass.read();

  // Return XYZ readings
  x = compass.getX();
  y = compass.getY();
  z = compass.getZ();
  
  Serial.print("X: ");
  Serial.print(x);
  Serial.print(" Y: ");
  Serial.print(y);
  Serial.print(" Z: ");
  Serial.print(z);
  Serial.println();

 // send value to nodemcu using hardware serial
  
  String name1 = "compassx";
  String name2 = "compassy";
    String message = name1 + ":" + String(x) + "," + name2 + ":" + String(y);

  Serial2.println(message);
  delay(1000);

  }
    
}

void getlocation() {

//use millis to feels liike multitasking and improve speed and efficiecy
if ( currentMillis - locationPreviousMillis >= locationblinkperiod ) {
   locationPreviousMillis = currentMillis;

       //send AT command to A9G module to retrive gps location
       Serial1.println("AT+LOCATION=2");

   delay(1000); 

    
    if (Serial1.available() > 0) {
      // Read the response from the device
      String response = Serial1.readString();

      
      //Serial.print("Response from device: ");
      //Serial.println(response);

      // Check if the response contains the expected substring indicating location data
      if (response.indexOf("+LOCATION:") != -1) {
        
        //Serial.println("Location data received:");

        // Find the position of the first comma in the response
        int commaIndex = response.indexOf(',');
        if (commaIndex != -1) {
          // Extract longitude substring from the response
          String longitude = response.substring(commaIndex + 1, commaIndex + 11);

          //Serial.print("Longitude: ");
          //Serial.println(longitude);

          // Find the position of the second comma in the response
          int secondCommaIndex = response.indexOf(',', commaIndex + 1);
          if (secondCommaIndex != -1) {
            // Extract latitude substring from the response
            String latitude = response.substring(secondCommaIndex + 1, secondCommaIndex + 10);
            String name1 = "Gpslong";
            String name2 = "Gpslati";
            String message = name1 + ":" + String(longitude) + "," + name2 + ":" + String(latitude);
             Serial2.println(message);
            delay(1000);
            // Serial.print("Latitude: ");
            // Serial.println(latitude);
          }
        }
      }
    } else {
      // No response received from the device
      Serial.println("No response received from device.");
    }
    delay(1000);
}
 
}

void getstatus(){
  if (Serial2.available()) {
    // Read the incoming message
    String message = Serial2.readStringUntil('\n');

    // Print received message
    Serial.println("Received message: " + message);

    // Extract data from the message
    int commaIndex = message.indexOf(",");
    if (commaIndex != -1) {
      String pair1 = message.substring(0, commaIndex);
      String pair2 = message.substring(commaIndex + 1);

      // Extract name and value from pair1
      int colonIndex1 = pair1.indexOf(":");
      if (colonIndex1 != -1) {
        String name1 = pair1.substring(0, colonIndex1);
        String valueString1 = pair1.substring(colonIndex1 + 1);
        float value1 = valueString1.toFloat();
        
        if( valueString1 == "go"){
          Serial.println("go");
            motor1.setSpeed(255);
            motor2.setSpeed(255);
            motor1.run(FORWARD);
            motor2.run(FORWARD);

        }
      else  if( valueString1 == "back"){
          Serial.println("back");
            motor1.setSpeed(255);
            motor2.setSpeed(255);
            motor1.run(BACKWARD);
            motor2.run(BACKWARD);

        }
      else  if( valueString1 == "left"){
          Serial.println("left");
            motor1.setSpeed(255);
            motor2.setSpeed(255);
            motor1.run(BACKWARD);
            motor2.run(FORWARD);

        }
      else  if( valueString1 == "right"){
          Serial.println("right");
            motor1.setSpeed(255);
            motor2.setSpeed(255);
            motor1.run(FORWARD);
            motor2.run(BACKWARD);

        }
      else  if( valueString1 == "stop"){
          Serial.println("stop");
            motor1.setSpeed(0);
            motor2.setSpeed(0);
            motor1.run(RELEASE);
            motor2.run(RELEASE);

        }
        
        // Print parsed data
        // Serial.print("Name 1: ");
        // Serial.println(name1);
        
        // Serial.print("Value 1: ");
        // Serial.println(value1);
        
      } else {
        Serial.println("Invalid pair format: " + pair1);
      }

      // Extract name and value from pair2
      int colonIndex2 = pair2.indexOf(":");
      if (colonIndex2 != -1) {
        String name2 = pair2.substring(0, colonIndex2);
        String valueString2 = pair2.substring(colonIndex2 + 1);
        float value2 = valueString2.toFloat();
        Serial.println(valueString2);
        if (valueString2 == "run") {
          Serial.print("runmotor");
            motor.setSpeed(250);
            motor3.setSpeed(250);
            motor.run(FORWARD);
            motor3.run(FORWARD);
        }
      else  if (valueString2 == "stop") {
            motor.setSpeed(0);
            motor3.setSpeed(0);
            motor.run(RELEASE);
            motor3.run(RELEASE);
        }
        
        // Print parsed data
        // Serial.print("Name 2: ");
        // Serial.println(name2);
        // Serial.print("Value 2: ");
        // Serial.println(value2);
        // Serial.printf("Set int... %s\n", Firebase.setInt(fbdo, F("/robot/humidility"), value2) ? "ok" : fbdo.errorReason().c_str());
      } else {
        Serial.println("Invalid pair format: " + pair2);
      }
    } else {
      Serial.println("Invalid message format");
    }
  }


}
void sensor(){
  digitalWrite(trig1,LOW);
    delayMicroseconds(2);
    digitalWrite(trig1, HIGH);
    delayMicroseconds(10);
    digitalWrite(trig1,LOW);

  long t1 = pulseIn(echo1,HIGH);

  digitalWrite(trig2,LOW);
    delayMicroseconds(2);
    digitalWrite(trig2, HIGH);
    delayMicroseconds(10);
    digitalWrite(trig2,LOW);

    long t2 = pulseIn(echo2,HIGH);

    long inches1 = t1/74/2;
    long cm1 = t1/29/2;
    long inches2 = t2/74/2;
    long cm2 = t2/29/2;

    digitalWrite(trig5,LOW);
    delayMicroseconds(2);
    digitalWrite(trig5, HIGH);
    delayMicroseconds(10);
    digitalWrite(trig5,LOW);

  long t5 = pulseIn(echo5,HIGH);

  digitalWrite(trig6,LOW);
    delayMicroseconds(2);
    digitalWrite(trig6, HIGH);
    delayMicroseconds(10);
    digitalWrite(trig6,LOW);

    long t6 = pulseIn(echo6,HIGH);

    long inches5 = t5/74/2;
    long cm5 = t5/29/2;
    long inches6 = t6/74/2;
    long cm6 = t6/29/2;
    digitalWrite(trig3,LOW);
    delayMicroseconds(2);
    digitalWrite(trig3, HIGH);
    delayMicroseconds(10);
    digitalWrite(trig3,LOW);

  long t3 = pulseIn(echo3,HIGH);

  digitalWrite(trig4,LOW);
    delayMicroseconds(2);
    digitalWrite(trig4, HIGH);
    delayMicroseconds(10);
    digitalWrite(trig4,LOW);

    long t4 = pulseIn(echo4,HIGH);

    long inches3 = t3/74/2;
    long cm3 = t3/29/2;
    long inches4 = t4/74/2;
    long cm4 = t4/29/2;

  if (cm1 <= 30 || cm2 <= 30 || cm5 <= 30 || cm6 <= 30 || cm3 <= 30 || cm4 <= 30 ) {
    motor1.setSpeed(0);
    motor2.setSpeed(0);
    motor1.run(RELEASE);
    motor2.run(RELEASE);
  }




    

    Serial.print(inches1);
    Serial.print(inches2);
    Serial.print("in  \t");
    Serial.print(cm1);
    Serial.print(cm2);
    Serial.println("cm");
    delay(100);
}


void back(){
  digitalWrite(trig5,LOW);
    delayMicroseconds(2);
    digitalWrite(trig5, HIGH);
    delayMicroseconds(10);
    digitalWrite(trig5,LOW);

  long t5 = pulseIn(echo5,HIGH);

  digitalWrite(trig6,LOW);
    delayMicroseconds(2);
    digitalWrite(trig6, HIGH);
    delayMicroseconds(10);
    digitalWrite(trig6,LOW);

    long t6 = pulseIn(echo6,HIGH);

    long inches5 = t5/74/2;
    long cm5 = t5/29/2;
    long inches6 = t6/74/2;
    long cm6 = t6/29/2;

  if (cm5 <= 30 || cm6 <= 30) {
    motor1.setSpeed(0);
    motor2.setSpeed(0);
    motor1.run(RELEASE);
    motor2.run(RELEASE);
  }
    

    Serial.print(inches5);
    Serial.print(inches6);
    Serial.print("in  \t");
    Serial.print(cm5);
    Serial.print(cm6);
    Serial.println("cm");
    delay(100);
}


void loop() {
  currentMillis = millis();
  readAccelerometer();
  readCompass();
  readScale();
  Serial.println("run");
  getlocation();
  getstatus();
  sensor();
  //Serial1.println("AT+GPS=1 \r");
}
