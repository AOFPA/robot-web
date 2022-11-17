// 203.158.177.150
// port: 1883
// coeiot:ComputerEng@2021
//mqtt
#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <ArduinoJson.h>
// const char* ssid = "espTest";
// const char* password = "xyz12345";
const char* ssid = "Ei";
const char* password = "h5nk8vny";

// MQTT Broker IP address:
const char* mqtt_server = "203.158.177.150";
const char* mqtt_user = "coeiot";
const char* mqtt_pwd = "ComputerEng@2021";

WiFiClient espClient;
PubSubClient client(espClient);

String table1, table2;

unsigned long previousMillis = 0;
unsigned long interval = 30000;

//json
StaticJsonDocument<200> doc;
char buffer[256];


//-------------<Moter Definning>-------------
//---Left Moter
#define LmoterRed 13
#define LmoterBlack 12

//---Right Moter
#define RmoterRed 14
#define RmoterBlack 27
//-------------</ moter definning>-------------
//-------------<IR Sensor Definning>-------------
#define LS 25
#define RS 26
//-------------</ ir sensor definning>-------------
//-------------------End Ultra Sonic Font ----------------------
const int trigPin = 32;
const int echoPin = 33;

//define sound speed in cm/uS
#define SOUND_SPEED 0.034
#define CM_TO_INCH 0.393701

long duration;
float distanceCm;
float distanceInch;

//------------ ตัวที่ 2 อยู่ด้านขวา กำหนดขา --------------------------
const int trigPin2 = 4;
const int echoPin2 = 36;
long duration2;
float distanceCm2;
float distanceInch2;
//---------------------------------------------------------
//-------------------End Ultra Sonic Font ----------------------
#define RXD2 16
#define TXD2 17
#include <HardwareSerial.h>
HardwareSerial SerialPort(2);  // use UART2
//-----------------End QR CODE Scanner -------------
// define variable serial
char SerailReadMain;
String SerialRead;
String CheckSerial;
char stringArray[100];
unsigned long timers;
//define control speed
const int freq = 30000;
const int pwmChannel = 0;
const int resolution = 8;
int dutyCycle = 190;
int enableA = 0;
int enableB = 2;
//bool stateQr = false;


//sw pin
#define SW_PIN 39
void setup() {
  pinMode(LmoterRed, OUTPUT);
  pinMode(LmoterBlack, OUTPUT);
  pinMode(RmoterRed, OUTPUT);
  pinMode(RmoterBlack, OUTPUT);
  pinMode(trigPin, OUTPUT);   // Sets the trigPin as an Output
  pinMode(echoPin, INPUT);    // Sets the echoPin as an Inpu
  pinMode(trigPin2, OUTPUT);  // Sets the trigPin as an Output
  pinMode(echoPin2, INPUT);   // Sets the echoPin as an Inpu
  pinMode(LS, INPUT);
  pinMode(RS, INPUT);

  //sw pinMode
  pinMode(SW_PIN, INPUT);

  Serial.begin(9600);
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);
  Serial.println("Serial Txd is on pin: " + String(TX));
  Serial.println("Serial Rxd is on pin: " + String(RX));
  //DEFINE CONTROLSPEED
  pinMode(enableA, OUTPUT);
  pinMode(enableB, OUTPUT);
  ledcSetup(pwmChannel, freq, resolution);
  ledcAttachPin(enableA, pwmChannel);
  ledcAttachPin(enableB, pwmChannel);
  setup_wifi();
  mqtt_init();
  doc["wifi_status"] = "online";
  doc["mqtt_status"] = "connected";
  serializeJson(doc, buffer);
  client.publish("status", buffer);
}

//---------------<function>---------------
void moveForward() {
  ledcWrite(pwmChannel, 174);
  ledcWrite(pwmChannel, 174);

  digitalWrite(LmoterRed, HIGH);
  digitalWrite(LmoterBlack, LOW);

  digitalWrite(RmoterRed, HIGH);
  digitalWrite(RmoterBlack, LOW);
  //  Serial.println("moveForward");
}

void robotStop() {
  digitalWrite(LmoterRed, LOW);
  digitalWrite(LmoterBlack, LOW);

  digitalWrite(RmoterRed, LOW);
  digitalWrite(RmoterBlack, LOW);
  //  Serial.println("robotStop");
}

