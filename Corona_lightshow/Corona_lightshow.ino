#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <WebSocketsClient.h> //  https://github.com/kakopappa/sinric/wiki/How-to-add-dependency-libraries
#include <ArduinoJson.h> // https://github.com/kakopappa/sinric/wiki/How-to-add-dependency-libraries (use the correct version)
#include <StreamString.h>
#include <ESP8266HTTPClient.h>

ESP8266WiFiMulti WiFiMulti;
WebSocketsClient webSocket;
WiFiClient client;
#define HEARTBEAT_INTERVAL 300000
uint64_t heartbeatTimestamp = 0;
bool isConnected = false;

// Fill all the following elements
// Get your Sinric API key at https://sinric.com
// You'll also need to install the Sinric skill on your Amazon Alexa and get a Hue API key
// Get yours at https://developers.meethue.com/develop/get-started-2/
// You'll need to create a developer account before accessing the ressources
#define MyApiKey "YOUR_SINRIC_API_KEY"
#define MySSID "SSID"
#define MyWifiPassword "PASSWORD"
#define DEVICE1 "DEVICE_ID"

//******************* Variables ********************
bool startLoopShow = false;

// Adjust the timers at your convenience 
const unsigned long delay1sec30 = 1500;
const unsigned long delay5sec = 5000;
const unsigned long delay8sec = 8000;
const unsigned long delay30sec = 30000;
const unsigned long delay2min30 = 150000;
unsigned long previousTimer = 0;
unsigned long previousTimer2 = 0;
unsigned long previousTimer3 = 0;
unsigned long previousTimer4 = 0;
unsigned long previousTimer5 = 0;
unsigned long currentTime = 0;
bool firstTimer = false;
bool secondTimer = false;
bool thirdTimer = false;
bool fourthTimer = false;
bool fifthTimer = false;


void turnAllLightsOn() {
  Serial.println("Turn lumineux dans toute la maison On");
  HTTPClient http;
  http.begin("http://IP_OF_YOUR_HUE_BRIDGE/api/YOUR_API_TOKEN/groups/YOUR_GROUP_NUMBER/action");
  http.addHeader("Content-Type", "application/json"); // Do not touch this
  char jsonPayload[1000];
  sprintf(jsonPayload, "{\"on\":true,\"bri\" :254,\"scene\":\"SCENE_ID\"}");
  int httpCode = http.PUT(jsonPayload);
  // You can uncomment the following lines if you need to see the API's request answer (for debugging for example)
  // I deactivated it to avoid "waiting" for a server's answer
  //  String payload = http.getString();
  //  Serial.println(httpCode);
  //  Serial.println(payload);
  http.end();
}

void turnAllLightsOnEnd() {
  Serial.println("Turn lumineux dans toute la maison end");
  HTTPClient http;
  http.begin("http://IP_OF_YOUR_HUE_BRIDGE/api/YOUR_API_TOKEN/groups/YOUR_GROUP_NUMBER/action");
  http.addHeader("Content-Type", "application/json");
  char jsonPayload[1000];
  sprintf(jsonPayload, "{\"on\":true,\"bri\" :254,\"transitiontime\":50,\"scene\":\"SCENE_ID\"}");
  int httpCode = http.PUT(jsonPayload);
  //  String payload = http.getString();
  //  Serial.println(httpCode);
  //  Serial.println(payload);
  http.end();
}

void turnAllLightsOff() {
  Serial.println("Turn all lights off");
  HTTPClient http;
  http.begin("http://IP_OF_YOUR_HUE_BRIDGE/api/YOUR_API_TOKEN/groups/YOUR_GROUP_NUMBER/action");
  http.addHeader("Content-Type", "application/json");
  char jsonPayload[1000];
  sprintf(jsonPayload, "{\"on\":false,\"transitiontime\":25}");
  int httpCode = http.PUT(jsonPayload);
  //  String payload = http.getString();
  //  Serial.println(httpCode);
  //  Serial.println(payload);
  http.end();
}

void turnAllLightsRed() {
  Serial.println("Turn all lights red");
  HTTPClient http;
  http.begin("http://IP_OF_YOUR_HUE_BRIDGE/api/YOUR_API_TOKEN/groups/YOUR_GROUP_NUMBER/action");
  http.addHeader("Content-Type", "application/json");
  char jsonPayload[1000];
  sprintf(jsonPayload, "{\"on\":true,\"bri\":254,\"scene\":\"SCENE_ID\"}");
  int httpCode = http.PUT(jsonPayload);
  //  String payload = http.getString();
  //  Serial.println(httpCode);
  //  Serial.println(payload);
  http.end();
}

void turnAllLightsWhite() {
  Serial.println("Turn all lights white");
  HTTPClient http;
  http.begin("http://IP_OF_YOUR_HUE_BRIDGE/api/YOUR_API_TOKEN/groups/YOUR_GROUP_NUMBER/action");
  http.addHeader("Content-Type", "application/json");
  char jsonPayload[1000];
  sprintf(jsonPayload, "{\"on\":true,\"bri\":254,\"scene\":\"SCENE_ID\"}");
  int httpCode = http.PUT(jsonPayload);
  //  String payload = http.getString();
  //  Serial.println(httpCode);
  //  Serial.println(payload);
  http.end();
}

void toggleStartLoopShow() {
  startLoopShow = !startLoopShow;
}

