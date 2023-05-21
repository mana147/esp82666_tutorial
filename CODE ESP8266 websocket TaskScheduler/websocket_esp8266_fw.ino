#include <ArduinoWebsockets.h>
#include <TaskScheduler.h>
#include <ESP8266WiFi.h>
#include "lib/ESP8266HTTPClient.h"
#include <Hash.h>
#include "FS.h"

#include <stddef.h>

#define s Serial
#define s_p	Serial.print
#define s_pf Serial.printf
#define s_pln Serial.println

using namespace websockets;
WebsocketsClient client;
Scheduler runner;

Task t1(1000, TASK_FOREVER, &t1Callback);  // Task1 chạy cứ sau 1s vô thời hạn
Task t2(100, TASK_FOREVER, &t2Callback);
Task t3(50, TASK_FOREVER, &t3Callback);

//const char* ssid = "Trang Lee"; //Enter SSID
//const char* password = "dong5599"; //Enter Password

const char* ssid = "dell123"; //Enter SSID
const char* password = "123456789"; //Enter Password

//const char* ssid = "MPEC1"; //Enter SSID
//const char* password = "dtth2014"; //Enter Password

const char* websockets_connection_string = "wss://demos.kaazing.com/echo"; //Enter server adress
//const char* websockets_connection_string = "wss://echo.websocket.org/"; //Enter server adress
//const char* websockets_connection_string = "ws://echo.websocket.org:80"; //Enter server adress
const char echo_org_ssl_fingerprint[] = "A4 F9 86 C7 7C 31 10 F8 D0 6F CB 80 02 86 F4 20 7A 6D 1E 44";


void MAIN_HTTPS_GET_STREAM(String link, String SHA1)
{
	HTTPClient https;

	WiFiClient * stream;

	if (WiFi.status() == WL_CONNECTED)
	{
		if (https.begin(link, SHA1))
		{
			int getCODE = https.GET();	s_pln("HTPPS GET CODE : " + String(getCODE));

			int getSIZE = https.getSize();	s_pln("HTTPS GET SIZE : " + String(getSIZE));

			String contentType = https.headers("Type"); s_pln("HTTPS HEADER Type: " + contentType);

			String Disposition = https.headers("Disposition"); s_pln("HTTPS HEADER Disposition : " + Disposition);

			String Date = https.headers("Date"); s_pln("HTTPS HEADER Date : " + Date);

			String Local = https.headers("Location"); s_pln("HTTPS HEADER Location : " + Local);

			String filename = Disposition.substring(Disposition.indexOf('=') + 2, Disposition.lastIndexOf(';') - 1);

			s_pln("file name : /" + filename);
			//-------------------------------------------------------

			if (contentType.equalsIgnoreCase("application/octet-stream"))
			{
				stream = https.getStreamPtr();

				fs::File f = SPIFFS.open("/" + filename, "w+");

				if (!f) { s_pln("file open failed"); stream->stop(); }

				int received; int remaining = getSIZE; uint8_t buff[512];

				while (stream->connected() && stream->available() && remaining > 0)
				{
					//String line = stream->readStringUntil('\n'); s_pln(line);

					received = stream->readBytes(buff, ((remaining > sizeof(buff)) ? sizeof(buff) : remaining));

					Serial.write(buff, received);

					//f.write(buff, received);

					if (remaining > 0) remaining -= received;

					yield();
				}

				if (remaining != 0) s_pln(" HTTPS -" + String(remaining));

				f.close();	 s_pln("HTTPS : end  : done down loand ");
			}
			https.end();
		}
	}
}

void t1Callback()
{
	// Send a message
	client.send("Hello Server");
}

void t2Callback()
{ư
	s_pln("t2Callback");

	client.close();

	String link_download_file = "https://doc-0g-10-docs.googleusercontent.com/docs/securesc/ha0ro937gcuc7l7deffksulhg5h7mbp1/dadhhld32r4ti2dei0h0o7vluedgdp29/1565265600000/12366162400552597640/*/1lGo31hEOZ8xwuDUMC6pjfiITZheHajDb?e=download";

	String SHA1 = "AC 0E 8D F7 8B DE 0E C4 0E 36 DC CD EB 30 68 33 32 5B 0E 9D";

	//String link_download_file = "https://www.websocket.org/aboutwebsocket.html";

	//String SHA1 = "A4 F9 86 C7 7C 31 10 F8 D0 6F CB 80 02 86 F4 20 7A 6D 1E 44";

	MAIN_HTTPS_GET_STREAM(link_download_file, SHA1);

	t2.disable(); runner.deleteTask(t2);

	client.connect(websockets_connection_string);
}

void t3Callback()
{
	if (client.available()) {
		client.poll();
	}
}

void wifi_config()
{
	Serial.begin(115200);
	SPIFFS.begin();

	WiFi.mode(WIFI_STA);
	WiFi.disconnect();
	WiFi.begin(ssid, password); s_pln("Connecting");

	while (WiFi.status() != WL_CONNECTED) {
		delay(200); s_p(".");
	}

	s_pln(); s_pf("Connected, IP address: "); s_pln(WiFi.localIP());
}

void socket_config()
{
	//client.setFingerprint(echo_org_ssl_fingerprint);

	//client.setInsecure();

	bool connected = client.connect(websockets_connection_string);

	if (connected) {
		Serial.println("Connecetd!"); client.send("Hello Server");
	}
	else {
		Serial.println("Not Connected!");
	}

	// run callback when messages are received
	client.onMessage([&](WebsocketsMessage message)
	{
		Serial.print("Got Message: ");
		Serial.println(message.data());
	});
}

void scheduler_config()
{
	s_pln("scheduler config ");

	runner.init();

	runner.addTask(t1);
	t1.enable();

	runner.addTask(t3);
	t3.enable();
}

void setup()
{
	wifi_config();

	t2Callback();

	socket_config();

	scheduler_config();
}

void loop()
{
	runner.execute();

	String str;

	if (Serial.available() > 0) {
		str = Serial.readString(); str.trim();  s_pln(str);
	}

	if (str.equals("AT"))
	{
		Serial.println("run task 2 download ...");
		runner.addTask(t2);
		t2.enable();
	}
}