void moveBackward() {
  ledcWrite(pwmChannel, 197);
  ledcWrite(pwmChannel, 197);
  digitalWrite(LmoterRed, LOW);
  digitalWrite(LmoterBlack, HIGH);

  digitalWrite(RmoterRed, LOW);
  digitalWrite(RmoterBlack, HIGH);
  //  Serial.println("moveBackward");
}

void turnLeft() {
  ledcWrite(pwmChannel, 200);
  ledcWrite(pwmChannel, 200);
  digitalWrite(LmoterRed, LOW);
  digitalWrite(LmoterBlack, LOW);

  digitalWrite(RmoterRed, HIGH);
  digitalWrite(RmoterBlack, LOW);


  //  Serial.println("turnRight");
}

void turnRight() {
  ledcWrite(pwmChannel, 200);
  ledcWrite(pwmChannel, 200);
  digitalWrite(LmoterRed, HIGH);
  digitalWrite(LmoterBlack, LOW);

  digitalWrite(RmoterRed, LOW);
  digitalWrite(RmoterBlack, LOW);
  //  Serial.println("turnLeft");
}
void turnLeftUntilFineLine() {
  //      เลี้ยวจนกว่าขวาจะเจอเส้นแล้วกลับไปทำงานตามปกติ
  if (digitalRead(RS) == 1) {
  }
}
//---------------</ function>---------------
// 1 คือ เจอสีดำ
void display() {
  if (millis() - timers >= 1000) {
    timers = millis();
    Serial.print("Distance Left : ");
    Serial.println(distanceCm);
    Serial.print("Distance Right : ");
    Serial.println(distanceCm2);
    if (distanceCm > 20.00 && distanceCm2 > 20.00) {
      if (!digitalRead(LS) && !digitalRead(RS)) {
        Serial.println("moveForward");
      }
      if (!digitalRead(LS) && digitalRead(RS)) {
        Serial.println("turnRight");
      }
      if (digitalRead(LS) && !digitalRead(RS)) {
        Serial.println("turnLeft");
      }
      if (digitalRead(LS) && digitalRead(RS)) {
        Serial.println("robotStop");
      }
    } else {
      Serial.println("robotStop");
    }
  }
}

void readUltrasonic() {
  //กำหนดอัตราโซนิคตัวที่ 1 (ด้านซ้าย)
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distanceCm = duration * SOUND_SPEED / 2;
  distanceInch = distanceCm * CM_TO_INCH;
  //  ------------------------------------------
  //กำหนดอัลตาโซนิคตัวที่ 2 (ด้านขวา)
  digitalWrite(trigPin2, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin2, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin2, LOW);
  duration2 = pulseIn(echoPin2, HIGH);
  distanceCm2 = duration2 * SOUND_SPEED / 2;
  distanceInch2 = distanceCm2 * CM_TO_INCH;
}
int order1 = 99;
int order2 = 99;
int stateTurn = false;

bool readingQRcode = false;
String QrCodeValue = "";
int currentOrder = 99;
bool sw_state;

//state turn right
bool wasGoMain = false;
//state in main
bool inMain = false;

//state turn right
bool wasTurnRight = false;
bool wasTurnLeft = false;
//state เดินผ่านแยกยัง
bool througLine = false;
bool manual = false;


//นับ
int countRead = 0;
int countRead34 = 0;

bool inStart = false;


bool afterSend34 = false;

