
#include <WebSockets.h>
//-----------------------------------------
#include <WiFiClientSecure.h>
//-----------------------------------------
#include <ESP8266WebServer.h>
//-----------------------------------------
#include "lib/ESP8266HTTPClient.h"
//-----------------------------------------
#include <WebSocketsClient.h>
//-----------------------------------------
#include <WebSocketsServer.h>
//-----------------------------------------
#include <ESP8266WiFi.h>
//-----------------------------------------
#include <ESP8266Ping.h>
//-----------------------------------------
#include <ArduinoJson.h>
//-----------------------------------------
#include "Function_detech.h"
//-----------------------------------------
#include "Function_web.h"
//-----------------------------------------
#include <Hash.h>
//-----------------------------------------
#include "FS.h"
//-----------------------------------------
#include "inc.h"

#define s Serial
#define s_p	Serial.print
#define s_pf Serial.printf
#define s_pln Serial.println

#define idWifiTest "nha 12"
#define	passWifiTest "tang2nha12"

//-----------------------------------------
String link_download_file = "https://doc-0g-10-docs.googleusercontent.com/docs/securesc/ha0ro937gcuc7l7deffksulhg5h7mbp1/r357jpf2dif3srievlm4rt79umr617sp/1567087200000/12366162400552597640/*/1lGo31hEOZ8xwuDUMC6pjfiITZheHajDb?e=download";
String SHA1 = "03:07:5D:58:FE:4B:2F:15:08:9E:C5:71:DC:24:BE:E5:2B:19:F4:D7";
//-----------------------------------------
DETECH_AT_CODE detech;
DETECH_CONFIG_JSON buffer_config;
DETECH_INDEX_JSON buffer_index;
//-----------------------------------------	 
WiFiClient * stream;
HTTPClient https;
WebSocketsClient SOCKET_CLIENT;
WebSocketsServer SOCKET_SERVER(81);
ESP8266WebServer WEB_SERVER(80);
//-----------------------------------------	
volatile bool Enabled_Socket_Server = false;
volatile bool Enabled_Socket_Client = false;
volatile bool Enabled_Web_Server = false;
volatile bool Enabled_Download = false;
//-----------------------------------------
const char *add_indexJson = "/index.json";
const char *add_configJson = "/config.json";
const char *add_indexHtml = "/index.html";
const char *add_imagefile = "/image.png"; 
//-----------------------------------------
const int httpsPort = 443;
const int pinled = D0;
const int pinbut_D1 = D1;
const int pinbut_D2 = D2;
//-----------------------------------------
//-----------------------------------------

String read_files_in_SPIFFS_String(const char * add)
{
	String s;

	File data_files = SPIFFS.open(add, "r");

	if (data_files)
	{
		while (data_files.available())
		{
			s = data_files.readString();

			data_files.flush();        // giải phóng bộ đệm
		}
		data_files.close(); // không có hoạt động nào khác trên đối tượng Files sau khi chức năng này được đóng 
	}

	return s;
}
//-----------------------------------------
void write_files_in_SPIFFS_String(const char * add, DynamicJsonDocument doc)
{
	SPIFFS.begin();

	File data_flies = SPIFFS.open(add_configJson, "w");

	if (data_flies)
	{
		serializeJsonPretty(doc, data_flies);
		data_flies.flush();
	}

	data_flies.close();
}
//-----------------------------------------
void MAIN_SOCKET_SERVER_EVENT(uint8_t channel, WStype_t type, uint8_t*payload, size_t length) // channel : kenh , type : trang thai , payload : data, string
{
	switch (type) // chuyển trạng thái hoạt động 
	{
		//---------------------------------------------------
	case WStype_DISCONNECTED:	// nếu WebS ngắt kết nối 
	{
		Serial.printf(" Disconnected %u \n ", channel);
		break;
	}
	//--------------------------------------------------
	case WStype_CONNECTED:		// nếu có kết nối mới 
	{
		Serial.printf("Connection %u \n", channel);
		break;
	}
	//--------------------------------------------------
	case WStype_TEXT:			// nếu data text mới đc received
	{
		String str = (char*)payload; // kiểm tra chuỗi , fix bug chuỗi NULL gây reset esp 
		Serial.println(str);
		break;
	}

	}
}
//--------------------------------------------------------
void MAIN_SOCKET_CLIENT_EVENT(WStype_t type, uint8_t * payload, size_t length)
{
	switch (type)
	{
	case WStype_DISCONNECTED:
	{
		//Serial.printf("[wss://] Disconnected! \n");
		break;
	}

	case WStype_CONNECTED:
	{
		s_pln("[wss://] send payload ");
		SOCKET_CLIENT.sendTXT("hello");
		break;
	}

	case WStype_TEXT:
	{
		Serial.printf("[wss://] get text : %s\n", payload);
		String str = (char*)payload;
		if (str.equals("down"))
			Enabled_Download = true;
		break;
	}

	}

}
//-----------------------------------------
void MAIN_WEB_HOME_PAGE()
{
	WEB_SERVER.send(200, "text/html", load_web_page_root());
}

