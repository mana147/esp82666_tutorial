
#include <ESP8266WiFi.h>
#include <ESP8266Ping.h>
#include <Hash.h>
#include <Arduino.h>
#include "esp8266-websocketclient-master/WebSocketClient.h"

WebSocketClient ws = true;

const char* ssid = "Trang Lee";

const char* password = "dong5599";

const char* remote_host = "www.google.com";

void SETUP_WIFI_STATION()
{
	Serial.begin(115200);
	WiFi.mode(WIFI_STA);
	//-----------------------------------------------

	Serial.printf("Connecting to %s \n", ssid);
	WiFi.begin(ssid, password);
	while (WiFi.status() != WL_CONNECTED)
	{
		delay(500);
		Serial.print(".");
	}
	Serial.println();
	Serial.print("Connected, IP address: ");
	Serial.println(WiFi.localIP());
}

void SETUP_PING()
{
	Serial.print("Pinging host ");

	Serial.println(remote_host);
	if (Ping.ping(remote_host))
	{
		Serial.println("Success");
	}
	else
	{
		Serial.println("Error");
	}
}

//-------------------------------------------------------

void setup()
{
	SETUP_WIFI_STATION();
	SETUP_PING();
}

void loop() 
{
	if (!ws.isConnected()) {
		ws.connect("echo.websocket.org", "/", 443);
	}
	else {
		ws.send("hello");

		String msg;
		if (ws.getMessage(msg)) {
			Serial.println(msg);
		}
	}
	delay(500);
}