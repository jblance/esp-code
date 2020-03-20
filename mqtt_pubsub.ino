/*
 ESP8266 MQTT pub sub + serial connectivity
 Based on code from Andreas Spiess https://github.com/SensorsIot/MQTT-Examples/blob/master/ESP_MQTT_PUBSUB_LIBRARY/ESP_MQTT_PUBSUB_LIBRARY.ino

 ESP8266 board
 - connects to MQTT broker
 - subscribes to "T_COMMAND" topic for commands to action
 - sends any commands to serial port and get response
 - publishes response (unchanged) to "T_RESPONSE" topic
*/

/*
 Original comment
 PUBSUB MQTT library

 Basic ESP8266 MQTT example
 This sketch demonstrates the capabilities of the pubsub library in combination
 with the ESP8266 board/library.
 It connects to an MQTT server then:
  - publishes "hello world" to the topic "outTopic" every two seconds
  - subscribes to the topic "inTopic", printing out any messages
    it receives. NB - it assumes the received payloads are strings not binary
  - If the first character of the topic "inTopic" is an 1, switch ON the ESP Led,
    else switch it off
 It will reconnect to the server if the connection is lost using a blocking
 reconnect function. See the 'mqtt_reconnect_nonblocking' example for how to
 achieve the same result without blocking the main loop.
 To install the ESP8266 board, (using Arduino 1.6.4+):
  - Add the following 3rd party board manager under "File -> Preferences -> Additional Boards Manager URLs":
       http://arduino.esp8266.com/stable/package_esp8266com_index.json
  - Open the "Tools -> Board -> Board Manager" and click install for the ESP8266"
  - Select your ESP8266 in "Tools -> Board"
  Code from Andreas Spiess
*/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

//credentials file
#include "credentials.h"

/*
#define WIFI_SSID       "..."
#define WIFI_PASSWD     "..."
#define MQTT_BROKER     "..."
#define MQTT_PORT       1883
#define MQTT_USERNAME   "..."
#define MQTT_KEY        "..."
*/
#define T_COMMAND       "/inverter0/command"
#define T_RESPONSE      "/inverter0/response"

unsigned long entry;
byte clientStatus, prevClientStatus = 99;
float luminosity, prevLumiosity = -1;
char valueStr[5];

WiFiClient WiFiClient;
PubSubClient client(WiFiClient);

//
void setup() {
  //pinMode(LEDBLUE, OUTPUT);
  Serial.begin(115200);
  delay(100);
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WIFI_SSID);
  
  // Connect to WiFi
  WiFi.begin(WIFI_SSID, WIFI_PASSWD);
  while (WiFi.status() != WL_CONNECTED) {
    // add a delay before checking if connected again
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  WiFi.printDiag(Serial);

  // set-up MQTT config
  client.setServer(MQTT_BROKER, MQTT_PORT);
  client.setCallback(callback);
}

//
void loop() {
  //Serial.println("-");
  yield();
  // Attempt to connect if not connected
  if (!client.connected()) {
    Serial.println("Attempting MQTT connection...");
    // Attempt to connect to MQTT broker .connect(clientID, username, password) or .connect(clientID)
    if (client.connect("", MQTT_USERNAME, MQTT_KEY)) {
      Serial.println("connected");
      // Subscribe to command topic
      client.subscribe(T_COMMAND, 1);
      // ^ need to check single topic vs client.subscribe(USERNAME PREAMBLE T_COMMAND, 1)
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000); // Wait 5 seconds before retrying
    }
  }

  // 1.2 sec delay between readings
  if (millis() - entry > 1200) {
    Serial.println("Measure");
    entry = millis();
    luminosity = analogRead(A0);
  }

  // publish only changes (with 0.5s delay to match 2/sec max of adafruit)
  if (client.connected() && prevLumiosity != luminosity) {
    Serial.println("Publish Luminosity");
    String hi = (String)luminosity;
    hi.toCharArray(valueStr, 5);
    client.publish(USERNAME PREAMBLE T_LUMINOSITY, valueStr);
    prevLumiosity = luminosity;
    delay(500);
  }

  if (client.connected()&& prevClientStatus != clientStatus ) {
    Serial.println("Publish Status");

    String hi = (String)clientStatus;
    hi.toCharArray(valueStr, 2);
    client.publish(USERNAME PREAMBLE T_CLIENTSTATUS, valueStr);
    prevClientStatus = clientStatus;
  }
  client.loop();
}


//----------------------------------------------------------------------
void callback(char* topic, byte * data, unsigned int length) {
  // handle message arrived {

  Serial.print(topic);
  Serial.print(": ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)data[i]);
  }
  Serial.println();
  // this would be where we send the command to the serial port
  // and publish the response

  /*
  if (data[1] == 'F')  {
    clientStatus = 0;
    //digitalWrite(LEDBLUE, LOW);
  } else {
    clientStatus = 1;
    //digitalWrite(LEDBLUE, HIGH);
  */
  }
}