void MAIN_WEB_IMG()
{
	if (SPIFFS.exists(add_imagefile))
	{
		File file = SPIFFS.open(add_imagefile, "r");
		size_t sent = WEB_SERVER.streamFile(file, "image/png");
		file.close();
		//s_pln(sent);
	}
}

void MAIN_WEB_LOGIN()
{
	WEB_SERVER.send(200, "text/html", "LOGIN");
}

void MAIN_WEB_DOWNLOAD()
{
	WEB_SERVER.send(200, "text/html", load_web_page_download());
}

void MAIN_WEB_UPLOAD()
{
	WEB_SERVER.send(200, "text/html", load_web_page_upload());
}

void MAIN_HANDLE_FILE_UPLOAD()
{
	s_pln("upload begin ...");

	File fsUploadFile;

	HTTPUpload & upload = WEB_SERVER.upload();

	if (upload.status == UPLOAD_FILE_START)
	{
		String filename = upload.filename;
		if (!filename.startsWith("/")) filename = "/" + filename;

		Serial.print("handleFileUpload Name: ");
		Serial.println(filename);

		fsUploadFile = SPIFFS.open(filename, "w");
		filename = String();
	}
	else if (upload.status == UPLOAD_FILE_WRITE)
	{
		if (fsUploadFile)
		{
			fsUploadFile.write(upload.buf, upload.currentSize);
		}
	}
	else if (upload.status == UPLOAD_FILE_END)
	{
		if (fsUploadFile)
		{
			fsUploadFile.close();
			Serial.print("handleFileUpload Size: ");
			Serial.println(upload.totalSize);

			WEB_SERVER.sendHeader("Location", "/dir");
			WEB_SERVER.send(303);
		}
		else
		{
			WEB_SERVER.send(500, "text/plain", "500: couldn't create file");
		}
	}

	s_pln("upload end !");
}

void MAIN_WEB_DELETE()
{
	WEB_SERVER.send(200, "text/html", "DELETE");
}

void MAIN_WEB_DIR()
{
#define numb 10	
	int i = 0;
	String str, name_file[numb], size_file[numb];
	Dir dir = SPIFFS.openDir("/");
	//-----------------------------------------
	while (dir.next())
	{
		name_file[i] = dir.fileName();

		if (dir.fileSize())
		{
			File f = dir.openFile("r");
			int bytes = f.size();
			String fsize = "";
			if (bytes < 1024)						fsize = String(bytes) + " B";
			else if (bytes < (1024 * 1024))			fsize = String(bytes / 1024.0, 3) + " KB";
			else if (bytes < (1024 * 1024 * 1024))	fsize = String(bytes / 1024.0 / 1024.0, 3) + " MB";
			else										fsize = String(bytes / 1024.0 / 1024.0 / 1024.0, 3) + " GB";

			size_file[i] = fsize;

			f.close();
		}

		i++;
	}
	//-----------------------------------------
	for (int i = 0; i < numb; i++)
	{
		if (name_file[i].length() <= 1) break;
		s_p(name_file[i]); s_p("-->");  s_pln(size_file[i]);
	}

	//-----------------------------------------	
	//-----------------------------------------
	str = "<!DOCTYPE html>\n";;
	str += "<html>\n";
	str += "<body>\n";

	//-----------------------------------------
	str += "<img src=\"\\img\" alt=\"Image from ESP8266\" width=\"400\" height=\"150\">\n";
	str += "<h2>Dir SPIFFS Esp8266 </h2>\n";
	//-----------------------------------------
	str += "<ul>\n";

	for (int i = 0; i < numb; i++)
	{
		if (name_file[i].length() <= 1) break;
		str += "<li>" + name_file[i] + " ---> " + size_file[i] + "</li>\n";
	}

	str += "</ul>  \n";
	//-----------------------------------------
	str += "<form action=\"fupload\" method=\"post\" enctype=\"multipart/form-data\">\n";
	str += "<input type=\"file\" name=\"fileToUpload\">\n";
	str += "<input type=\"submit\" value=\"Upload\" name=\"submit\">\n";
	str += "</form>";
	//-----------------------------------------
	str += "</body>\n";
	str += "</html>\n";
	//-----------------------------------------
	WEB_SERVER.send(200, "text/html", str);
	//-----------------------------------------
}

