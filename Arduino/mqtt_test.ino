//203.158.177.150
//port: 1883
//coeiot:ComputerEng@2021

#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <ArduinoJson.h>

const char* ssid = "espTest";
const char* password = "xyz12345";

//MQTT Broker IP address:
const char* mqtt_server = "203.158.177.150";
const char* mqtt_user = "coeiot";
const char* mqtt_pwd = "ComputerEng@2021";

WiFiClient espClient;
PubSubClient client(espClient);

String table1, table2 ;

unsigned long previousMillis = 0;
unsigned long interval = 30000;

void setup() {
  Serial.begin(115200);
  setup_wifi();
  mqtt_init();
}


void loop() {
  unsigned long currentMillis = millis();
  // if WiFi is down, try reconnecting every CHECK_WIFI_TIME seconds
  if ((WiFi.status() != WL_CONNECTED) && (currentMillis - previousMillis >= interval)) {
    Serial.print(millis());
    Serial.println("Reconnecting to WiFi...");
    WiFi.disconnect();
    setup_wifi();
    mqtt_init();
    previousMillis = currentMillis;
  }
  client.loop();
}
void mqtt_init() {
  client.setServer(mqtt_server, 1883);
  if (client.connect(mqtt_server, mqtt_user, mqtt_pwd)) {
    Serial.println("mqtt connected.");
    client.publish("/nodejs/mqtt", "hello world");
    client.subscribe("table");
    client.setCallback(callback);
  }
}

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

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  String strTopic = topic;
  payload[length] = '\0';
  String strPayload = (char*)payload;
  String strStationId;
  Serial.println("[" + strTopic + "] " + strPayload);
  StaticJsonDocument<200> dataPayload;

  if (strPayload != "") {
    DeserializationError error = deserializeJson(dataPayload, strPayload);
    if (error) {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.c_str());
      return;
    }
    const char* order1 = dataPayload["order1"];
    const char* order2 = dataPayload["order2"];
    String order1_txt = String(order1);
    String order2_txt = String(order2);
    Serial.println(order1_txt);
    Serial.println(order2_txt);
  } else {
    Serial.println(F("Payload is null"));
    return;
  } //check payload is NULL
}
