#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Arduino_JSON.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <string.h>
#include "FS.h"
#include "confightml.h"

// -------------------------------------------------------------------
// -------------------------------------------------------------------
// -------------------------------------------------------------------

#define TIME_BLINK 500
#define SaveDisconnectTime 1000
#define DEBUG_1(x) Serial.println((x));
#define DEBUG_2(n, x)  \
	Serial.print((n)); \
	Serial.println((x));

#define ON 0
#define OFF 1
int buttonState = 0;

#define pin_input_01 16
#define pin_BUILTIN_LED 2
#define pin_out_1 2
#define pin_out_2 3

String ESP_NAME = "esp_8266";
String mqttPort = "1883";
String c_TOPIC = "control-light/esp8266-led-2";

// const char *ssid = "VCCorp";
// const char *password = "Vcc123**";

String ssid = "HONGDO17_303";
String password = "123456789";
String mqttServer = "mqtt.bctoyz.com";
String mqttUser = "testled";
String mqttPassword = "testled123";

const char *data_json = "/data.json";
const char *htmlfile_config = "/config.html";
const char *jsfile_jquery = "/jquery.js";

uint16_t u16t_MQTT_PACKET_SIZE = 2048;

ESP8266WebServer server(80);
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

// uint8_t *buf = new uint8_t[jquery_js_len];

// -------------------------------------------------------------------
// -------------------------------------------------------------------
// -------------------------------------------------------------------

// format bytes
String formatBytes(size_t bytes)
{
	if (bytes < 1024)
	{
		return String(bytes) + "B";
	}
	else if (bytes < (1024 * 1024))
	{
		return String(bytes / 1024.0) + "KB";
	}
	else if (bytes < (1024 * 1024 * 1024))
	{
		return String(bytes / 1024.0 / 1024.0) + "MB";
	}
	else
	{
		return String(bytes / 1024.0 / 1024.0 / 1024.0) + "GB";
	}
}

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

void setup_wifi(String ssid_, String password_)
{
	delay(10);
	// We start by connecting to a WiFi network
	Serial.println();
	Serial.print("Connecting to ");
	Serial.println(ssid_);

	WiFi.mode(WIFI_STA);
	WiFi.begin(ssid_, password_);

	while (WiFi.status() != WL_CONNECTED)
	{
		digitalWrite(pin_BUILTIN_LED, HIGH);
		delay(100);
		digitalWrite(pin_BUILTIN_LED, LOW);
		delay(100);
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
		clientMQTT.publish(c_TOPIC.c_str(), "status: 1");
	}
}

void connectMQTT()
{
	clientMQTT.setServer(mqttServer.c_str(), mqttPort.toInt());
	clientMQTT.setBufferSize(u16t_MQTT_PACKET_SIZE);
	clientMQTT.setCallback(callback);

	Serial.print("Connecting to MQTT...");

	if (clientMQTT.connect(ESP_NAME.c_str(), mqttUser.c_str(), mqttPassword.c_str()))
	{
		Serial.println("connected");
		clientMQTT.subscribe(c_TOPIC.c_str());
	}
	else
	{
		Serial.print("failed with state ");
		Serial.println(clientMQTT.state());
	}

	delay(50);
}

// -----------------------------------------------
void root_page()
{
	DEBUG_1("> load_web");

	String dataType = "text/html";

	if (SPIFFS.exists(htmlfile_config))
	{
		File dataFile = SPIFFS.open(htmlfile_config, "r");
		if (!dataFile)
		{
			DEBUG_1(" read dataFile false !");
			return;
		}
		if (server.streamFile(dataFile, dataType) != dataFile.size())
		{
			DEBUG_1("Sent less data than expected!");
		}
		else
		{
			DEBUG_1("Page served!");
		}

		dataFile.close();
	}
	else
	{
		DEBUG_1("SPIFFS false")
		return;
	}
	DEBUG_1("done");
}

void jqueryjs()
{
	DEBUG_1("> load_jquery");
	String dataType = "text/javascript";

	if (SPIFFS.exists(jsfile_jquery))
	{
		File dataFile = SPIFFS.open(jsfile_jquery, "r");
		if (!dataFile)
		{
			DEBUG_1(" read dataFile false !");
			return;
		}

		if (server.streamFile(dataFile, dataType) != dataFile.size())
		{
			DEBUG_1("Sent less data than expected!");
		}
		else
		{
			DEBUG_1("Page served!");
		}

		dataFile.close();
	}
	else
	{
		DEBUG_1("SPIFFS false")
		return;
	}
	DEBUG_1("done");
}

void load_jquery_b()
{
	DEBUG_1("> load_jquery");
	String dataType = "text/javascript";
	// server.sendHeader(F("Content-Encoding"), F("gzip"));
	// server.send(200, "text/javascript", (const char *)jquery_js);
	// DEBUG_1(size);
}

void getdatajson()
{
	DEBUG_1("> load_web");

	String dataType = "application/json";

	if (SPIFFS.exists(data_json))
	{
		File dataFile = SPIFFS.open(data_json, "r");
		if (!dataFile)
		{
			DEBUG_1(" read dataFile false !");
			return;
		}
		if (server.streamFile(dataFile, dataType) != dataFile.size())
		{
			DEBUG_1("Sent less data than expected!");
		}
		else
		{
			DEBUG_1("Page served!");
		}

		dataFile.close();
	}
	else
	{
		DEBUG_1("SPIFFS false")
		return;
	}
	DEBUG_1("done");
}