void MAIN_WEB_NOT_FOUND()
{
	WEB_SERVER.send(404, "text/plain", "404: Not found"); // Send HTTP status 404 (Not Found) when there's no handler for the URI in the request
}

void MAIN_HTTPS_GET_STREAM(String link, String SHA1)
{
	WiFiClient * stream;

	HTTPClient https;

	if (WiFi.status() == WL_CONNECTED)
	{
		if (https.begin(link, SHA1))
		{
			s_pln("HTTPS BEGIN : OK ");

			int getCODE = https.GET();	s_pln("HTPPS GET CODE : " + String(getCODE));

			int getSIZE = https.getSize();	s_pln("HTTPS GET SIZE : " + String(getSIZE));

			String contentType = https.headers("Type"); s_pln("HTTPS HEADER Type: " + contentType);

			String Disposition = https.headers("Disposition"); s_pln("HTTPS HEADER Disposition : " + Disposition);

			String Date = https.headers("Date"); s_pln("HTTPS HEADER Date : " + Date);

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

					f.write(buff, received);

					if (remaining > 0) remaining -= received;

					yield();
				}

				if (remaining != 0) s_pln(" HTTPS -" + String(remaining));

				f.close();	 s_pln("HTTPS : end  : done down loand ");
			}

			//-----------------------------------------------------

			https.end();
		}
	}
}

void HTTPSRequest(const char *host, const char *SHA1)
{
	WiFiClientSecure client;
	Serial.print("connecting to ");
	Serial.println(host);

	Serial.printf("Using fingerprint '%s'\n", SHA1);

	client.setFingerprint(SHA1);

	if (!client.connect(host, httpsPort)) {
		Serial.println("connection failed");
		return;
	}
}

//-----------------------------------------

void READ_FROM_FILE_CONFIG()
{
	SPIFFS.begin();
	String str = read_files_in_SPIFFS_String(add_configJson);

	DynamicJsonDocument doc(500);
	deserializeJson(doc, str);
	//-----------------------------------------

	int wifi_BaudRate = doc["wifi"]["BaudRate"]; // 115200
	int wifi_channel = doc["wifi"]["channel"]; // 1
	const char* wifi_mode = doc["wifi"]["mode"]; // "WIFI_STA"
	const char* wifi_ssid = doc["wifi"]["ssid"]; // "Wifi tang 2."
	const char* wifi_pass = doc["wifi"]["pass"]; // "080393itc"

	const char* socket_host = doc["socket"]["host"]; // "itc-server.dynu.net"
	const char* socket_mode = doc["socket"]["mode"]; // "SOCKET_CLIENT"
	int socket_port = doc["socket"]["port"]; // 8443
	const char* socket_url = doc["socket"]["url"]; // "/"
	const char* socket_wss = doc["socket"]["wss"]; // "WSS_ON"

	const char* web_mode = doc["web"]["mode"]; // "WEBSERVER_OFF"
	int web_port = doc["web"]["port"]; // 80
	const char* web_url = doc["web"]["url"]; // "/"

	//-----------------------------------------
	buffer_config.Wifi_BaudRate = wifi_BaudRate;
	buffer_config.Wifi_mode = wifi_mode;
	buffer_config.Wifi_ssid = wifi_ssid;
	buffer_config.Wifi_pass = wifi_pass;
	buffer_config.Wifi_Channel = wifi_channel;

	buffer_config.socket_mode = socket_mode;
	buffer_config.socket_host = socket_host;
	buffer_config.socket_port = socket_port;
	buffer_config.socket_url = socket_url;
	buffer_config.socket_wss = socket_wss;

	buffer_config.web_mode = web_mode;
	buffer_config.web_port = web_port;
	buffer_config.web_url = web_url;

	//-----------------------------------------
}

