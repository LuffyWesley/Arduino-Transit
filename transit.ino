#include "config.h"  // edit the config.h tab and enter your credentials
#include <ESP8266WiFi.h>  // library provides ESP8266 specific WiFi methods we are calling to connect to network                              
#include <ESP8266HTTPClient.h>  // Needed to communicate with websites                        
#include <ArduinoJson.h>  // Needed to parse json files                             
#include <Adafruit_Sensor.h>
#include <Wire.h>  // library allows you to communicate with I2C / TWI devices
#include <Adafruit_GFX.h>  // Needed for OLED
#include <Adafruit_SSD1306.h>  // Needed for OLED
#include <NTPClient.h>    // time library which does graceful NTP server synchronization
#include <WiFiUdp.h>    // library handles UDP protocol like opening a UDP port, sending and receiving UDP packets etc
#include <Adafruit_NeoPixel.h>  // Needed for LED strip
#include <Button.h>  // Needed for buttons
#include "AdafruitIO_WiFi.h" // Needed to communicate with io.adadruit.com

#ifdef __AVR__
 #include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

AdafruitIO_WiFi io(IO_USERNAME, IO_KEY, WIFI_SSID, WIFI_PASS);

// set up the feeds for each persons location and a button feed
AdafruitIO_Feed *wordOne = io.feed("Word Input");
AdafruitIO_Feed *wordTwo = io.feed("Word Input 2");
AdafruitIO_Feed *wordThree = io.feed("Word Input 3");
AdafruitIO_Feed *wordFour = io.feed("Word Input 4");
AdafruitIO_Feed *buttons = io.feed("IO Output");

//Set variables for the IO button to change
int ty = 0;
int jon = 0;
int puru = 0;
int hue = 0;
int tyLocation = 0;
int jonLocation = 0;
int puruLocation = 0;
int hueLocation = 0;

unsigned long previousMillis = 0;  //Timer

// Set the pins for the buttons and potentiometer
Button tyBt(12, INPUT_PULLUP);  
Button jonBt(13, INPUT_PULLUP);  
Button puruBt(0, INPUT_PULLUP);
Button hueBt(2, INPUT_PULLUP);

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Declaration for LED strip
#define LED_PIN  14
#define LED_COUNT 4
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_RGBW + NEO_KHZ800);  //let the code know what its dealing with

// Adjust the UTC offset for your timezone in milliseconds (https://en.wikipedia.org/wiki/List_of_UTC_time_offsets)
// UTC -5.00 : -5 * 60 * 60 : -18000
// UTC +1.00 : 1 * 60 * 60 : 3600
// UTC -8.00 : -8 * 60 * 60 : -28800 (Washington, USA)
// UTC does not account for daylight savings, took normal UTC for washington and +1 so UTC -7
const long utcOffsetInSeconds = -25200;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds); // specify the address of the NTP Server we wish to use

void setup() {

  // start the serial connection
  Serial.begin(115200);
  // wait for serial monitor to open
  while(! Serial);
  //These four lines give description of of file name and date 
  Serial.print("This board is running: ");
  Serial.println(F(__FILE__));                       
  Serial.print("Complied: ");
  Serial.println(F(__DATE__ " " __TIME__));
  
  // connect to io.adafruit.com
  Serial.print("Connecting to Adafruit IO");
  io.connect();
  
  // set up a message handler for the 'digital' feed
  buttons->onMessage(handleMessage);

  // wait for a connection
  while(io.status() < AIO_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  // we are connected
  Serial.println();
  Serial.println(io.statusText());  // Print connection status

  int analogValue = analogRead(A0);  //Read value of slider
  int outputValue = map(analogValue,400,800,25,255);  //Map it to range of brightness with the minimum still being on
  
  strip.begin();  // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();  // Turn OFF all pixels 
  strip.setBrightness(outputValue);  // Set BRIGHTNESS to slider
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 128x32)
  display.clearDisplay();  // Clear display
  display.setTextSize(1);  // Set text size
  display.setTextColor(WHITE);  // Set text color
  display.setCursor(0, 0);  // Put cursor back on top left
  display.println("Starting up.");  // Test and write up
  display.display();  // Display the display
  
  pinMode(A0, INPUT);  // Declare slider as input

  timeClient.begin(); // initialize the NTP client
}

