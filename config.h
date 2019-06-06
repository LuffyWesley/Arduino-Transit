// Config wi-fi
#define WIFI_SSID "University of Washington"
#define WIFI_PASS ""

// Create a developer account https://developer.here.com 
// Then get credentials by pressing button under Javascript/Rest
const char* app_id = "";
const char* app_code = "";

// Visit https://www.latlong.net to get longitude and latitude information 
// Current values is for bus stop at Virginia st & 9th Ave in Seattle, WA
const char* latitude = "47.6166";
const char* longitude = "-122.3354";

// Configure time and date
// Format is as follows: yyyy-mm-ddThh:mm:ss (24 hr format)
// %3A represents :
const char* nowTime = "2019-06-05T19%3A30%3A00";

// The maximum number of next departures per station to be included in response
int maxDeparture = 5;

// The maximum number of stations for which departures are required
const char* maxStations = "1";