void READ_FROM_FILE_INDEX()
{
	String str = read_files_in_SPIFFS_String(add_indexJson);
	//-----------------------------------------
	DynamicJsonDocument doc(500);
	deserializeJson(doc, str);

	const char* sn = doc["sn"];
	const char* ac = doc["ac"];
	const char* cm = doc["cm"];
	const char* rx_data = doc["rx"]["data"];
	const char* rx_addr = doc["rx"]["addr"];
	//-----------------------------------------
	buffer_index.sn = sn;
	buffer_index.ac = ac;
	buffer_index.cm = cm;
	buffer_index.rx_addr = rx_data;
	buffer_index.rx_addr = rx_addr;
	//-----------------------------------------
}

void WRITE_TO_FILE_CONFIG()
{
	DynamicJsonDocument doc(500);

	JsonObject wifi = doc.createNestedObject("wifi");
	wifi["BaudRate"] = buffer_config.Wifi_BaudRate;
	wifi["channel"] = buffer_config.Wifi_Channel;
	wifi["mode"] = buffer_config.Wifi_mode;
	wifi["ssid"] = buffer_config.Wifi_ssid;
	wifi["pass"] = buffer_config.Wifi_pass;

	JsonObject socket = doc.createNestedObject("socket");
	socket["host"] = buffer_config.socket_host;
	socket["mode"] = buffer_config.socket_mode;
	socket["port"] = buffer_config.socket_port;
	socket["url"] = buffer_config.socket_url;
	socket["wss"] = buffer_config.socket_wss;

	JsonObject web = doc.createNestedObject("web");
	web["mode"] = buffer_config.web_mode;
	web["port"] = buffer_config.web_port;
	web["url"] = buffer_config.web_url;

	write_files_in_SPIFFS_String(add_configJson, doc);
}

//-----------------------------------------
void SETUP_PIN()
{
	pinMode(pinled, OUTPUT);
	pinMode(pinbut_D1, INPUT_PULLUP);
	pinMode(pinbut_D2, INPUT_PULLUP);
	digitalWrite(pinled, LOW);
}

