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


SYSTEM_MODE(AUTOMATIC);

String dateTime, timeOnly;
unsigned int lastTime;
const int OLED_RESET=-1;
const int relay = D15;
int moistureLevel;
int dust = D16;
unsigned long duration;
unsigned long starttime;
unsigned long sampletime_ms = 30000;
unsigned long lowpulseoccupancy = 0;
float ratio = 0;
float concentration = 0;
const char *EVENT_NAME = "water";
void waterTime(int moistureLevel, int relay);
Adafruit_SSD1306 display(OLED_RESET);


void setup() {
pinMode (relay, OUTPUT);
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

}


void loop() {
// autowater
moistureLevel = analogRead(A1);
waterTime(moistureLevel, relay);

// display moisture level 
if( millis() - lastTime >10000) {
display.clearDisplay();
display.setTextSize(2);
display.setTextColor(WHITE);
display.setCursor(0,20);
display.printf("moisture\n %i\n ", moistureLevel);
display.display();
lastTime = millis();
delay(500);
}

// display dust
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
delay(2000);

// display time
dateTime = Time.timeStr();
timeOnly=dateTime.substring(11 ,19);
if ((millis()-starttime) > sampletime_ms){
display.clearDisplay();
display.setTextSize(2);
display.setTextColor(WHITE);
display.setCursor(0,25);
display.printf("time\n %s\n",timeOnly.c_str());
display.display();
delay(2000);
starttime = millis();
// if (concentration >)
}
}
}



// auto water function
void waterTime(int moistureLevel, int relay){
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