#include <WiFi.h>
#include <PubSubClient.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

#define MY_DHT_PIN 15
#define DHT_TYPE DHT22
#define MSG_BUFFER_SIZE  (50)

#define MQTT_USERNAME ""
#define MQTT_PASSWORD ""
const char MQTT_CLIENTID[] = "";

const char* ssid = "";
const char* password = "";
const char* mqtt_server = "io.adafruit.com";

const char currentPriceTopic[] = MQTT_USERNAME "/feeds/current-price";
const char gateStateTopic[] = MQTT_USERNAME "/feeds/gate-state";
const char guestsTopic[] = MQTT_USERNAME "/feeds/guests";
const char membersTopic[] = MQTT_USERNAME "/feeds/members";
const char revenueTopic[] = MQTT_USERNAME "/feeds/revenue";
const char totalCarsTopic[] = MQTT_USERNAME "/feeds/total-cars";
const char pullStatusTopic[] = MQTT_USERNAME "/feeds/status";
const char fireTopic[] = MQTT_USERNAME "/feeds/fire";
const char openTopic[] = MQTT_USERNAME "/feeds/open";


//class MQTT_inst {

DHT_Unified dht(MY_DHT_PIN, DHT_TYPE);
WiFiClient espClient;
PubSubClient client(espClient);
char itoa_buf[12];
char msg[MSG_BUFFER_SIZE];

unsigned long lastMsgTime = 0;
String tempStr;
int currentPrice = 5;
String gateState = "close";
int guests = 0;
int members = 0;
int revenue = 0;
int totalCars = 3;
int fireAlert = 0;
int openVal = 0;
boolean failedData = false;

void mqtt_setup_wifi() {
  int counterTry = 0;
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    counterTry++;
    if (counterTry > 10) {
      Serial.println("Could not connect to WiFi, retrying...");
      counterTry = 0;
      mqtt_setup_wifi();
    }
  }
  randomSeed(micros());
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}


void mqtt_setCurrentPrice(int price) {
  currentPrice = price;
}


int mqtt_getCurrentPrice() {
  return currentPrice;
}

String mqtt_getGateState() {
  return gateState;
}

int mqtt_getGuests() {
  return guests;
}

void mqtt_setGuests(int givenGuests) {
  guests = givenGuests;
}

int mqtt_getMembers() {
  return members;
}

void mqtt_setMembers(int givenMembers) {
  members = givenMembers;
}

int mqtt_getRevenue() {
  return revenue;
}

int mqtt_getTotalCars() {
  return totalCars;
}

void mqtt_setTotalCars(int cars) {
  totalCars = cars;
}

int mqtt_getFireAlert() {
  return fireAlert;
}

void mqtt_setFireAlert(int fire) {
  fireAlert = fire;
}

int mqtt_getOpen() {
  return openVal;
}

void mqtt_setOpen(int gate) {
  openVal = gate;
}

void mqtt_incMembers() {
  members++;
  revenue += currentPrice / 4;
}

void mqtt_incGuests() {
  guests++;
  revenue += currentPrice;
}

void mqtt_callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  String value = "";
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
  // Only proceed if incoming message's topic matches
  if (String(topic) == currentPriceTopic) {
    for (int i = 0; i < length; i++) {
      value += (char)payload[i];
    }
    int price = value.toInt();
    mqtt_setCurrentPrice(price);
    Serial.println("CurrentPrice supposed to be updated");
    delay(1000);

  } else if (String(topic) == fireTopic) {
    // fire alert is 1 when pressed, 0 when released
    for (int i = 0; i < length; i++) {
      value += (char)payload[i];
    }
    mqtt_setFireAlert(value.toInt());
    Serial.println("FireAlert supposed to be updated");
    delay(1000);
    if (value.toInt() != 0 && value.toInt() != 1) {
      Serial.println("Invalid fire alert value received");
      delay(1000);
    }
    
    } else if (String(topic) == openTopic) {
    // fire alert is 1 when pressed, 0 when released
    for (int i = 0; i < length; i++) {
      value += (char)payload[i];
    }
    mqtt_setOpen(value.toInt());
    Serial.println("Open supposed to be updated");
    delay(1000);
  } else {
    Serial.println("No valid massage RECEIVED");
    delay(1000);
  }
}


void mqtt_reconnectMQTT() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);
    if (client.connect(MQTT_CLIENTID, MQTT_USERNAME, MQTT_PASSWORD)) {
      Serial.println("connected");
      client.subscribe(currentPriceTopic);
      client.subscribe(fireTopic);
      client.subscribe(openTopic);
    }
    else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}



void mqtt_init() {
  lastMsgTime = 0;
  tempStr = "";
  mqtt_setup_wifi();
  dht.begin();
  client.setServer(mqtt_server, 1883);
  client.setCallback(mqtt_callback);
  if (!client.connected()) {
    mqtt_reconnectMQTT();
  }
  client.publish(pullStatusTopic, "READY");
}

void mqtt_pushData() {
  if (!client.connected()) {
    mqtt_reconnectMQTT();
  }
  unsigned long now = millis();
  if (now - lastMsgTime >= 11000) { // Every 11 second
    lastMsgTime = now;
    Serial.println("Receiveing data...");
    client.loop(); // Here it is also supposed to (Automatically) call the callback function
    Serial.println("Publishing all data...");
    // tempStr = String(currentPrice);
    // client.publish(currentPriceTopic, tempStr.c_str());
    tempStr = String(gateState);
    client.publish(gateStateTopic, tempStr.c_str());
    tempStr = String(guests);
    client.publish(guestsTopic, tempStr.c_str());
    tempStr = String(members);
    client.publish(membersTopic, tempStr.c_str());
    tempStr = String(revenue);
    client.publish(revenueTopic, tempStr.c_str());
    tempStr = String(totalCars);
    client.publish(totalCarsTopic, tempStr.c_str());
  }

  if (failedData && currentPrice == -1) {
    Serial.println("ERROR could not read data, halting...");
    failedData = false;
    while (true) {
      delay(10000);
    }
  }

  if (currentPrice == -1) {
    Serial.println("Error reading data, retrying...");
    mqtt_pushData();
    failedData = true;
  }
}

void mqtt_resetData() {
  currentPrice = 5;
  gateState = "close";
  guests = 0;
  members = 0;
  revenue = 0;
  totalCars = 0;
}
