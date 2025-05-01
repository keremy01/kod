#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

const char* ssid = "Kerem";
const char* password = "12345678";
const char* mqtt_server = "opkg.pievision.com";

const char* mqtt_sub_topic = "Keremsensor";
const char* mqtt_pub_topic = "Keremveri";

WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) delay(500);
}

void callback(char* topic, byte* payload, unsigned int length) {
  char msg[length + 1];
  memcpy(msg, payload, length);
  msg[length] = '\0';

  StaticJsonDocument<512> doc;
  if (deserializeJson(doc, msg)) return;

  if (doc["temperature"]) {
    float t = doc["temperature"];
    Serial.print("Sıcaklık: ");
    Serial.print(t);
    Serial.println(" °C");
  }

  if (doc["humidity"]) {
    int h = doc["humidity"];
    Serial.print("Nem: ");
    Serial.print(h);
    Serial.println(" %");
  }

  if (doc["doorOpen"]) {
    bool door = doc["doorOpen"];
    Serial.print("Kapı: ");
    Serial.println(door ? "Açık" : "Kapalı");
  }

  if (doc["timestamp"]) {
    const char* ts = doc["timestamp"];
    Serial.print("Zaman: ");
    Serial.println(ts);
  }

  if (doc["location"]) {
    float lat = doc["location"]["lat"];
    float lon = doc["location"]["lon"];
    Serial.print("Konum: ");
    Serial.print(lat, 6);
    Serial.print(", ");
    Serial.println(lon, 6);
  }

  if (doc["temperatureLog"]) {
    JsonArray log = doc["temperatureLog"];
    Serial.println("Geçmiş sıcaklıklar:");
    for (float t : log) {
      Serial.print(" - ");
      Serial.print(t);
      Serial.println(" °C");
    }
  }

  StaticJsonDocument<128> outDoc;
  outDoc["status"] = "Veri işlendi";
  outDoc["time"] = millis();
  char buffer[128];
  serializeJson(outDoc, buffer);
  client.publish(mqtt_pub_topic, buffer);
}

void reconnect() {
  while (!client.connected()) {
    if (client.connect("arduinoClient")) {
      client.subscribe(mqtt_sub_topic);
    } else {
      delay(2000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  
  if (!client.connected()) {
    reconnect();
  }
}

void loop() {
  if (!client.connected()) reconnect();
  client.loop();
}
