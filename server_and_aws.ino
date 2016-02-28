/**
 * DOCS:
 *    https://github.com/esp8266/Arduino/blob/master/doc/reference.md
 *    http://playground.arduino.cc/Main/DHT11Lib
 */
 
#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include <ESP8266WebServer.h>
//#include <dht11.h>
#include <aws_iot_mqtt.h>
#include "aws_iot_config.h"

aws_iot_mqtt_client myClient;

const char* ssid = "NAME_AP";
const char* password = "PASSWORD_AP";

ESP8266WebServer server(80);

void msg_callback(char* src, int len) {
  Serial.println("CALLBACK:");
  int i;
  for(i = 0; i < len; i++) {
    Serial.print(src[i]);
  }
  Serial.println("");
}
//dht11 DHT11;
#define DHT11PIN 5
String temp_txt = "";
 int rc;
void handleRoot()
{
  server.send(200, "text/plain", get_temp_humidity_string());
}

void setup()
{
 
  delay(5000);
  Serial.begin(115200);
  delay(5000);
  connect_as_hosting_wifi();

  // Setup AWS IOT Client
  if((rc = myClient.setup(AWS_IOT_CLIENT_ID)) == 0) {
    // Load user configuration
    if((rc = myClient.config(AWS_IOT_MQTT_HOST, AWS_IOT_MQTT_PORT, AWS_IOT_ROOT_CA_PATH, AWS_IOT_PRIVATE_KEY_PATH, AWS_IOT_CERTIFICATE_PATH)) == 0) {
        // Use default connect: 60 sec for keepalive
        if((rc = myClient.connect()) == 0) {
            // Do nothing for now
        }
    }
  }

  if((rc = myClient.subscribe("HeatingRoomChannel", 1, msg_callback)) != 0) {
    Serial.println("Subscribe failed!");
    Serial.println(rc);
  }
  delay(2000);

}

void connect_as_hosting_wifi()
{
  WiFi.softAP(ssid, password);
  temp_txt = "WiFi setup with " + (String) ssid + ":" + (String) password;
  Serial.println(temp_txt);

  IPAddress myIP = WiFi.softAPIP();
  
  server.on("/", handleRoot);
  
  server.begin();
  temp_txt = "HTTP Server started on " + (String) myIP;
  Serial.println(temp_txt);
}

String get_temp_humidity_string()
{
  String txt;
      return "Unknown error"; 
}

void loop()
{
  server.handleClient();
  String msg;
  msg = get_temp_humidity_string();
  char *topic="HeatingRoomChannel";
  if((rc = myClient.publish(topic, "hello" ,msg.length(),1, false)) != 0) {
    Serial.println("Publish failed!");
    Serial.println(rc);
  }
  if((rc = myClient.yield()) != 0) {
    Serial.println("Yield failed!");
    Serial.println(rc);
  }
  delay(5000);
}