//btn online
bool btn = false;
//------------------------------------------------LOOP-----------------------------------------------
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
  if (order1 != 99 && order2 != 99 && currentOrder != 99 && manual == false) {
    manual = false;
    Serial.println("robot running");
    sw_state = digitalRead(SW_PIN);
    Serial.println("sw state->" + String(sw_state));
    //เมื่อกด sw
    if (sw_state == 1 || btn == true) {
      delay(100);
      btn = false;
      if (currentOrder == 1) {
        delay(500);
        moveBackward();
        delay(1000);
        turnRight();
        delay(3000);
        moveBackward();
        delay(800);
        // QrCodeValue = "R";
        wasTurnRight = true;
        currentOrder = 2;
        // order1 = 0;
        //state go main
        // wasGoMain = true;
      } else if (currentOrder == 2) {
        delay(500);
        moveBackward();
        delay(1000);
        turnRight();
        delay(3000);
        moveBackward();
        delay(800);

        wasTurnRight = true;
        currentOrder = 0;
      }
    }
    readUltrasonic();
    //    stateQr = false;
    if (Serial2.available()) {
      readingQRcode = true;
      moveForward();
      delay(100);
      // readedQrcode = false;
      Serial.println("Serial do work!!");

      SerialRead = Serial2.readString();
      stringArray[SerialRead.length() + 1];
      SerialRead.toCharArray(stringArray, SerialRead.length() + 1);
      Serial.println("[0]->" + (String)stringArray[0]);
      Serial.println("[1]->" + (String)stringArray[1]);

      if ((String)stringArray[0] == "1" && (String)stringArray[1] == "2") {
        countRead++;
        doc["countRead12"] = (String)countRead;
        serializeJson(doc, buffer);
        client.publish("status", buffer);


        if (countRead == 2) {
          readingQRcode = false;
        }
      }

      if ((String)stringArray[0] == "3" && (String)stringArray[1] == "4") {
        countRead34++;
        doc["countRead34"] = (String)countRead34;
        serializeJson(doc, buffer);
        client.publish("status", buffer);


        if (countRead34 == 2) {
          // readingQRcode = false;
          if (order2 == 1) {
            QrCodeValue = "R";
          } else if (order2 == 2) {
            QrCodeValue = "L";
          } else {
            QrCodeValue = "F";
          }
          readingQRcode = false;
          afterSend34 = true;
        }
      }

      if (order1 != 0) {
        currentOrder = 1;
        readingQRcode = false;
        if ((String)order1 == (String)stringArray[0]) {
          QrCodeValue = "L";
          Serial.println("L");
        } else if ((String)order1 == (String)stringArray[1]) {
          QrCodeValue = "R";
          Serial.println("R");
        } else {
          QrCodeValue = "F";
          Serial.println("F");
        }
      } else if (order2 != 0 && order1 == 0 && afterSend34 == false) {
        // currentOrder = 2;
        readingQRcode = false;
        if ((String)order2 == (String)stringArray[0]) {
          QrCodeValue = "L";
          Serial.println("L");
        } else if ((String)order2 == (String)stringArray[1]) {
          QrCodeValue = "R";
          Serial.println("R");
        } else {
          QrCodeValue = "F";
          Serial.println("F");
        }
      }
    }
    //แสดงผล millis function
    // display();
    //ถ้าระยะทางเจออยู่ไกลอีกก็ไม่เป็นไร
    if (distanceCm > 20.00 && distanceCm2 > 20.00) {
      if (!digitalRead(LS) && !digitalRead(RS)) {
        if (wasTurnRight == true) {
          turnRight();
        } else if (wasTurnLeft == true) {
          turnLeft();
        } else {

          if (inStart == true) {
            robotStop();
            turnLeft();
            delay(600);
            robotStop();
            inStart = false;
            order1 = 99;
            order2 = 99;
            currentOrder = 99;
            countRead = 0;
            QrCodeValue = "";
            manual = false;
            througLine = false;
            afterSend34 = false;
          } else {
            moveForward();
          }
        }
        // wasTurnRight = false ;
      }
      if (!digitalRead(LS) && digitalRead(RS)) {

        turnRight();
        if (wasTurnRight == true) {
          wasTurnRight = false;
        }
      }
      if (digitalRead(LS) && !digitalRead(RS)) {
        turnLeft();
        if (wasTurnLeft == true) {
          wasTurnLeft = false;
        }
      }
      if (digitalRead(LS) && digitalRead(RS) && (readingQRcode == false)) {
        if (countRead == 2) {
          doc["inIf"] = "in";
          serializeJson(doc, buffer);
          client.publish("status", buffer);
          robotStop();
          // delay(5000);
          delay(500);
          moveBackward();
          delay(1000);
          turnRight();
          delay(3000);
          moveBackward();
          delay(800);
          wasTurnRight = true;
          countRead = 0;
          inStart = true;
        } else {
          if (QrCodeValue == "R") {
            turnRight();
            delay(1000);
            QrCodeValue = "";
            wasTurnRight = true;
          } else if (QrCodeValue == "L") {
            turnLeft();
            delay(1000);
            QrCodeValue = "";
            wasTurnLeft = true;
          } else if (QrCodeValue == "F") {
            moveForward();
            delay(250);
            QrCodeValue = "";
            througLine = false;

          } else {
            robotStop();
            //ไปส่ง order1 เสร็จแล้ว
            if (currentOrder == 2 && througLine == false) {
              if (order1 > 2 && order2 > 2 && currentOrder == 2) {
                QrCodeValue = "F";
                througLine = true;
                order1 = 0;
              } else if (order1 > 0 && order2 > 0 && order1 < 3 && order2 < 3 && currentOrder == 2) {
                QrCodeValue = "F";
                througLine = true;
                order1 = 0;
              } else if (order1 == 1 && order2 > 2 && currentOrder == 2) {
                QrCodeValue = "L";
                order1 = 0;
              } else if (order1 == 2 && order2 > 2 && currentOrder == 2) {
                QrCodeValue = "R";
                order1 = 0;
              } else if ((order1 == 1 || order1 == 3) && order2 == 0 && currentOrder == 2) {
                QrCodeValue = "R";
                througLine = true;
                order1 = 0;
                order2 = 0;
                currentOrder = 0;
              } else if ((order1 == 2 || order1 == 4) && order2 == 0 && currentOrder == 2) {
                QrCodeValue = "L";
                througLine = true;
                order1 = 0;
                order2 = 0;
                currentOrder = 0;
              } else if (order1 > 2 && order2 < 3 && order2 > 0 && currentOrder == 2) {
                if (order1 == 3) {
                  QrCodeValue = "R";
                } else if (order1 == 4) {
                  QrCodeValue = "L";
                }
                order1 = 0;
              }
              //กดสวิตช์ที่โต๊ะ order2
            } else if (currentOrder == 0 && througLine == false) {
              if (order2 == 3) {
                QrCodeValue = "R";
                througLine = true;
                order2 = 0;
              } else if (order2 == 4) {
                QrCodeValue = "L";
                througLine = true;
                order2 = 0;
              } else if (order2 == 1) {
                QrCodeValue = "R";
                througLine = true;
                order2 = 0;
              } else if (order2 == 2) {
                QrCodeValue = "L";
                througLine = true;
                order2 = 0;
              }
            }
          }
        }
      }
    } else {
      robotStop();
    }
  } else {
  }
}

