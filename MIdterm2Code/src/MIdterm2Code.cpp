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


SYSTEM_MODE(AUTOMATIC);

String dateTime, timeOnly;
unsigned int lastTime;
const int OLED_RESET=-1;
const int relay = D15;
int reads;
int pin = D16;
unsigned long duration;
unsigned long starttime;
unsigned long sampletime_ms = 30000;
unsigned long lowpulseoccupancy = 0;
float ratio = 0;
float concentration = 0;
const char *EVENT_NAME = "water";
void subscriptionHandler(const char *event, const char *data);
Adafruit_SSD1306 display(OLED_RESET);



void setup() {
pinMode (relay, OUTPUT);
Time.zone(-6);
Particle.syncTime();
Serial.begin(9600);
waitFor(Serial.isConnected,15000);
pinMode(pin,INPUT);
starttime = millis();
// String subscriptionName = String::format("%s/%s/", System.deviceID().c_str(), EVENT_NAME);
// Particle.subscribe(subscriptionName, subscriptionHandler, MY_DEVICES);
// Serial.printf("Subscribing to %s\n", subscriptionName.c_str());
}


void loop() {
reads = analogRead(A1);
dateTime = Time.timeStr();
timeOnly=dateTime.substring(11 ,19);
if( millis () - lastTime >10000) {
lastTime = millis();
display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
display.clearDisplay();
display.setTextSize(2);
display.setTextColor(WHITE);
display.setCursor(0,25);
display.printf("%i\n %s\n", reads, timeOnly.c_str());
display.display();
delay(100);
digitalWrite(relay,HIGH);
delay(500);
digitalWrite(relay, LOW);
delay (2000);
}

duration = pulseIn(pin, LOW);
lowpulseoccupancy = lowpulseoccupancy+duration;
if ((millis()-starttime) > sampletime_ms){
ratio = lowpulseoccupancy/(sampletime_ms*10.0);
concentration = 1.1*pow(ratio,3)-3.8*pow(ratio,2)+520*ratio+0.62;
Serial.printf("%li\n%f\n concentration %f\n",lowpulseoccupancy, ratio, concentration);
lowpulseoccupancy = 0;
starttime = millis();
}
}