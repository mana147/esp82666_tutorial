#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Arduino_JSON.h>
// #include <ESP8266WebServer.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>

#include <DNSServer.h>
#include "FS.h"

// -------------------------------------------------------------------
// -------------------------------------------------------------------
// -------------------------------------------------------------------

#define TIME_BLINK 500
#define SaveDisconnectTime 1000
#define DEBUG_1(x) Serial.println((x));
#define DEBUG_2(n, x)  \
	Serial.print((n)); \
	Serial.println((x));

#define ESP_NAME "esp_8266_nodeMCU"
const int mqttPort = 1883;
const char *c_TOPIC = "control-light/esp8266-led-2";
// const char *ssid = "VCCorp";
// const char *password = "Vcc123**";
String ssid = "HONGDO17_303";
String password = "123456789";
const char *mqttServer = "mqtt.bctoyz.com";
const char *mqttUser = "testled";
const char *mqttPassword = "testled123";

const char *c_PATH_FILE_TXT = "/data.json";
const char *htmlfile_config = "/config.html";
const char *jsfile_jquery = "/jquery.js";

uint16_t u16t_MQTT_PACKET_SIZE = 2048;

AsyncWebServer server(80);
WiFiClient espClient;
PubSubClient clientMQTT(espClient);
DNSServer dnsServer;

uint8_t numb = 0;

IPAddress local_IP(192, 168, 4, 99); //  IP tĩnh muốn gán cho module
IPAddress gateway(192, 168, 4, 99);	 //  IP của gateway (thường là router) để kết nối ra mạng bên ngoài
IPAddress subnet(255, 255, 255, 0);	 //  subnet xác định phạm vi IP của mạng nội bộ

const char *ssidWifi = "ESP8266"; // ten wifi phát ra
const char *passwordWifi = "123456789";
const char *HostName = "esp.com";
const byte DNS_PORT = 53;

// -------------------------------------------------------------------
// -------------------------------------------------------------------
// -------------------------------------------------------------------

String int_to_string(int x)
{
	char buffer[100];
	sprintf(buffer, "%d", x);
	return buffer;
}

String read_files_in_SPIFFS(String string)
{
	String str;
	File data_files;
	data_files = SPIFFS.open(string, "r");
	if (data_files)
	{
		while (data_files.available())
		{
			str = data_files.readString();
			data_files.flush();
		}
		data_files.close();
	}
	return str;
}

void handle_data_json(String payload)
{
	JSONVar myObject = JSON.parse(payload);

	if (JSON.typeof(myObject) == "undefined")
	{
		Serial.println("Parsing input failed!");
		return;
	}
}

int write_files_in_SPIFFS(String string, String path)
{
	int key;
	File data_flies;
	data_flies = SPIFFS.open(path, "w");
	if (data_flies)
	{
		data_flies.print(string);
		data_flies.flush();
		key = 1;
	}
	else
	{
		key = 0;
	}

	data_flies.close();
	return key;
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
		clientMQTT.publish(c_TOPIC, "status: 1");
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

void handleRoot(AsyncWebServerRequest *request)
{
	const char *dataType = "text/html";

	String str_html = read_files_in_SPIFFS(htmlfile_config);

	Serial.println("Stream the array!");

	AsyncWebServerResponse *response = request->beginResponse_P(200, dataType, str_html.c_str());

	// response->addHeader("Content-Encoding", "gzip");
	request->send(response);
}

void load_jquery()
{
	const char *dataType = "text/javascript";

	String str = read_files_in_SPIFFS(htmlfile_config);

	Serial.println("Stream the array!");

	// AsyncWebServerResponse *response = request->beginResponse_P(200, dataType, str_html.c_str());

	// response->addHeader("Content-Encoding", "gzip");
	// request->send(response);
}

// -------------------------------------------------------------------
// -------------------------------------------------------------------
// -------------------------------------------------------------------
#define ON 0
#define OFF 1
int buttonState = 0;

void setup()
{
	// init serial baud 115200
	Serial.begin(115200);
	// init SPIFFS
	Serial.print(F("Inizializing FS..."));
	if (SPIFFS.begin())
	{
		Serial.println(F("done."));
	}
	else
	{
		Serial.println(F("fail."));
	}
	// server web esp8266
	server.begin();

	// init pin out
	pinMode(D9, OUTPUT);
	pinMode(D4, INPUT);

	digitalWrite(D9, OFF);

	buttonState = digitalRead(D4);
	if (buttonState == 0)
	{
		DEBUG_1("chế độ config");
		WiFi.softAPConfig(local_IP, gateway, subnet);
		WiFi.softAP(ssidWifi, passwordWifi);

		dnsServer.setTTL(10);
		dnsServer.setErrorReplyCode(DNSReplyCode::ServerFailure);
		dnsServer.start(DNS_PORT, HostName, local_IP);

		server.on("/", handleRoot);
		// server.on("/jquery.min.js", HTTP_GET, load_jquery);
	}
	else if (buttonState == 1)
	{
		DEBUG_1("chế độ WIFI");
		DEBUG_1("> get list data beacon from SPIFFS");

		String data_txt = read_files_in_SPIFFS(c_PATH_FILE_TXT);
		JSONVar myObject = JSON.parse(data_txt);
		if (JSON.typeof(myObject) == "undefined")
		{
			Serial.println("Parsing input failed!");
			return;
		}

		String ESP_NAME_ = myObject["ESP_NAME"];
		String TOPIC_ = myObject["TOPIC"];
		String ssid_ = myObject["ssid"];
		String password_ = myObject["password"];
		String mqttPort_ = myObject["mqttPort"];

		ssid = ssid_;
		password = password_;

		// setup wifi
		DEBUG_1("setup wifi");
		setup_wifi();
	}

	// delay
	delay(100);
}

// -------------------------------------------------------------------
// -------------------------------------------------------------------
// -------------------------------------------------------------------

void loop(void)
{
	if (buttonState == 0)
	{
		dnsServer.processNextRequest();
		// server.handleClient();
	}
	else if (buttonState == 1)
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
}

// -------------------------------------------------------------------
// -------------------------------------------------------------------
// -------------------------------------------------------------------