void startLightshow() {
  Serial.println("Start lightshow");
  turnAllLightsOn();
  previousTimer = millis();
  firstTimer = true;
}

void stopLightshow() {
  Serial.println("Stop lightshow");
  startLoopShow = false;
  turnAllLightsOff();
  previousTimer5 = millis();
  fifthTimer = true;
}

void turnOn(String deviceId) {
  if (deviceId == DEVICE1)
  {
    Serial.print("Turn on device id: ");
    Serial.println(deviceId);

    startLightshow();
  }
  else {
    Serial.print("Turn on for unknown device id: ");
    Serial.println(deviceId);
  }
}

void turnOff(String deviceId) {
  if (deviceId == DEVICE1)
  {
    Serial.print("Turn off Device ID: ");
    Serial.println(deviceId);

    stopLightshow();
  }
  else {
    Serial.print("Turn off for unknown device id: ");
    Serial.println(deviceId);
  }
}

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
  switch (type) {
    case WStype_DISCONNECTED:
      isConnected = false;
      Serial.printf("[WSc] Webservice disconnected from sinric.com!\n");
      break;
    case WStype_CONNECTED: {
        isConnected = true;
        Serial.printf("[WSc] Service connected to sinric.com at url: %s\n", payload);
        Serial.printf("Waiting for commands from sinric.com ...\n");
      }
      break;
    case WStype_TEXT: {
        Serial.printf("[WSc] get text: %s\n", payload);

#if ARDUINOJSON_VERSION_MAJOR == 5
        DynamicJsonBuffer jsonBuffer;
        JsonObject& json = jsonBuffer.parseObject((char*)payload);
#endif
#if ARDUINOJSON_VERSION_MAJOR == 6
        DynamicJsonDocument json(1024);
        deserializeJson(json, (char*) payload);
#endif
        String deviceId = json ["deviceId"];
        String action = json ["action"];

        if (action == "setPowerState") {
          String value = json ["value"];
          if (value == "ON") {
            turnOn(deviceId);
          } else {
            turnOff(deviceId);
          }
        }
        else if (action == "test") {
          Serial.println("[WSc] received test command from sinric.com");
        }
      }
      break;
    case WStype_BIN:
      Serial.printf("[WSc] get binary length: %u\n", length);
      break;
  }
}

void setup() {
  //Serial.begin(115200);  // Uncomment this line if you need serial
  WiFiMulti.addAP(MySSID, MyWifiPassword);
  Serial.println();
  Serial.print("Connecting to Wifi: ");
  Serial.println(MySSID);

  // Waiting for Wifi connect
  while (WiFiMulti.run() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  if (WiFiMulti.run() == WL_CONNECTED) {
    Serial.println("");
    Serial.print("WiFi connected. ");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  }

  // server address, port and URL
  webSocket.begin("iot.sinric.com", 80, "/");

  // event handler
  webSocket.onEvent(webSocketEvent);
  webSocket.setAuthorization("apikey", MyApiKey);

  // try again every 5000ms if connection has failed
  webSocket.setReconnectInterval(5000);   // If you see 'class WebSocketsClient' has no member named 'setReconnectInterval' error update arduinoWebSockets
}

void loop() {
  webSocket.loop();

  if (isConnected) {
    uint64_t now = millis();

    // Send heartbeat in order to avoid disconnections during ISP resetting IPs over night. Thanks @MacSass
    if ((now - heartbeatTimestamp) > HEARTBEAT_INTERVAL) {
      heartbeatTimestamp = now;
      webSocket.sendTXT("H");
    }
  }

  // TIMERS *******************************************
  if (startLoopShow) {
    if (thirdTimer) {
      currentTime = millis();
      if ((unsigned long)(currentTime - previousTimer3) >= delay1sec30) {
        Serial.println("third timer");
        turnAllLightsRed();
        thirdTimer = false;
        fourthTimer = true;
      }
      previousTimer4 = millis();
    }
    if (fourthTimer) {
      currentTime = millis();
      if ((unsigned long)(currentTime - previousTimer4) >= delay1sec30) {
        Serial.println("fourth timer");
        turnAllLightsWhite();
        fourthTimer = false;
        thirdTimer = true;
      }
      previousTimer3 = millis();
    }
  }

  if (firstTimer) {
    currentTime = millis();
    if ((unsigned long)(currentTime - previousTimer) >= delay2min30) {
      Serial.println("first timer");
      turnAllLightsOff();
      firstTimer = false;
      secondTimer = true;
    }
    previousTimer2 = millis();
  }
  if (secondTimer) {
    currentTime = millis();
    if ((unsigned long)(currentTime - previousTimer2) >= delay30sec) {
      Serial.println("second timer");
      toggleStartLoopShow();
      secondTimer = false;
      thirdTimer = true;
    }
    previousTimer3 = millis();
  }

  if (fifthTimer) {
    firstTimer = false;
    secondTimer = false;
    thirdTimer = false;
    fourthTimer = false;
    currentTime = millis();
    if ((unsigned long)(currentTime - previousTimer5) >= delay5sec) {
      Serial.println("fifth timer");
      turnAllLightsOnEnd();
      fifthTimer = false;
      previousTimer = 0;
      previousTimer2 = 0;
      previousTimer3 = 0;
      previousTimer4 = 0;
      previousTimer5 = 0;
      currentTime = 0;
      Serial.println("End of lightshow program");
    }
  }
}
