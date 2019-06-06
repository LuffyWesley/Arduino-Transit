#include <Wire.h>   // library allows you to communicate with I2C / TWI devices
#include <ESP8266WiFi.h>    // library provides ESP8266 specific WiFi methods we are calling to connect to network
#include <ArduinoJson.h>   // Needed to parse json files
#include <ESP8266HTTPClient.h> // Needed to communicate with websites

#include "config.h" // edit the config.h tab and enter your credentials

void setup() {
 
  Serial.begin(115200); // start the serial connection

  // Prints the results to the serial monitor
  Serial.print("This board is running: ");  //Prints that the board is running
  Serial.println(F(__FILE__));
  Serial.print("Compiled: "); //Prints that the program was compiled on this date and time
  Serial.println(F(__DATE__ " " __TIME__));
 
  while(! Serial); // wait for serial monitor to open

  setup_wifi(); // calls setup_wifi funciton to connect to wifi
}

/////SETUP_WIFI/////
void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500); // wait 5 ms
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected.");  //get the unique MAC address to use as MQTT client ID, a 'truly' unique ID.
  Serial.println(WiFi.macAddress());  //.macAddress returns a byte array 6 bytes representing the MAC address
}    

void loop() {
  transitCondition(); // run weatherCondition function to get weather condition data

  delay(10000); // wait for 10 seconds
}

void transitCondition() { 
  HTTPClient theClient;
  theClient.begin(String("http://transit.api.here.com/v3/multiboard/by_geocoord.json?lang=en&center=") + latitude + "%2c" + longitude + "&time=" + nowTime + "&app_id=" + app_id + "&app_code=" + app_code + "&max=" + maxDeparture + "&maxStn=" + maxStations);
  int httpCode = theClient.GET();

  // checks wether got an error while trying to access the website/API url
  if (httpCode > 0) {
    if (httpCode == 200) {
      String payload = theClient.getString();

      const size_t capacity = 3*JSON_ARRAY_SIZE(1) + JSON_ARRAY_SIZE(5) + 6*JSON_OBJECT_SIZE(1) + 8*JSON_OBJECT_SIZE(2) + 4*JSON_OBJECT_SIZE(3) + 12*JSON_OBJECT_SIZE(4) + 2*JSON_OBJECT_SIZE(6) + 1490;
      
      DynamicJsonDocument doc(capacity);
      // DeserializationError error = deserializeJson(doc, payload);
      DeserializationError error = deserializeJson(doc, payload, DeserializationOption::NestingLimit(11));

      // Test if parsing succeeds.
      if (error) { 
        Serial.print("deserializeJson() in transitCondition() failed with code ");
        Serial.println(error.c_str());
        return;
      }

      for (int i = 0; i < maxDeparture; i++) {
        String scheduledTime = doc["Res"]["MultiNextDepartures"]["MultiNextDeparture"][0]["NextDepartures"]["Dep"][i]["time"];
        String realTime = doc["Res"]["MultiNextDepartures"]["MultiNextDeparture"][0]["NextDepartures"]["Dep"][i]["RT"]["dep"];
        String busNumber = doc["Res"]["MultiNextDepartures"]["MultiNextDeparture"][0]["NextDepartures"]["Dep"][i]["Transport"]["name"];

        Serial.println("Departure time: " + scheduledTime);
        Serial.println("Real-time departure time: " + realTime);
        Serial.println("Bus #: " + busNumber);
        Serial.println();
      }
    }
  } else {
    Serial.printf("Something went wrong with connecting to the endpoint in transitCondition().");//prints the statement in case of failure connecting to the end point
  }
}