void SETUP_AT()
{
	Serial.begin(buffer_config.Wifi_BaudRate);

	while (true)
	{
		String str;

		if (Serial.available() > 0)
		{
			str = Serial.readString(); str.trim(); s_pln(str);

			detech.D_AT(str); detech.D_CODE(str);

			if (detech.STR_AT[1].equals("WIFI"))
			{
				if (detech.STR_AT[2].equals("BAUDRATE"))
					buffer_config.Wifi_BaudRate = detech.STR_CODE[0].toInt();
				else if (detech.STR_AT[2].equals("CHANNEL"))
					buffer_config.Wifi_Channel = detech.STR_CODE[0].toInt();
				else if (detech.STR_AT[2].equals("MODE"))
					buffer_config.Wifi_mode = detech.STR_CODE[0];
				else if (detech.STR_AT[2].equals("SSID"))
					buffer_config.Wifi_ssid = detech.STR_CODE[0];
				else if (detech.STR_AT[2].equals("PASS"))
					buffer_config.Wifi_pass = detech.STR_CODE[0];
			}

			if (detech.STR_AT[1].equals("SOCKET"))
			{
				if (detech.STR_AT[2].equals("MODE"))
					buffer_config.socket_mode = detech.STR_CODE[0];
				else if (detech.STR_AT[2].equals("HOST"))
					buffer_config.socket_host = detech.STR_CODE[0];
				else if (detech.STR_AT[2].equals("PORT"))
					buffer_config.socket_port = detech.STR_CODE[0].toInt();
				else if (detech.STR_AT[2].equals("URL"))
					buffer_config.socket_url = detech.STR_CODE[0];
				else if (detech.STR_AT[2].equals("WSS"))
					buffer_config.socket_wss = detech.STR_CODE[0];
			}

			if (detech.STR_AT[1].equals("WEB"))
			{
				if (detech.STR_AT[2].equals("MODE"))
					buffer_config.web_mode = detech.STR_CODE[0];
				else if (detech.STR_AT[2].equals("PORT"))
					buffer_config.web_port = detech.STR_CODE[0].toInt();
				else if (detech.STR_AT[2].equals("URL"))
					buffer_config.web_url = detech.STR_CODE[0];
			}
		}

		if (str.equals("AT"))
		{
			Serial.println("AT SETPUP ... ");
			digitalWrite(pinled, HIGH);
			delay(200);
			digitalWrite(pinled, LOW);
		}
		else if (str.equals("AT+RESET"))
		{
			ESP.restart();
		}
		else if (str.equals("AT+CONFIG"))
		{
			s_pln("===========WIFI=============");
			s_p("BaudRate: "); s_pln(buffer_config.Wifi_BaudRate);
			s_p("Channel Wifi: "); s_pln(buffer_config.Wifi_Channel);
			s_p("MODE Wifi : "); s_pln(buffer_config.Wifi_mode);
			s_p("SSID Wifi : "); s_pln(buffer_config.Wifi_ssid);
			s_p("Pass Wifi : "); s_pln(buffer_config.Wifi_pass);
			s_pln("==========SOCKET===========");
			s_p("MODE Socket : "); s_pln(buffer_config.socket_mode);
			s_p("HOST : "); s_pln(buffer_config.socket_host);
			s_p("PORT : "); s_pln(buffer_config.socket_port);
			s_p("URL : "); s_pln(buffer_config.socket_url);
			s_p("WSS : "); s_pln(buffer_config.socket_wss);
			s_pln("==========WEB_SERVER=========");
			s_p("MODE Web : "); s_pln(buffer_config.web_mode);
			s_p("PORT : "); s_pln(buffer_config.web_port);
			s_p("URL : "); s_pln(buffer_config.web_url);
		}
		else if (str.equals("AT+START"))
		{
			break;
		}

		//break; // bo qua setup at
	}
}