void loop() {
  
  // Processes any incoming data from io.adafruit.com (required)
  io.run();
                          
  // Green, Red, Blue, White
  //strip.setPixelColor(0, 255, 0, 0,0);
  // See if a button was pressed
  int action1 = tyBt.checkButtonAction();
  int action2 = jonBt.checkButtonAction();
  int action3 = puruBt.checkButtonAction();
  int action4 = hueBt.checkButtonAction();

  // If button one pressed (leftmost), change where Ty is
  if (action1 == Button::CLICKED) {                      
    ty = 1;                                               
    Serial.println("Using physical button for Ty");
  }

  // If button two pressed, change where Jon is
  if (action2 == Button::CLICKED) {                       
    jon = 1;                                              
    Serial.println("Using physical button for Jon");      
  }

  // If button three pressed, change where Puru is
  if (action3 == Button::CLICKED) {                       
    puru = 1;                                            
    Serial.println("Using physical button for Puru");     
  }

  // If button four pressed, change where Hue is
  if (action4 == Button::CLICKED) {                     
    hue = 1;                                              
    Serial.println("Using physical button for Hue");      
  }
  
  if (ty == 1) {  // Check if a Ty button was clicked
    ty = 0;  // Prepare for next click
    tyLocation++;  // Cycle the location
    Serial.println(tyLocation);
  
    if (tyLocation == 1) {                        
      strip.setPixelColor(0, 255, 0, 0,0);  //Green
      wordOne->save("Office");  //Send the value to IO
    } else if (tyLocation == 2) {
      strip.setPixelColor(0, 0, 255, 0,0);  //Red
      wordOne->save("Workshop");  //Send the value to IO
    } else if (tyLocation == 3) {
      strip.setPixelColor(0, 0, 0, 255,0);  //Blue
      wordOne->save("Lab");  //Send the value to IO
    } else if (tyLocation == 4) {
      strip.setPixelColor(0, 0, 255, 255,0);  //Magenta
      wordOne->save("Not In");  //Send the value to IO
      tyLocation = 0;  //prepare to cycle to first
    }
  } else if (jon == 1) {                            //Check if a Jon button is clicked
    jon = 0;                                      //Prepare for next click
    jonLocation++;                                //Cycle the location
    Serial.println(jonLocation);                  
    
    if (jonLocation == 1) {
      strip.setPixelColor(1, 255, 0, 0,0);        //Green
      wordTwo->save("Office");                    //Send the value to IO
    } else if (jonLocation == 2) {
      strip.setPixelColor(1, 0, 255, 0,0);        //Red
      wordTwo->save("Workshop");                  //Send the value to IO
    } else if (jonLocation == 3) {
      strip.setPixelColor(1, 0, 0, 255,0);        //Blue
      wordTwo->save("Lab");                       //Send the value to IO
    } else if (jonLocation == 4) {
      strip.setPixelColor(1, 0, 255, 255,0);      //Magenta
      wordTwo->save("Not In");                    //Send the value to IO
      jonLocation = 0;                            //Prepare to cycle back to first
    }
  } else if (puru == 1) {                           //Check if a Puru button is clicked
    puru = 0;                                     //Prepare for next click
    puruLocation++;                               //Cycle the location
    Serial.println(puruLocation);                 

    if (puruLocation == 1) {
      strip.setPixelColor(2, 255, 0, 0,0);        //Green
      wordThree->save("Office");                  //Send the value to IO
    } else if (puruLocation == 2) {
      strip.setPixelColor(2, 0, 255, 0,0);        //Red
      wordThree->save("Workshop");                //Send the value to IO
    } else if (puruLocation == 3) {
      strip.setPixelColor(2, 0, 0, 255,0);        //Blue
      wordThree->save("Lab");                     //Send the value to IO
    } else if (puruLocation == 4) {
      strip.setPixelColor(2, 0, 255, 255,0);      //Magenta
      wordThree->save("Not In");                  //Send the value to IO
      puruLocation = 0;                           //Prepare to cycle back to first
    }  
  } else if (hue == 1) {                            //Check if a Hue button is clicked
    hue = 0;                                      //Prepare for next click
    hueLocation++;                                //Cycle the location
    Serial.println(hueLocation);                  

    if (hueLocation == 1) {                        
      strip.setPixelColor(3, 255, 0, 0,0);        //Green 
      wordFour->save("Office");                   //Send the value to IO
    } else if (hueLocation == 2) {
      strip.setPixelColor(3, 0, 255, 0,0);        //Red
      wordFour->save("Workshop");                 //Send the value to IO
    } else if (hueLocation == 3) {
      strip.setPixelColor(3, 0, 0, 255,0);        //Blue
      wordFour->save("Lab");                      //Send the value to IO
    } else if (hueLocation == 4) {
      strip.setPixelColor(3, 0, 255, 255,0);      //Magenta
      wordFour->save("Not In");                   //Send the value to IO
      hueLocation = 0;                            //Prepare to cycle back to first
    }
  }
  strip.show();                                   //Show off those colors
  
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= 30000) { // timer, wait 30 secs before calling API
    previousMillis = currentMillis;
    transitCondition(); // get that transit data
  }
}

