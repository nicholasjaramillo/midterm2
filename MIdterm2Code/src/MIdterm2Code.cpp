/* 
 * Project Midterm 2
 * Author: Nicholas Jaramillo
 * Date: 3/19/2026
 * For comprehensive documentation and examples, please visit:
 * https://docs.particle.io/firmware/best-practices/firmware-template/
 */

// Include Particle Device OS APIs
#include "Particle.h"
#include "Adafruit_SSD1306.h"
#include "Adafruit_GFX.h"
#include "credentials.h"
#include "Adafruit_BME280.h"
#include "Adafruit_MQTT/Adafruit_MQTT_SPARK.h"
#include "Adafruit_MQTT/Adafruit_MQTT.h"
#include <Adafruit_MQTT.h>

SYSTEM_MODE(AUTOMATIC);

String dateTime, timeOnly;
unsigned int lastTime;
const int OLED_RESET=-1;
const int relay = D15;
const int HEXADDRESS = 0x76;
int moistureLevel = analogRead(A1);
bool status;
int dust = D16;
unsigned long duration;
unsigned long starttime;
unsigned long sampletime_ms = 30000;
unsigned long lowpulseoccupancy = 0;
float ratio = 0;
float concentration = 0;
float tempC;
float humidRH;
void MQTT_connect ();
bool MQTT_ping ();
float  pubValue;
const char *EVENT_NAME = "water";
void waterTime(int moistureLevel, int relay);
void grabTemp(float tempC,  float humidRH);
void grabTime(String dateTime, String timeOnly);
void grabDust(int dust, unsigned long starttime, unsigned long sampletime_ms, unsigned long duration, float concentration, unsigned long lowpulseoccupancy);
void grabMoisture(int moistureLevel);
Adafruit_SSD1306 display(OLED_RESET);
Adafruit_BME280 bme; 
TCPClient TheClient; 
Adafruit_MQTT_SPARK mqtt(&TheClient,AIO_SERVER,AIO_SERVERPORT,AIO_USERNAME,AIO_KEY); 
Adafruit_MQTT_Publish pubFeed = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/temp");
Adafruit_MQTT_Publish pubFeedd = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/dust");
Adafruit_MQTT_Publish pubFeeddd = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/moister");


void setup() {
Serial.begin (9600);
waitFor( Serial.isConnected , 15000) ;
pinMode (relay, OUTPUT);
WiFi.connect(); 
Time.zone(-6);
Particle.syncTime();
Serial.begin(9600);
waitFor(Serial.isConnected,15000);
pinMode(dust,INPUT);
starttime = millis();
Time.zone(-6);
Particle.syncTime();
Serial.begin(9600);
waitFor(Serial.isConnected,15000);
display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
display.clearDisplay();
status = bme.begin(HEXADDRESS);
if(status == false) {
Serial.printf("BME280 at address 0x%02X failed to start ",HEXADDRESS);
}
}

void loop() {
waterTime(moistureLevel, relay);
grabMoisture(moistureLevel);
grabDust(dust, starttime, sampletime_ms, duration, concentration, lowpulseoccupancy);
grabTime(dateTime, timeOnly);
grabTemp(tempC, humidRH);
}



// auto water function
void waterTime(int moistureLevel, int relay){
moistureLevel = analogRead(A1);
static unsigned int longTime;
if ((millis()-longTime) >6000){
if (moistureLevel>2700){
digitalWrite(relay, HIGH);
delay(500);
digitalWrite(relay, LOW);
} 
longTime = millis();
}
}

// get temputure
void grabTemp(float tempC,  float humidRH){
tempC = bme.readTemperature();
humidRH = bme.readHumidity();
if( mqtt.Update() ){
pubFeed.publish (tempC);
display.clearDisplay();
display.setTextSize(2);
display.setTextColor(WHITE);
display.setCursor(0,25);
display.printf("temperture %f\n",tempC);
display.clearDisplay();
display.printf("humidity %f\n",humidRH);
display.display();
delay(2000);
}
}

// get time
void grabTime(String dateTime, String timeOnly){
dateTime = Time.timeStr();
timeOnly=dateTime.substring(11 ,19);
display.clearDisplay();
display.setTextSize(2);
display.setTextColor(WHITE);
display.setCursor(0,25);
display.printf("time\n %s\n",timeOnly.c_str());
display.display();
delay(2000);

if(concentration > 0.7){
display.clearDisplay();
display.setTextSize(2);
display.setTextColor(WHITE);
display.setCursor(0,25);
display.printf("!DUSTY!\n ");
display.display();
delay(2000);
}
}
// get dust reading
void grabDust(int dust, unsigned long starttime, unsigned long sampletime_ms, unsigned long duration, float concentration, unsigned long lowpulseoccupancy){
duration = pulseIn(dust, LOW);
lowpulseoccupancy = lowpulseoccupancy+duration;
if ((millis()-starttime) > sampletime_ms){
concentration = 1.1*pow(ratio,3)-3.8*pow(ratio,2)+520*ratio+0.62;
display.clearDisplay();
display.setTextSize(2);
display.setTextColor(WHITE);
display.setCursor(0,25);
display.printf("dust\n %f ",concentration);
display.display();
lowpulseoccupancy = 0;
pubFeedd.publish (concentration);
delay(2000);
}
if(concentration > 0.7){
display.clearDisplay();
display.setTextSize(2);
display.setTextColor(WHITE);
display.setCursor(0,25);
display.printf("!DUSTY!\n ");
display.display();
delay(2000);
}
}
// display moisture
void grabMoisture(int moistureLevel){
moistureLevel = analogRead(A1);
pubFeeddd.publish (moistureLevel);
display.clearDisplay();
display.setTextSize(2);
display.setTextColor(WHITE);
display.setCursor(0,20);
display.printf("moisture\n %i\n ", moistureLevel);
display.display();
delay(500);
}
