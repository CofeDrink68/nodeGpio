#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
 
const char* ssid = "WIFI SSID";
const char* password = "WIFI PSK";
#define button 15 // PIN D8

int m = 1; // 0 is for impulse (time limitation) and 1 is for broadcast (remote pin)
int _mode = INPUT_PULLUP; 
int id = 1;

char* onRequests = "http://192.168.1.97:8000/?piston=1&state=on";
char* offRequests = "http://192.168.1.97:8000/?piston=1&state=off";
char* impRequests = "http://192.168.1.97:8000/?piston=1&state=imp";

void setup () {
  pinMode(button, _mode);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting..");
    yield();
  }
  Serial.println("Connected");
  digitalWrite(LED_BUILTIN, HIGH);
}

int getInput(int _button) {
  if(_mode == INPUT) {
    return !digitalRead(_button);
  } else {
    return digitalRead(_button);
  }
}
void loop() {
  if (WiFi.status() == WL_CONNECTED) { //Check WiFi connection status
    if(m) { // broadcast
      HTTPClient http;  //Declare an object of class HTTPClient
      if(getInput(button))   {http.begin(onRequests);}
      else                   {http.begin(offRequests);;}
      int httpCode = http.GET();                                                                  //Send the request
      if (httpCode > 0) { //Check the returning code
        String payload = http.getString();   //Get the request response payload
        Serial.println(payload);                     //Print the response payload
      }
      http.end();   //Close connection
    } else { // impulse
      HTTPClient http;  //Declare an object of class HTTPClient
      while(getInput(button)) {yield(); /* Thanks Watchdog */} // On attends que le boutton soit relaché
      http.begin(impRequests);
      while(not getInput(button)) {yield(); /* Thanks Watchdog */} // On attends un appui sur le boutton 
      int httpCode = http.GET();                                                                  //Send the request
      if (httpCode > 0) { //Check the returning code
        String payload = http.getString();   //Get the request response payload
        Serial.println(payload);                     //Print the response payload
     
      }
       http.end();   //Close connection 
    }
  }
  delay(100);    // Try send a request every 100 ms for debouncing
}