void handleMessage(AdafruitIO_Data *data) {       //Deal with any incoming data from IO

  Serial.print("received <- "); 
  
  if(data->toInt() == 1) {                        //Make it not a pointer so it can be used, then see if pressed or not
    Serial.println("Ty is setting location");     
    ty = 1;                                       //If pressed get ready to change ty location
  }
  
  if(data->toInt() == 2) {                        //Make it not a pointer so it can be used, then see if pressed or not
    Serial.println("Jon is setting location");    
    jon = 1;                                      //If pressed get ready to change jon location
  }

  if(data->toInt() == 3) {                       //Make it not a pointer so it can be used, then see if pressed or not
    Serial.println("Puru is setting location");   
    puru = 1;                                     //If pressed get ready to change puru location
  }

  if(data->toInt() == 4) {                       //Make it not a pointer so it can be used, then see if pressed or not
    Serial.println("Hue is setting location");   
    hue = 1;                                     //If pressed get ready to change hue location
  }
}

void transitCondition() { 

  timeClient.update();    //call the update() function whenever we want current day & time
  int hrs = timeClient.getHours();
  int mins = timeClient.getMinutes();
  int secs = timeClient.getSeconds();
  String formattedDate = timeClient.getFormattedDate(); // 2019-06-06T11:56:09Z
  
  // Extract date 2019-06-06
  int splitT = formattedDate.indexOf("T");
  String dayStamp = formattedDate.substring(0, splitT);

  String nowTime = "" + dayStamp + "T" + hrs + "%3A" + mins + "%3A" + secs;  // current date and time in url parameter format
  
  HTTPClient theClient;
  theClient.begin(String("http://transit.api.here.com/v3/multiboard/by_geocoord.json?lang=en&center=") + latitude + "%2c" + longitude + "&time=" + nowTime + "&app_id=" + app_id + "&app_code=" + app_code + "&max=" + maxDeparture + "&maxStn=" + maxStations);
  int httpCode = theClient.GET();
  // checks wether got an error while trying to access the website/API url
  if (httpCode > 0) {
    if (httpCode == 200) {
      String payload = theClient.getString();
      // Scale buffer according to size of API, revisit if the size is to big. Go to https://arduinojson.org/v6/assistant/ for help resizing
      const size_t capacity = 3*JSON_ARRAY_SIZE(1) + JSON_ARRAY_SIZE(5) + 6*JSON_OBJECT_SIZE(1) + 8*JSON_OBJECT_SIZE(2) + 4*JSON_OBJECT_SIZE(3) + 12*JSON_OBJECT_SIZE(4) + 2*JSON_OBJECT_SIZE(6) + 1490;
      
      DynamicJsonDocument doc(capacity);
      DeserializationError error = deserializeJson(doc, payload, DeserializationOption::NestingLimit(11));

      // Test if parsing succeeds.
      if (error) { 
        Serial.print("deserializeJson() in transitCondition() failed with code ");
        Serial.println(error.c_str());
        return;
      }

      // Loop through json based on maxDeparture number on config.h file. Current number is 2 meaning that code will only display the next two bus departure times
      for (int i = 0; i < maxDeparture; i++) {
        String scheduledTime = doc["Res"]["MultiNextDepartures"]["MultiNextDeparture"][0]["NextDepartures"]["Dep"][i]["time"];
        String realTime = doc["Res"]["MultiNextDepartures"]["MultiNextDeparture"][0]["NextDepartures"]["Dep"][i]["RT"]["dep"];
        String busNumber = doc["Res"]["MultiNextDepartures"]["MultiNextDeparture"][0]["NextDepartures"]["Dep"][i]["Transport"]["name"];

        // Formatting time from 2019-06-06T10:26:00 to 12:31:00
        int split = scheduledTime.indexOf("T");
        String niceTime = scheduledTime.substring(split+1);

        // Conditional statement incase API sends null data for real time departure. Statement make real time to be the same as scheduled data
        // Null appears only when real time departure is the same as scheduled time
        if (realTime == "null") {
          realTime = scheduledTime;
        }

        // Formatting time from 2019-06-06T10:26:00 to 12:31:00
        int split1 = realTime.indexOf("T");
        String niceRealTime = realTime.substring(split+1); 

        Serial.println("Departure time: " + niceTime);
        Serial.println("Real-time departure time: " + niceRealTime);
        Serial.println("Bus #: " + busNumber);
        Serial.println();

        display.clearDisplay();  //Clear test display off
        display.setCursor(0, 0);  //Reset cursor
        display.print("The # ");
        display.print(busNumber);
        display.println(" leaves in ");
        display.println(niceRealTime);               
        display.display();  //Display that display 
      }
    }
  } else {
    Serial.printf("Something went wrong with connecting to the endpoint in transitCondition().");//prints the statement in case of failure connecting to the end point
  }
}