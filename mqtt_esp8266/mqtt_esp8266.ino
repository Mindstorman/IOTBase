#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>


#define REDPIN 4
#define GREENPIN 5
#define BLUEPIN 14
 
#define FADESPEED 5


// Update these with values suitable for your network.

const char* ssid = "NETWORK_SSID";
const char* password = "NETWORK_PASSWORD";
const char* mqtt_server = "MQTT_BROKER";
const char* mqttUser = "MQTT_USERNAME";
const char* mqttPassword = "MQTT_PASSWORD";
double storedBrightness = 1;
double storedRed = 255;
double storedBlue = 255;
double storedGreen = 255;

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  StaticJsonDocument<256> doc;
  deserializeJson(doc, payload, length);
  const char* world = doc["state"];
  int bright = doc["brightness"];
  double brightness = storedBrightness;
  int r = doc["color"]["r"];
  double red = storedRed;
  int g = doc["color"]["g"];
  double green = storedGreen;
  int b = doc["color"]["b"];
  double blue = storedBlue;
  Serial.print("Message arrived [");
  if ( (bright/255.0) != 0 )
  {
    storedBrightness = (bright/255.0);
    brightness = storedBrightness;
  }
  if ( r != 0 || g != 0 || b != 0)
  {
    red = (r/255.0);
    storedRed = (r/255.0);
    blue = (b/255.0);
    storedBlue = (b/255.0);
    green = (g/255.0);
    storedGreen = (g/255.0);
  }
  Serial.print("Red: ");
  Serial.println(brightness*red*1024);
  Serial.print("Blue: ");
  Serial.println(brightness*blue*1024);
  Serial.print("Green: ");
  Serial.println(brightness*green*1024);
  

  Serial.print(brightness);
  Serial.println("]");
  
  //analogWrite(5,1024);
  delay(5);

  char buffer[512];
  StaticJsonDocument<256> res;
  
  if (strcmp(world,"ON") == 0) {
    //digitalWrite(BUILTIN_LED, LOW);   // Turn the LED on (Note that LOW is the voltage level
    analogWrite(GREENPIN,brightness*green*1024);
    analogWrite(REDPIN,brightness*red*1024);
    analogWrite(BLUEPIN,brightness*blue*1024);
    res["state"] = "ON";
    serializeJson(res, buffer);
    client.publish("lights/status", buffer);
    // but actually the LED is on; this is because
    // it is active low on the ESP-01)
  } else {
    digitalWrite(BUILTIN_LED, HIGH);  // Turn the LED off by making the voltage HIGH
    analogWrite(GREENPIN,0);
    analogWrite(REDPIN,0);
    analogWrite(BLUEPIN,0);
    res["state"] = "OFF";
    serializeJson(res, buffer);
    client.publish("lights/status", buffer);
  }

}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str(), mqttUser, mqttPassword)) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("outTopic", "hello world");
      // ... and resubscribe
      client.subscribe("lights/set");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  //pinMode(GREENPIN, OUTPUT);
  //pinMode(BLUEPIN, OUTPUT);
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  long now = millis();
  if (now - lastMsg > 2000) {
    lastMsg = now;
    ++value;
    snprintf (msg, 50, "hello world #%ld", value);
    Serial.print("Publish message: ");
    Serial.println(msg);
    //client.publish("outTopic", msg);
  }
}
