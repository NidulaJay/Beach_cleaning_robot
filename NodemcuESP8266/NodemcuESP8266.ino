#include <SoftwareSerial.h>

#define RX_PIN_NODEMCU D2 
#define TX_PIN_NODEMCU D3 

SoftwareSerial nodeMCUSerial(RX_PIN_NODEMCU, TX_PIN_NODEMCU);

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>


#include <addons/TokenHelper.h>
#include <addons/RTDBHelper.h>

/* 1. Define the WiFi credentials */
#define WIFI_SSID "DESKTOP-V8VL3BM 5019"
#define WIFI_PASSWORD "885H^8h2"

// For the following credentials, see examples/Authentications/SignInAsUser/EmailPassword/EmailPassword.ino

/* 2. Define the API Key */
#define API_KEY "AIzaSyCzp4vCM-CzxYubZXYXB0ORVOBerfrpB0s"

/* 3. Define the RTDB URL */
#define DATABASE_URL "beach-robot-assignment-default-rtdb.firebaseio.com" //<databaseName>.firebaseio.com or <databaseName>.<region>.firebasedatabase.app

/* 4. Define the user Email and password that alreadey registerd or added in your project */
#define USER_EMAIL "greensuper924@gmail.com"
#define USER_PASSWORD "123456789"

FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;

unsigned long count = 0;

#if defined(ARDUINO_RASPBERRY_PI_PICO_W)
WiFiMulti multi;
#endif

void setup() {
  Serial.begin(9600); // Initialize Serial for debugging
  nodeMCUSerial.begin(9600); // Initialize SoftwareSerial for communication with Arduino Mega
  #if defined(ARDUINO_RASPBERRY_PI_PICO_W)
  multi.addAP(WIFI_SSID, WIFI_PASSWORD);
  multi.run();
#else
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
#endif

  Serial.print("Connecting to Wi-Fi");
  unsigned long ms = millis();
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(300);
#if defined(ARDUINO_RASPBERRY_PI_PICO_W)
    if (millis() - ms > 10000)
      break;
#endif
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  Serial.printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);

  config.api_key = API_KEY;

  /* Assign the user sign in credentials */
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;

  /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL;

  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; // see addons/TokenHelper.h

  // Comment or pass false value when WiFi reconnection will control by your code or third party library e.g. WiFiManager
  Firebase.reconnectNetwork(true);

  // Since v4.4.x, BearSSL engine was used, the SSL buffer need to be set.
  // Large data transmission may require larger RX buffer, otherwise connection issue or data read time out can be occurred.
  fbdo.setBSSLBufferSize(4096 /* Rx buffer size in bytes from 512 - 16384 */, 1024 /* Tx buffer size in bytes from 512 - 16384 */);

  // The WiFi credentials are required for Pico W
  // due to it does not have reconnect feature.
#if defined(ARDUINO_RASPBERRY_PI_PICO_W)
  config.wifi.clearAP();
  config.wifi.addAP(WIFI_SSID, WIFI_PASSWORD);
#endif


  Firebase.begin(&config, &auth);

  Firebase.setDoubleDigits(5);
}



void loop() {
  //Serial.println("run");
  String name1 = "satus1";
  String name2 = "belt";
  String status = Firebase.getString(fbdo, F("/status/status")) ? fbdo.to<const char *>() : fbdo.errorReason().c_str();
  String belt = Firebase.getString(fbdo, F("/status/belt")) ? fbdo.to<const char *>() : fbdo.errorReason().c_str();
  Serial.println(status);
  
  String message = name1 + ":" + String(status) + "," + name2 + ":" + String(belt);
      nodeMCUSerial.println(message);
      

  if (nodeMCUSerial.available()) {
    
    String message = nodeMCUSerial.readStringUntil('\n');

    
    Serial.println("Received message: " + message);

    
    int commaIndex = message.indexOf(",");
    if (commaIndex != -1) {
      String pair1 = message.substring(0, commaIndex);
      String pair2 = message.substring(commaIndex + 1);

      
      int colonIndex1 = pair1.indexOf(":");
      if (colonIndex1 != -1) {
        String name1 = pair1.substring(0, colonIndex1);
        String valueString1 = pair1.substring(colonIndex1 + 1);
        float value1 = valueString1.toFloat();
        
        if (name1 == "scale"){
            Serial.printf("Set int... %s\n", Firebase.setInt(fbdo, F("/scale/weight"), value1) ? "ok" : fbdo.errorReason().c_str());
           
        }
        else if (name1 == "compassx"){
            Serial.printf("Set int... %s\n", Firebase.setInt(fbdo, F("/compass/xaxis"), value1) ? "ok" : fbdo.errorReason().c_str());

        }
        else if (name1 == "accelerox"){
            Serial.printf("Set int... %s\n", Firebase.setInt(fbdo, F("/accelerometer/x"), value1) ? "ok" : fbdo.errorReason().c_str());

        }
        else if (name1 == "Gpslong"){
          Serial.println("gps recived");
            Serial.printf("Set string... %s\n", Firebase.setString(fbdo, F("/GPS/longtitude"), value1) ? "ok" : fbdo.errorReason().c_str());

        }
        
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
        
        if (name2 == "compassy"){
            Serial.printf("Set int... %s\n", Firebase.setInt(fbdo, F("/compass/yaxis"), value2) ? "ok" : fbdo.errorReason().c_str());

        }
        else if (name2 == "acceleroy"){
            Serial.printf("Set int... %s\n", Firebase.setInt(fbdo, F("/accelerometer/y"), value2) ? "ok" : fbdo.errorReason().c_str());

        }
        else if (name2 == "Gpslati"){
            Serial.printf("Set String... %s\n", Firebase.setString(fbdo, F("/GPS/latitude"), value2) ? "ok" : fbdo.errorReason().c_str());

        }
        
        // Serial.print("Name 2: ");
        // Serial.println(name2);
        // Serial.print("Value 2: ");
        // Serial.println(value2);
        // Serial.printf("Set int... %s\n", Firebase.setInt(fbdo, F("/robot/error"), value2) ? "ok" : fbdo.errorReason().c_str());
      } else {
        Serial.println("Invalid pair format: " + pair2);
      }
    } else {
      Serial.println("Invalid message format");
    }
  }
}