void mqtt_init() {
  client.setServer(mqtt_server, 1883);
  Serial.println("mqtt connecting");
  if (client.connect(mqtt_server, mqtt_user, mqtt_pwd)) {
    Serial.println("mqtt connected.");
    client.publish("/nodejs/mqtt", "hello world");
    client.subscribe("table");
    client.subscribe("dir");
    client.subscribe("mode");
    client.setCallback(callback);
  } else {
    Serial.println("mqtt not connected");
    Serial.println("try connec agian.");
    mqtt_init();
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
    if (strcmp(topic, "dir") == 0) {
      // whatever you want for this topic
      const char* dir = dataPayload["dir"];
      Serial.print("dir from nodejs : ");
      Serial.println(dir);
      if (strcmp(dir, "forward") == 0) {
        moveForward();
        Serial.println("move forward");

      } else if (strcmp(dir, "left") == 0) {
        turnLeft();
        Serial.println("left");

      } else if (strcmp(dir, "right") == 0) {
        turnRight();
        Serial.println("right");

      } else if (strcmp(dir, "stop") == 0) {
        robotStop();
        Serial.println("stop");

      } else if (strcmp(dir, "backward") == 0) {
        moveBackward();
        Serial.println("moveBackward");
      }
    } else if (strcmp(topic, "table") == 0) {
      const char* pl_order1 = dataPayload["order1"];
      const char* pl_order2 = dataPayload["order2"];
      String order1_txt = String(pl_order1);
      String order2_txt = String(pl_order2);
      Serial.println(order1_txt);
      Serial.println(order2_txt);
      order1 = order1_txt.toInt();
      order2 = order2_txt.toInt();
      currentOrder = 1;
    } else if (strcmp(topic, "mode") == 0) {
      const char* pl_mode = dataPayload["mode"];
      String mode_txt = String(pl_mode);
      Serial.println(mode_txt);
      if (mode_txt == "m") {
        manual = true;
      } else {
        manual = false;
      }

      if (mode_txt == "b") {
        btn = true;
      }
      if (mode_txt == "r") {
        inStart = false;
        order1 = 99;
        order2 = 99;
        currentOrder = 99;
        countRead = 0;
        QrCodeValue = "";
        manual = false;
        througLine = false;
        afterSend34 = false;
      }
    }

  } else {
    Serial.println(F("Payload is null"));
    return;
  }  //check payload is NULL
}