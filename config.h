// Config wi-fi
#define WIFI_SSID "XXXXXX"
#define WIFI_PASS "XXXXXX"

// visit io.adafruit.com to retreive credentials
#define IO_USERNAME "XXXXXXXX"
#define IO_KEY "XXXXXX"

// Visit https://developer.here.com to retrieve credentials
const char* app_id = "XXXXXXX";
const char* app_code = "XXXXXX";

// Visit https://www.latlong.net to get longitude and latitude information 
// Current values is for bus stop at Virginia st & 9th Ave in Seattle, WA
// https://www.google.com/maps/place/Virginia+St+%26+9th+Ave/@47.6166119,-122.3375339,17z/data=!3m1!4b1!4m5!3m4!1s0x5490154a712cecc9:0x981687a10cecbfce!8m2!3d47.6166083!4d-122.3353452?client=safari
const char* latitude = "47.6166";
const char* longitude = "-122.3354";

// The maximum number of next departures per station to be included in response
int maxDeparture = 2;

// The maximum number of stations for which departures are required
const char* maxStations = "1";