
#include <WiFiUdp.h>
#include <WiFiServer.h>
#include <WiFiClientSecure.h>
#include <WiFiClient.h>
#include <ESP8266WiFiType.h>
#include <ESP8266WiFiSTA.h>
#include <ESP8266WiFiScan.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266WiFiGeneric.h>
#include <ESP8266WiFiAP.h>
#include <ESP8266WiFi.h>


const int trig = 5;     // chân trig của HC-SR04
const int echo = 4;     // chân echo của HC-SR04

WiFiServer server(80);  // tao server voi cong 80

void setup()
{
	Serial.begin(115200);
	pinMode(trig, OUTPUT);
	pinMode(echo, INPUT);

	WiFi.begin("FTP Telecom", "12345678");
	Serial.print("Connecting");
	while (WiFi.status() != WL_CONNECTED )
	{
		delay(500);
		Serial.print(".");
	}
	Serial.print("Connected ");
	server.begin();
	Serial.println("Khoi dong Server");
	Serial.println(WiFi.localIP());
}

void loop()
{
	WiFiClient client = server.available();
	if (!client)
	{
		return;
	}
	Serial.println(" co mot client dang ket noi xem du lieu ");
	while (!client.available())
	{
		delay(1);
	}

	hcSr_05(); //

	String rep = client.readStringUntil('\r');
	Serial.println(rep);
	client.flush();

}

//-------------------------------------------------------
void hcSr_05()
{
	unsigned long duration;
	int distance;
	digitalWrite(trig, 0);
	delayMicroseconds(2);
	digitalWrite(trig, 1);
	delayMicroseconds(5);
	digitalWrite(trig, 0);
	duration = pulseIn(echo, HIGH);
	distance = int(duration / 2 / 29.412);
	Serial.print(distance);
	Serial.println("cm");
	delay(200);
}