void postdatajson()
{
	if (server.method() != HTTP_POST)
	{
		server.send(405, "text/plain", "Method Not Allowed");
	}
	else
	{
		String arg_ESP_NAME = server.arg("ESP_NAME");
		String arg_TOPIC = server.arg("TOPIC");
		String arg_ssid = server.arg("ssid");
		String arg_password = server.arg("password");
		String arg_mqttPort = server.arg("mqttPort");
		String arg_mqttServer = server.arg("mqttServer");
		String arg_mqttUser = server.arg("mqttUser");
		String arg_mqttPassword = server.arg("mqttPassword");
		String arg_pinout1 = server.arg("pinout1");
		String arg_pinout2 = server.arg("pinout2");
		String arg_pinout3 = server.arg("pinout3");

		// tạo một cấu trúc JSON
		String json_ = "{\n";
		json_ += "\"ESP_NAME\": \"" + arg_ESP_NAME + "\",\n";
		json_ += "\"TOPIC\": \"" + arg_TOPIC + "\",\n";
		json_ += "\"ssid\": \"" + arg_ssid + "\",\n";
		json_ += "\"password\": \"" + arg_password + "\",\n";
		json_ += "\"mqttPort\" : " + arg_mqttPort + ",\n";
		json_ += "\"mqttServer\": \"" + arg_mqttServer + "\",\n";
		json_ += "\"mqttUser\": \"" + arg_mqttUser + "\",\n";
		json_ += "\"mqttPassword\": \"" + arg_mqttPassword + "\",\n";
		json_ += "\"pinout1\" : \"" + arg_pinout1 + "\",\n";
		json_ += "\"pinout2\" : \"" + arg_pinout2 + "\",\n";
		json_ += "\"pinout3\" : \"" + arg_pinout3 + "\"\n";
		json_ += "}";

		DEBUG_1(json_);

		int write_data = write_files_in_SPIFFS(json_, data_json);

		server.send(200, "text/plain", (String)write_data);
	}
}

// -------------------------------------------------------------------
// -------------------------------------------------------------------
// -------------------------------------------------------------------

void setup()
{
	// init serial baud 115200
	Serial.begin(115200);
	DEBUG_1("");
	DEBUG_1("");

	// init SPIFFS
	SPIFFS.begin();
	// get info SPIFFS

	Dir dir = SPIFFS.openDir("/");
	while (dir.next())
	{
		String fileName = dir.fileName();
		size_t fileSize = dir.fileSize();
		Serial.printf("FS File: %s, size: %s\n", fileName.c_str(), formatBytes(fileSize).c_str());
	}
	Serial.printf("\n");

	// init pin out
	pinMode(pin_BUILTIN_LED, OUTPUT);
	pinMode(D3, OUTPUT);
	pinMode(D4, OUTPUT);

	// off led
	digitalWrite(pin_BUILTIN_LED, OFF);
	digitalWrite(D3, OFF);
	digitalWrite(D4, OFF);

	// while (1)
	// {
	// 	digitalWrite(D4, ON);
	// 	digitalWrite(D3, ON);
	// 	digitalWrite(pin_BUILTIN_LED, ON);
	// 	delay(500);

	// 	digitalWrite(D4, OFF);
	// 	digitalWrite(D3, OFF);
	// 	digitalWrite(pin_BUILTIN_LED, OFF);
	// 	delay(500);
	// }

	// pin button setup option config
	pinMode(pin_input_01, INPUT);
	buttonState = digitalRead(pin_input_01);
	if (buttonState == 0)
	{
		DEBUG_1("chế độ config");
		WiFi.softAPConfig(local_IP, gateway, subnet);
		WiFi.softAP(ssidWifi, passwordWifi);

		dnsServer.setTTL(10);
		dnsServer.setErrorReplyCode(DNSReplyCode::ServerFailure);
		dnsServer.start(DNS_PORT, HostName, local_IP);

		server.on("/", HTTP_GET, root_page);
		server.on("/jquery.js", HTTP_GET, jqueryjs);
		server.on("/getdatajson", HTTP_GET, getdatajson);
		server.on("/postdatajson", HTTP_POST, postdatajson);

		// server.on("/jquery.js", HTTP_GET, load_jquery_b);
		// server web esp8266

		server.enableCORS(true);
		server.begin();
	}
	else if (buttonState == 1)
	{
		DEBUG_1("chế độ MQTT");

		String data_txt = read_files_in_SPIFFS(data_json);

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

		String mqttServer_ = myObject["mqttServer"];
		String mqttUser_ = myObject["mqttUser"];
		String mqttPassword_ = myObject["mqttPassword"];

		ESP_NAME = ESP_NAME_;
		c_TOPIC = TOPIC_;
		ssid = ssid_;
		password = password_;
		mqttServer = mqttServer_;
		mqttUser = mqttUser_;
		mqttPassword = mqttPassword_;

		// setup wifi
		DEBUG_1("setup wifi");
		setup_wifi(ssid_, password_);
	}

	// delay
	delay(100);
}

// -------------------------------------------------------------------
// -------------------------------------------------------------------
// -------------------------------------------------------------------

void loop()
{
	if (buttonState == 0)
	{
		dnsServer.processNextRequest();
		server.handleClient();
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