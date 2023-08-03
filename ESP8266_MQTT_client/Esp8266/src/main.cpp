#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// -------------------------------------------------------------------
// -------------------------------------------------------------------
// -------------------------------------------------------------------

#define TIME_BLINK 500
#define SaveDisconnectTime 1000
#define DEBUG_1(x) Serial.println((x));
#define DEBUG_2(n, x)  \
	Serial.print((n)); \
	Serial.println((x));

#define ESP_NAME "esp_8266"

const int mqttPort = 1883;
const char *c_TOPIC = "control-light/esp8266-led-1";
// const char *ssid = "VCCorp";
// const char *password = "Vcc123**";
const char *ssid = "HONGDO17_303";
const char *password = "123456789";
const char *mqttServer = "mqtt.bctoyz.com";
const char *mqttUser = "testled";
const char *mqttPassword = "testled123";

uint16_t u16t_MQTT_PACKET_SIZE = 2048;

WiFiClient espClient;
PubSubClient clientMQTT(espClient);

uint8_t numb = 0;

// -------------------------------------------------------------------
// -------------------------------------------------------------------
// -------------------------------------------------------------------

String int_to_string(int x)
{
	char buffer[100];
	sprintf(buffer, "%d", x);
	return buffer;
}

void setup_wifi()
{
	delay(10);
	// We start by connecting to a WiFi network
	Serial.println();
	Serial.print("Connecting to ");
	Serial.println(ssid);

	WiFi.mode(WIFI_STA);
	WiFi.begin(ssid, password);

	while (WiFi.status() != WL_CONNECTED)
	{
		delay(500);
		Serial.print(".");
	}

	randomSeed(micros());

	Serial.println("");
	Serial.println("WiFi connected");
	Serial.println("IP address: ");
	Serial.println(WiFi.localIP());
}

void callback(char *topic, byte *payload, unsigned int length)
{
	DEBUG_2("subscribe topic : ", topic);

	for (unsigned int i = 0; i < length; i++)
	{
		Serial.print((char)payload[i]);
	}
	Serial.println();

	// Switch on the LED if an 1 was received as first character
	if ((char)payload[0] == '1')
	{
		Serial.println("HIGHT");
		digitalWrite(LED_BUILTIN, HIGH);
		digitalWrite(D2, HIGH);
	}
	else if ((char)payload[0] == '0')
	{
		Serial.println("LOW");
		digitalWrite(LED_BUILTIN, LOW);
		digitalWrite(D2, LOW);
	}

	if ((char)payload[1] == '1')
	{
		Serial.println("restart");
		delay(50);
		ESP.restart();
	}

	if ((char)payload[2] == '1')
	{
		clientMQTT.publish(c_TOPIC, "ok esp8266");
	}
}

void connectMQTT()
{
	clientMQTT.setServer(mqttServer, mqttPort);
	clientMQTT.setBufferSize(u16t_MQTT_PACKET_SIZE);
	clientMQTT.setCallback(callback);

	Serial.print("Connecting to MQTT...");

	if (clientMQTT.connect(ESP_NAME, mqttUser, mqttPassword))
	{
		Serial.println("connected");
		clientMQTT.subscribe(c_TOPIC);
	}
	else
	{
		Serial.print("failed with state ");
		Serial.println(clientMQTT.state());
	}

	delay(50);
}

// -------------------------------------------------------------------
// -------------------------------------------------------------------
// -------------------------------------------------------------------

void setup()
{
	// init serial baud 115200
	Serial.begin(115200);
	delay(500);

	Serial.println();
	Serial.println("Publish message: ");

	// setup wifi
	setup_wifi();

	// init pin out
	pinMode(LED_BUILTIN, OUTPUT);
	pinMode(D2, OUTPUT);

	// delay
	delay(100);
}

// -------------------------------------------------------------------
// -------------------------------------------------------------------
// -------------------------------------------------------------------

void loop()
{

	while (!clientMQTT.connected())
	{
		connectMQTT();
	}

	// if (WiFi.status() == WL_CONNECTED && clientMQTT.connected())
	// {
	// 	// numb++;
	// 	// String data = int_to_string(numb);
	// 	// bool resultMQTT = clientMQTT.publish(c_TOPIC, "ok esp8266");
	// 	// Serial.print("MQTT Result: ");
	// 	// Serial.println(numb);
	// }

	clientMQTT.loop();
	delay(100);
}

// -------------------------------------------------------------------
// -------------------------------------------------------------------
// -------------------------------------------------------------------