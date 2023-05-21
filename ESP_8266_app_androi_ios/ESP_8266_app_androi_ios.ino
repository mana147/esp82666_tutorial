/*
	< Phạm Trung Hiếu >
*/
//-----------------------------------------
#include <ESP8266WiFi.h>
//-----------------------------------------
#include <ESP8266WebServer.h>
//-----------------------------------------
#include <DNSServer.h>
//-----------------------------------------
#include "FS.h"
//-----------------------------------------

ESP8266WebServer server(80);

DNSServer dnsServer;

IPAddress local_IP(1,1,1,1);	//  IP tĩnh muốn gán cho module

IPAddress gateway(192,168,1,100);		//  IP của gateway (thường là router) để kết nối ra mạng bên ngoài

IPAddress subnet(255,255,255,0);		//  subnet xác định phạm vi IP của mạng nội bộ

String request_from_clienst;

//const char *ssid = "Motorbike_SMARTKEY";          // ten wifi phát ra

const char *ssid = "SMARTKEY";          // ten wifi phát ra

const char *password = "123456789";

const char *HostName = "khoavantayxemay.com";

const char* htmlfile = "/web.html";

const char* jsonFile = "/json1.json";

const char* key_In_html = "esp8266";

const char* request_functionConnect = "/";

const char* request_function_ADD = "/ADD";

const char* request_function_DELETE = "/DLETE";

const byte DNS_PORT = 53;

//-------------------------------------------------
//-------------------------------------------------

String read_files_in_SPIFFS(String string)
{
	String s;
	File data_files;
	data_files = SPIFFS.open(string, "r");
	if (data_files)
	{
		while (data_files.available())
		{
			s = data_files.readString();
			data_files.flush();        // giải phóng bộ đệm
		}
		data_files.close(); // không có hoạt động nào khác trên đối tượng Files sau khi chức năng này được đóng 
	}
	else
	{

	}
	return s;
}

int write_files_in_SPIFFS(String string, String path)
{
	char key;
	String s;
	File data_flies;
	Dir dir;

	data_flies = SPIFFS.open(path, "w");

	//dir = SPIFFS.openDir(path);

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

//--------------------------------------------------

void action_connect()
{
	//String str_html = "connecting"
	String str_html = read_files_in_SPIFFS(jsonFile);
	Serial.println(str_html);
	server.send(200, "text/html", str_html);
}

void action_ADD()
{
	String name_request = server.arg("name");

	if (name_request.length() == 0) {
		name_request = "NULL";
	}

	Serial.println(name_request);
	server.send(200, "text/html", name_request);
}

void action_DELETE()
{
	String name_request = server.arg("name");

	if (name_request.length() == 0) {
		name_request = "NULL";
	}

	Serial.println(name_request);
	server.send(200, "text/html", name_request);
}

//-------------------------------------------------
//-------------------------------------------------
void setup()
{
	Serial.begin(115200);
	SPIFFS.begin();
	server.begin();

	//---------------------------------------------

	WiFi.softAPConfig(local_IP, gateway, subnet);
	WiFi.softAP(ssid, password);

	//---------------------------------------------

	dnsServer.setTTL(10);
	dnsServer.setErrorReplyCode(DNSReplyCode::ServerFailure);
	dnsServer.start(DNS_PORT, HostName, local_IP);

	//---------------------------------------------

	server.on(request_functionConnect, action_connect);
	server.on(request_function_ADD, action_ADD);
	server.on(request_function_DELETE, action_DELETE);

	//---------------------------------------------
	//---------------------------------------------
	String str_html = read_files_in_SPIFFS(jsonFile);
	Serial.println(str_html);

}

void loop()
{
	//-----------------------------------
	dnsServer.processNextRequest();
	server.handleClient();
	//-----------------------------------

	delay(1);
}