void SETUP_CONFIG()
{
	Serial.begin(buffer_config.Wifi_BaudRate);
	//-----------------------------------------
	if (buffer_config.Wifi_mode.equals("WIFI_OFF"))
	{
		WiFi.mode(WIFI_OFF);
		Serial.println("WIFI_OFF");
	}
	else if (buffer_config.Wifi_mode.equals("WIFI_STA"))
	{
		WiFi.mode(WIFI_STA); s_pln("WIFI_STA");
		
		WiFi.disconnect(); delay(100); //s_pln(buffer_config.Wifi_ssid);

		//WiFi.begin(buffer_config.Wifi_ssid, buffer_config.Wifi_pass, buffer_config.Wifi_Channel);

		WiFi.begin(idWifiTest , passWifiTest);

		int t = 0;
		while (true)
		{
			t++; delay(200); s_p(".");
			if (t > 100)
			{
				s_pln("Connected lost times ");
				break;
			}
			if (WiFi.status() == WL_CONNECTED)
			{
				s_pln(" Connected OK  ");
				s_p("IP : "); s_pln(WiFi.localIP());
				//----------------------------------------------------
				//s_p("ping : google.com : ");
				//if (Ping.ping("www.google.com")) s_pln("Success");
				//else s_pln("Error");
				//----------------------------------------------------
				break;

			}
			else if (WiFi.status() == WL_CONNECT_FAILED)
			{
				s_pln("Connected Failed ");
				break;
			}
		}

		//----------------------------------------------------

		//----------------------------------------------------
	}
	else if (buffer_config.Wifi_mode.equals("WIFI_AP"))
	{
		WiFi.mode(WIFI_AP);
		Serial.println("WIFI_AP");
		WiFi.softAP(buffer_config.Wifi_ssid, buffer_config.Wifi_pass, buffer_config.Wifi_Channel, 0, 8);
		Serial.println(WiFi.softAPIP());
	}
	else if (buffer_config.Wifi_mode.equals("WIFI_AP_STA"))
	{
		WiFi.mode(WIFI_AP_STA);
		Serial.println("WIFI_AP_STA");
	}
	//-----------------------------------------
	if (buffer_config.socket_mode.equals("SOCKET_OFF"))
	{
		delay(50); s_pln("SOCKET_OFF");
	}
	else if (buffer_config.socket_mode.equals("SOCKET_SERVER"))
	{
		Serial.println("SOCKET_SERVER");
		SOCKET_SERVER.begin();
		SOCKET_SERVER.onEvent(MAIN_SOCKET_SERVER_EVENT);
		Enabled_Socket_Server = true;
	}
	else if (buffer_config.socket_mode.equals("SOCKET_CLIENT"))
	{
		s_pln("SOCKET_CLIENT");
		//SOCKET_CLIENT.begin(buffer_config.socket_host, buffer_config.socket_port);
		SOCKET_CLIENT.beginSSL(buffer_config.socket_host, buffer_config.socket_port);
		SOCKET_CLIENT.onEvent(MAIN_SOCKET_CLIENT_EVENT);
		Enabled_Socket_Client = true;
	}
	//-----------------------------------------
	if (buffer_config.web_mode.equals("WEB_OFF"))
	{
		delay(50); s_pln("WEB_OFF");
	}
	else if (buffer_config.web_mode.equals("WEB_CLIENT"))
	{
		s_pln("WEB_CLIENT : update continued ..... ");
		// to be continued .....
	}
	else if (buffer_config.web_mode.equals("WEB_SERVER"))
	{
		s_pln("WEB_SERVER");

		WEB_SERVER.begin();

		WEB_SERVER.on("/", HTTP_GET, MAIN_WEB_HOME_PAGE);
		WEB_SERVER.on("/img", HTTP_GET, MAIN_WEB_IMG);
		WEB_SERVER.on("/login", HTTP_GET, MAIN_WEB_LOGIN);
		WEB_SERVER.on("/download", HTTP_GET, MAIN_WEB_DOWNLOAD);
		WEB_SERVER.on("/upload", HTTP_GET, MAIN_WEB_UPLOAD);
		WEB_SERVER.on("/fupload", HTTP_POST, []() { WEB_SERVER.send(200); }, MAIN_HANDLE_FILE_UPLOAD);
		WEB_SERVER.on("/delete", MAIN_WEB_DELETE);
		WEB_SERVER.on("/dir", HTTP_GET, MAIN_WEB_DIR);
		WEB_SERVER.on("/RESET_ESP", []() { WEB_SERVER.send(200); ESP.restart(); });
		WEB_SERVER.onNotFound(MAIN_WEB_NOT_FOUND);

		Enabled_Web_Server = true;

	}

}

