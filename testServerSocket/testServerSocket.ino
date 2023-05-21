/*
Name:    WebSocket_esp8266_0.ino
Created:  26-03-2019 2:59:57 AM
Author:    Phạm Trung Hiếu
Note:   Web nhúng trên esp8266 v12
code web được tách riêng khỏi hệ thống , và lưu trên 1 phần vùng bộ nhớ riêng
trong quá trình chạy web được gọi ra thông qua thư viên FS.h và đẩy toàn bộ code lên phía client .
Phân vùng bộ nhớ SPIFFS , mã nguồn và tài liệu liên quan cập nhật tại
*/
//-----------------------------------------
#include <ESP8266WiFi.h>
//-----------------------------------------
#include <ESP8266WebServer.h>
//-----------------------------------------
#include <WebSocketsServer.h>
//-----------------------------------------
#include <DNSServer.h>
//-----------------------------------------
#include "FS.h"
//-----------------------------------------
//-----------------------------------------

ESP8266WebServer server(80);
WebSocketsServer webSocket(81);


char* ssid = "Hoa Ban T1";
char* password = "caphephamay";
char* htmlfile = "/index.html";

//-----------------------------------------
//-----------------------------------------

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

//-----------------------------------------
//-----------------------------------------

void load_web()
{
  String str_html = read_files_in_SPIFFS(htmlfile);
  server.send(200, "text/html", str_html);
}

void readUART_and_sendESP()
{
  if (Serial.available() > 0)
  {
    char c[] = { (char)Serial.read() }; // doc ky tu
    webSocket.broadcastTXT(c, sizeof(c)); // gui tra socket
  }
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) // khi nhận đc tin nhắn từ websocket
{
	switch (type) // WStype_DISCONNECTED // WStype_CONNECTED
	{
		case WStype_TEXT:
		{
			Serial.printf("[%u] get Text for esp8266: %s\n", num, payload);

    		if (payload[0] == '#')
    		{
     			 uint16_t get_range = (uint16_t) strtol( (char*)&payload[1] , NULL , 10 );
     			 Serial.println (get_range);
      			 analogWrite(D0,get_range);
    		}
		}
	}
}


//-----------------------------------------
//-----------------------------------------
void setup()
{
  Serial.begin(115200);
  SPIFFS.begin();
  server.begin();
  webSocket.begin();
  WiFi.begin(ssid, password);
  //-----------------------------------

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("IP Address: ");
  Serial.println(WiFi.localIP());

  server.on("/", load_web);
  webSocket.onEvent(webSocketEvent);
}

void loop()
{
  server.handleClient();
  webSocket.loop();
  readUART_and_sendESP();
}
