#include "Configuration.h"
#include "StartPage.h"
#include <WiFiClient.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <Wire.h>
#include <Adafruit_NeoPixel.h>

//variable for Wifi
const char* ssid = STASSID;
const char* password = STAPSK;

//Create a webserver objrt that listens for HTTP request on port 80
ESP8266WebServer server(PORT_SERVER);

IPAddress IP(IP_ADDRESS);
IPAddress mask = (GATEWAY);

// function prototypes for HTTP handlers
void handleRoot();
void DailyGauge();
void RacingGauge();
void FunnyGauge();
void BackPlane();
void FrontPlane();
void GaugeOFF();  
void BrightnessDeafult();
void BrightnessBrighter();
//handler
//Function LED            
void handleNotFound();
int BackPlane_RPM();
int FrontPlane_RPM();
int BackPlane_Speed();
int FrontPlane_Speed();
int Display_LCD();
//Function LED

//Configuration for LED_RPM_GAUGE
Adafruit_NeoPixel backplane_rpm = Adafruit_NeoPixel(QUANTITY_LED_BACKPLANE_RPM, PIN_D7_BACKPLANE_RPM, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel frontplane_rpm = Adafruit_NeoPixel(QUANTITY_LED_FRONTPLANE_RPM, PIN_D6_FRONTPLANE_RPM, NEO_GRB + NEO_KHZ800);
//Configuration for LED_SPEED_GAUGE
Adafruit_NeoPixel backplane_speed = Adafruit_NeoPixel(QUANTITY_LED_BACKPLANE_SPEED, PIN_D8_BACKPLANE_SPEED, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel frontplane_speed = Adafruit_NeoPixel(QUANTITY_LED_FRONTPLANE_SPEED, PIN_D5_FRONTPLANE_SPEED, NEO_GRB + NEO_KHZ800);
//Configuration for DISPLAY
Adafruit_NeoPixel display_lcd = Adafruit_NeoPixel(QUANTITY_LED_DISPLAY, PIN_D1_DISPLAY, NEO_GRB + NEO_KHZ800);

//variable for functions
int daily_gauge_loop;
int brightenss_FrontPlane = BRIGHTNESS_FRONTPLANE;
int brightenss_BackPlane = BRIGHTNESS_BACKPLANE;
int brightenss_display = BRIGHTNESS_DISPLAY;
int firstStart = true;

void setup(){
  Serial.begin(115200);
  /**
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print("Connecting to server ");
    Serial.print(ssid);
    Serial.println("");
    digitalWrite(LED_ONBOARD, HIGH);
    delay(200);
    digitalWrite(LED_ONBOARD, LOW);
  }
  Serial.print("Connected to ");
  Serial.println(WiFi.SSID());
  //Serial.print("IP address: ");
  //Serial.println(WiFi.localIP());
  **/
  
  Serial.println("Configuring access point...");
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password);
  WiFi.softAPConfig(IP, IP, mask);
  Serial.print("IP: "); Serial.print(WiFi.softAPIP());
  Serial.print("MAC:"); Serial.println(WiFi.softAPmacAddress());
  Serial.println("Server started.");
  
  WiFi.printDiag(Serial);
    
  // Inicjalizacja biblioteki LED
  backplane_rpm.begin();
  backplane_rpm.setBrightness(brightenss_BackPlane);
  frontplane_rpm.begin();
  frontplane_rpm.setBrightness(brightenss_FrontPlane);
  backplane_speed.begin();
  backplane_speed.setBrightness(brightenss_BackPlane);
  frontplane_speed.begin();
  frontplane_speed.setBrightness(brightenss_FrontPlane);
  display_lcd.begin();
  display_lcd.setBrightness(brightenss_display);

  server.on("/", HTTP_GET, handleRoot);  // Call the 'handleLED' function when a POST request is made to URI "/LED"
  server.on("/DailyGauge", HTTP_GET, DailyGauge);
  server.on("/RacingGauge", HTTP_GET, RacingGauge);  // Call the 'handleLED' function when a POST request is made to URI "/LED"
  server.on("/FunnyGauge", HTTP_GET, FunnyGauge); 
  server.on("/BackPlane", HTTP_GET, BackPlane);
  server.on("/FrontPlane", HTTP_GET, FrontPlane);
  server.on("/BrightnessDeafult", HTTP_GET, BrightnessDeafult);
  server.on("/BrightnessBrighter", HTTP_GET, BrightnessBrighter);
  server.on("/GaugeOFF", HTTP_GET, GaugeOFF);
  server.onNotFound(handleNotFound);        // When a client requests an unknown URI (i.e. something other than "/"), call function "handleNotFound"

  server.begin();                           // Actually start the server
  Serial.println("HTTP server started");
}
 
void loop()
{ 
  server.handleClient();                    // Listen for HTTP requests from clients
  if(firstStart == true){
    BackPlane_RPM(0, GREEN_COLOR, 0);
    FrontPlane_RPM(ORANGE_COLOR_FIRST, ORANGE_COLOR_SECOND, ORANGE_COLOR_THIRD);
    BackPlane_Speed(0, GREEN_COLOR, 0);
    FrontPlane_Speed(ORANGE_COLOR_FIRST, ORANGE_COLOR_SECOND, ORANGE_COLOR_THIRD);
    Display_LCD(ORANGE_COLOR_FIRST, ORANGE_COLOR_SECOND, ORANGE_COLOR_THIRD);
    firstStart = false;
  }
}

//handler
void handleRoot() {    // When URI / is requested, send a web page with a button to toggle the LED
  #ifdef DEBUG
     Serial.println("Start page");
  #endif
  server.send(200, "text/html", Menu_Celica_Gauge);
}

void GaugeOFF(){
  #ifdef DEBUG
     Serial.println("GaugeOFF");
  #endif
  server.send(200, "text/html", Menu_Celica_Gauge);
  server.sendHeader("Location","/"); 
  BackPlane_RPM(NON_COLOR, NON_COLOR, NON_COLOR);
  FrontPlane_RPM(NON_COLOR, NON_COLOR, NON_COLOR);
  BackPlane_Speed(NON_COLOR, NON_COLOR, NON_COLOR);
  FrontPlane_Speed(NON_COLOR, NON_COLOR, NON_COLOR);
  Display_LCD(NON_COLOR, NON_COLOR, NON_COLOR);
}

void DailyGauge(){
  #ifdef DEBUG
     Serial.println("DailyGauge");
  #endif
  server.send(200, "text/html", Menu_Celica_Gauge);
  server.sendHeader("Location","/");  
  BackPlane_RPM(0, 0, BLUE_COLOR);
  FrontPlane_RPM(ORANGE_COLOR_FIRST, ORANGE_COLOR_SECOND, ORANGE_COLOR_THIRD);
  BackPlane_Speed(0, 0, BLUE_COLOR);
  FrontPlane_Speed(ORANGE_COLOR_FIRST, ORANGE_COLOR_SECOND, ORANGE_COLOR_THIRD);
  Display_LCD(ORANGE_COLOR_FIRST, ORANGE_COLOR_SECOND, ORANGE_COLOR_THIRD);
}

void RacingGauge(){
  #ifdef DEBUG
     Serial.println("RacingGauge");
  #endif
  server.send(200, "text/html", Menu_Celica_Gauge);
  server.sendHeader("Location","/"); 
  BackPlane_RPM(RED_COLOR, NON_COLOR, NON_COLOR);
  FrontPlane_RPM(RED_COLOR, NON_COLOR, NON_COLOR);
  BackPlane_Speed(RED_COLOR, NON_COLOR, NON_COLOR);
  FrontPlane_Speed(RED_COLOR, NON_COLOR, NON_COLOR);
  Display_LCD(RED_COLOR, NON_COLOR, NON_COLOR);
}

void FunnyGauge(){
  #ifdef DEBUG
     Serial.println("FunnyGauge");
  #endif
  server.send(200, "text/html", Menu_Celica_Gauge);
  server.sendHeader("Location","/"); 
  BackPlane_RPM(FUNNY_COLOR_FIRST, FUNNY_COLOR_SECOND, FUNNY_COLOR_THIRD);
  FrontPlane_RPM(FUNNY_COLOR_FIRST, FUNNY_COLOR_SECOND, FUNNY_COLOR_THIRD);
  BackPlane_Speed(FUNNY_COLOR_FIRST, FUNNY_COLOR_SECOND, FUNNY_COLOR_THIRD);
  FrontPlane_Speed(FUNNY_COLOR_FIRST, FUNNY_COLOR_SECOND, FUNNY_COLOR_THIRD);
  Display_LCD(FUNNY_COLOR_FIRST, FUNNY_COLOR_SECOND, FUNNY_COLOR_THIRD);
}

void FrontPlane(){
  #ifdef DEBUG
     Serial.println("FrontPlane");
  #endif
  server.send(200, "text/html", Menu_Celica_Gauge);
  server.sendHeader("Location","/"); 
  BackPlane_RPM(NON_COLOR, NON_COLOR, NON_COLOR);
  FrontPlane_RPM(0, 0, BLUE_COLOR);
  BackPlane_Speed(NON_COLOR, NON_COLOR, NON_COLOR);
  FrontPlane_Speed(0, 0, BLUE_COLOR);
  Display_LCD(0, 0, BLUE_COLOR);
}

void BackPlane(){
  #ifdef DEBUG
     Serial.println("BackPlane");
  #endif
  server.send(200, "text/html", Menu_Celica_Gauge);
  server.sendHeader("Location","/"); 
  BackPlane_RPM(RED_COLOR, GREEN_COLOR, BLUE_COLOR);
  FrontPlane_RPM(NON_COLOR, NON_COLOR, NON_COLOR);
  BackPlane_Speed(RED_COLOR, GREEN_COLOR, BLUE_COLOR);
  FrontPlane_Speed(NON_COLOR, NON_COLOR, NON_COLOR);
  Display_LCD(RED_COLOR, GREEN_COLOR, BLUE_COLOR);
}
void BrightnessDeafult(){
  #ifdef DEBUG
     Serial.println("BrightnessDeafult");
  #endif
  server.send(200, "text/html", Menu_Celica_Gauge);
  brightenss_FrontPlane = BRIGHTNESS_BACKPLANE;
}
void BrightnessBrighter(){
  #ifdef DEBUG
     Serial.println("BrightnessBrighter");
  #endif
  server.send(200, "text/html", Menu_Celica_Gauge);
  brightenss_FrontPlane =- 50; //TODO sciemnic 
}
//handler

//FUNCTIONS FOR LED'S
/**TODO napisac funkcje z parametrami i zmniejszyc ilosc kodu i zmienić delay na millis()**/
int BackPlane_RPM(int FirstColor, int SecondColor, int ThirdColor){
  for(daily_gauge_loop = 0; daily_gauge_loop < backplane_rpm.numPixels(); daily_gauge_loop++){
    #ifdef DEBUG
      Serial.print("Backplane_rpm = ");
      Serial.println(daily_gauge_loop);
    #endif
    backplane_rpm.setPixelColor(daily_gauge_loop, FirstColor, SecondColor, ThirdColor); // Dioda "i" oraz skladowe R=255 G=0 B=0
    backplane_rpm.show(); // Wysylamy dane do lancucha
    delay(TIME_WAITING);
  }
}
int FrontPlane_RPM(int FirstColor, int SecondColor, int ThirdColor){
  for(daily_gauge_loop = frontplane_rpm.numPixels(); daily_gauge_loop >= 0 ; daily_gauge_loop--){
    #ifdef DEBUG
      Serial.print("Frontplane_rpm = ");
      Serial.println(daily_gauge_loop);
    #endif
    frontplane_rpm.setPixelColor(daily_gauge_loop, FirstColor, SecondColor, ThirdColor); // Dioda "i" oraz skladowe R=255 G=0 B=0
    frontplane_rpm.show(); // Wysylamy dane do lancucha
    delay(TIME_WAITING);
  }
}
int BackPlane_Speed(int FirstColor, int SecondColor, int ThirdColor){
  for(daily_gauge_loop = 0; daily_gauge_loop < backplane_speed.numPixels(); daily_gauge_loop++){
    #ifdef DEBUG
      Serial.print("Backplane_Speed = ");
      Serial.println(daily_gauge_loop);
    #endif
    backplane_speed.setPixelColor(daily_gauge_loop, FirstColor, SecondColor, ThirdColor); // Dioda "i" oraz skladowe R=255 G=0 B=0
    backplane_speed.show(); // Wysylamy dane do lancucha
    delay(TIME_WAITING);
  }
}
int FrontPlane_Speed(int FirstColor, int SecondColor, int ThirdColor){
  for(daily_gauge_loop = frontplane_speed.numPixels(); daily_gauge_loop >= 0 ; daily_gauge_loop--){
    #ifdef DEBUG
      Serial.print("Frontplane_speed = ");
      Serial.println(daily_gauge_loop);
    #endif
    frontplane_speed.setPixelColor(daily_gauge_loop, FirstColor, SecondColor, ThirdColor); // Dioda "i" oraz skladowe R=255 G=0 B=0
    frontplane_speed.show(); // Wysylamy dane do lancucha
    delay(TIME_WAITING);
  }
}
int Display_LCD(int FirstColor, int SecondColor, int ThirdColor){
  for(daily_gauge_loop = 0; daily_gauge_loop < display_lcd.numPixels(); daily_gauge_loop++){
    #ifdef DEBUG
      Serial.print("Display = ");
      Serial.println(daily_gauge_loop);
    #endif
    display_lcd.setPixelColor(daily_gauge_loop, FirstColor, SecondColor, ThirdColor); // Dioda "i" oraz skladowe R=255 G=0 B=0
    display_lcd.show(); // Wysylamy dane do lancucha
    delay(TIME_WAITING);
  }
}
//FUNCTIONS FOR LED'S

void handleNotFound(){
  server.send(404, "text/plain", "404: Not found"); // Send HTTP status 404 (Not Found) when there's no handler for the URI in the request
}