//-----------------------------------------
//-----------------------------------------
//-----------------------------------------
void setup()
{
	//-----------------------------------------
	READ_FROM_FILE_CONFIG();		// đọc từ file config từ bộ nhớ SSD rồi ghi ra bộ đệm [buffer_config] để xử lý 
	
	READ_FROM_FILE_INDEX();			// đọc từ file index  từ bố nhớ SSD rồi ghi ra bộ đệm [buffer_index] để xử lý
									// ( [buffer_config] như một mảng trung gian để xử lý dữ liệu mà không ghi đè trực tiếp lên bộ nhớ )
	//-----------------------------------------
	SETUP_PIN();					// cái đặt các chân pin
	SETUP_AT();						// hàm này cài đặt các tham số đầu vào cho hàm SETUP_CONFIG() thông qua [buffer_config]
									// kiểm tra và so sáng các tham số đc truyền vào qua lệnh AT 
									// ghi đè thay thế các tham số từ lệnh AT vào [buffer_config]
									// kết thúc khi gọi lệnh AT+START
	SETUP_CONFIG();					// hàm này đọc các tham số trong [buffer_config] rồi thực thi sau khi kết thúc hàm SETUP_AT() 
	//-----------------------------------------
	WRITE_TO_FILE_CONFIG();			// sau khi hàm setup_config() cài đặt xong các tham số .
									// hàm này ghi lại các tham số mới đc thiết lập từ bộ đệm buffer_config về file config trong bộ nhớ SSD.
									// kết thúc quá trình thao tác đọc / ghi với các tập tin trong bộ nhớ SSD .
	//----------------------------------------

}
//-----------------------------------------
//-----------------------------------------
//-----------------------------------------
void loop()
{
	//-----------------------------------------
	if (Enabled_Socket_Client) SOCKET_CLIENT.loop(); // hàm này chỉ đc bật thông qua key Enabled_xxx có trong hàm SETUP_CONFIG
	if (Enabled_Socket_Server) SOCKET_SERVER.loop();
	if (Enabled_Web_Server) WEB_SERVER.handleClient();

	//-----------------------------------------

	String str;

	if (Serial.available() > 0)
	{
		str = Serial.readString(); str.trim();  s_pln(str);

		detech.D_AT(str); detech.D_CODE(str);

		if (detech.STR_AT[2].equals("HOST")) link_download_file = detech.STR_CODE[0];

		if (detech.STR_AT[2].equals("SHA1")) SHA1 = detech.STR_CODE[0];

	}

	if (str.equals("AT"))
	{
		Serial.println("AT RUNING ...");
		digitalWrite(pinled, HIGH);
		delay(200);
		digitalWrite(pinled, LOW);
	}
	else if (str.equals("AT+RESET"))
	{
		ESP.restart();
	}
	else if (str.equals("AT+CONFIG"))
	{
		s_pln("===========WIFI=============");
		s_p("BaudRate: "); s_pln(buffer_config.Wifi_BaudRate);
		s_p("Channel Wifi: "); s_pln(buffer_config.Wifi_Channel);
		s_p("MODE Wifi : "); s_pln(buffer_config.Wifi_mode);
		s_p("SSID Wifi : "); s_pln(buffer_config.Wifi_ssid);
		s_p("Pass Wifi : "); s_pln(buffer_config.Wifi_pass);
		s_pln("==========SOCKET===========");
		s_p("MODE Socket : "); s_pln(buffer_config.socket_mode);
		s_p("HOST : "); s_pln(buffer_config.socket_host);
		s_p("PORT : "); s_pln(buffer_config.socket_port);
		s_p("URL : "); s_pln(buffer_config.socket_url);
		s_p("WSS : "); s_pln(buffer_config.socket_wss);
		s_pln("==========WEB_SERVER=========");
		s_p("MODE Web : "); s_pln(buffer_config.web_mode);
		s_p("PORT : "); s_pln(buffer_config.web_port);
		s_p("URL : "); s_pln(buffer_config.web_url);

	}
	else if (str.equals("LOGIN"))
	{
		const char* json = "{\"sn\":\"stmsfkiavbn\",\"ac\":\"0399474475\",\"cm\":\"login\",\"rx\":{\"data\":\"\",\"addr\":\"\"}}";
		SOCKET_CLIENT.sendTXT(json);
	}
	else if (str.equals("GET_TIME"))
	{
		const char* json = "{\"sn\":\"stmsfkiavbn\",\"ac\":\"0399474475\",\"cm\":\"gettime\",\"rx\":{\"data\":\"datatx/datarx\",\"addr\":\"server\"}}";
		SOCKET_CLIENT.sendTXT(json);
	}
	else if (str.equals("SEND"))
	{
		const char* json = "{\"sn\":\"stmsfkiavbn\",\"ac\":\"0399474475\",\"cm\":\"message\",\"rx\":{\"data\":\"Hello client\",\"addr\":\"0943952697\"}}";
		SOCKET_CLIENT.sendTXT(json);
	}
	else if (str.equals("STREAM"))
	{
		SOCKET_CLIENT.disconnect();	// phải ngắt kết nối socket rồi mới thực hiện đc HTTPS
	
		MAIN_HTTPS_GET_STREAM(link_download_file, SHA1);

		SOCKET_CLIENT.beginSSL("echo.websocket.org", 443);
	}
	else if (str.equals("DOWN"))
	{
		const char* send = "down";

		SOCKET_CLIENT.sendTXT(send);
	}

	if (Enabled_Download)
	{
		SOCKET_CLIENT.disconnect();

		MAIN_HTTPS_GET_STREAM (link_download_file, SHA1);

		Enabled_Download = false;
	}
}
//-----------------------------------------
//-----------------------------------------
//------------------note-------------------


//-----------------------------------------