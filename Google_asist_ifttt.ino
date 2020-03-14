#include <ESP8266WiFi.h>
#include "DHT.h"
#include <Servo.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#define door D1
#define fan D2
#define bulb D3
#define w_s D6
#define s_s D7
#define WLAN_SSID       "smarthome"             // Your SSID
#define WLAN_PASS       "smarthome123"        // Your password
#define DHTPIN D4
#define DHTTYPE DHT11

/************************* Adafruit.io Setup *********************************/

#define AIO_SERVER      "io.adafruit.com" //Adafruit Server
#define AIO_SERVERPORT  1883                   
#define AIO_USERNAME    "vishnupriya60"            // Username
#define AIO_KEY         "d553d34d84b646429533f8113c4f97db"   // Auth Key

DHT dht(DHTPIN, DHTTYPE);
//WIFI CLIENT
WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);
Servo myservo;

Adafruit_MQTT_Publish temp = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/temp");


Adafruit_MQTT_Subscribe Light1 = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME"/feeds/light"); 
Adafruit_MQTT_Subscribe door1 = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME"/feeds/door"); 
Adafruit_MQTT_Subscribe fan1 = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME"/feeds/fan");
void MQTT_connect();


void setup() 
{
  Serial.begin(115200);
  myservo.attach(door);
  myservo.write(90);
  pinMode(bulb, OUTPUT);
  pinMode(fan, OUTPUT);
  dht.begin();
  pinMode(w_s, OUTPUT);
  pinMode(s_s, OUTPUT);  
  digitalWrite(bulb,HIGH);
  // Connect to WiFi access point.
  Serial.println(); Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WLAN_SSID);

  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED)
 {
      delay(500);
     Serial.print(".");
              }
  Serial.println();
  Serial.println("WiFi connected");
  digitalWrite(w_s,HIGH);
  Serial.println("IP address: "); 
  Serial.println(WiFi.localIP()); 
  mqtt.subscribe(&Light1);
  mqtt.subscribe(&fan1);
  mqtt.subscribe(&door1);
  

}
void loop() 
{ 
  MQTT_connect();
  Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(20000))) 
{
    if (subscription == &Light1)
 {
      Serial.print(F("Got: light"));
      Serial.println((char *)Light1.lastread);
      int Light_State = atoi((char *)Light1.lastread);
      digitalWrite(bulb, Light_State);
     }  
     else if (subscription == &fan1)
     {
      Serial.print(F("Got:fan "));
      Serial.println((char *)fan1.lastread);
      int fan_State = atoi((char *)fan1.lastread);
      digitalWrite(fan, fan_State);
     }   
     else if (subscription == &door1)
     {
      Serial.print(F("Got:door "));
      Serial.println((char *)door1.lastread);
      int door_State = atoi((char *)door1.lastread);
      myservo.write(door_State);
     }    
           }
     float t = dht.readTemperature();
     if (! temp.publish(t)) {
      Serial.println(F("Failed"));
    } else {
      Serial.println(F("OK!"));
    }

}
void MQTT_connect()
 {
  int8_t ret;
  if (mqtt.connected()) 
{
    return;
                     }

  Serial.print("Connecting to MQTT... ");
  uint8_t retries = 3;  
  while ((ret = mqtt.connect()) != 0) 
{
    Serial.println(mqtt.connectErrorString(ret));
    Serial.println("Retrying MQTT connection in 5 seconds...");
    mqtt.disconnect();
    delay(5000); 
    retries--;
    if (retries == 0) 
    {
            while (1);
                }
  }
  Serial.println("MQTT Connected!");
  digitalWrite(s_s,HIGH);
}
