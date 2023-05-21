/*
 Name:		Socket_Server_esp8266_0.ino
 Created:	26-03-2019 2:59:57 AM
 Author:    Phạm Trung Hiếu 
 Note :		Socket server esp8266
*/
//-----------------------------------------
//-----------------------------------------
#include <ESP8266WiFi.h>
//-----------------------------------------
#include <ESP8266WebServer.h>
//-----------------------------------------
#include <WebSocketsServer.h>
//-----------------------------------------
#include <ArduinoJson.h>
//-----------------------------------------
#include "FS.h"
//-----------------------------------------
#include <Hash.h>

ESP8266WebServer server(8080);

WebSocketsServer webSocket = WebSocketsServer(81);

//-----------------------------------------

IPAddress staticIP(192, 168, 1, 22);

IPAddress gateway(192, 168, 1, 9);

IPAddress subnet(255, 255, 255, 0);

//-----------------------------------------

const char* ssid_wifi = "Wifi tang 2.";

const char* password_wifi = "080393itc";

//const char *ssid = "SMARTKEY";

const char *ssid = "khoavantayxemay.com";

const char *password = "123456789";

//const char *htmlfile = "/index.html";

const char *htmlfile = "/web.html";

const char *add_dataJson = "/data.json";

const char *jsonkeyconfig = "/key_config.json";

//--------------------------------------------

const char *AUTH_KEY	= "esp01234567890";

const char *RF_KEY		= "esp0123456789";

const char *ID_KEY		= "esp0123456789";

const char *CHECK_KEY	= "check_key";

const char *SETTING_KEY		= "setting_key";

const char *SETTING_USER	= "setting_user";

const char *SETTING_BEEP	= "beep";

const char *CLASS_OWNER		= "owner";

const char *CLASS_CUSTOMER	= "customer";

const char *TYPE_ADD		= "add";

const char *TYPE_DELETE		= "delete";

const char *GET_ALL_USER	= "get_allUser";

const int pinled = D0;

const int pinbut_D1 = D1;

const int pinbut_D2 = D2;

int key = 0;

bool setting_fingerprint = false ;

bool key_beep = false;

//------------------------------------------------

uint8 num_c = NULL;

String str_html , str_data_json_in_SSD , str_data_json_in_payload;

String String_data_owner_ssd[10];

String String_data_customer_ssd[10];

//---------------------------------------------------------

 typedef struct
{
	 String request_json;
	 String	key_json;
	 String auth_key_json;
	 String	rf_key_json;
	 String id_key_json;
	 String classify_json;
	 String type_json;
	 String name_json;
	 int index_json;
} Get_data;

 Get_data GetDataJson;

 //--------------------------------------------------------

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
	 else
	 {

	 }

	 return s;
 }

 char read_files_in_SPIFFS_char(const char * add)
 {
	 File data_files = SPIFFS.open(add, "r");

	 size_t size = data_files.size();

	 std::unique_ptr<char[]> buf(new char[size]);

	 if (data_files)
	 {
		 while (data_files.available())
		 {
			 data_files.readBytes(buf.get(), size);
			 data_files.flush();        // giải phóng bộ đệm
		 }

		 data_files.close(); // không có hoạt động nào khác trên đối tượng Files sau khi chức năng này được đóng 
	 }

 }

 void check_free_ramm()
 {
	 uint32_t free = system_get_free_heap_size();
	 Serial.println(free);
 }

 //--------------------------------------------------------

void readUART_and_sendESP()
{
	if (Serial.available() > 0)
	{
		char c[] = {(char)Serial.read() }; // doc ky tu
		webSocket.broadcastTXT(c, sizeof(c)); // gui tra socket
	}
}

void ReadUART_Sending_Socket()
{
	if (Serial.available() > 0)
	{
		String s = Serial.readString();
		if (s.equals("ok"))
		{

		}
		if (s.equals("not"))
		{

		}
	}
}

void load_web() // gửi cho client 1 web socket để làm socket clien . kết nối đến esp
{
	server.send(200, "text/html", str_html);
	Serial.println(str_html);
}

//----------------------------------------------------------

void Get_old_list_from_SPIFFS ( const char *add , String OWNER[10] , String CUSTOMER[10] )
{
	//----------------------------------------------------------
	File data_files = SPIFFS.open(add, "r");
	size_t size = data_files.size();
	std::unique_ptr <char[]> buf(new char[size]);
	if (data_files)
	{
		while (data_files.available())
		{
			data_files.readBytes(buf.get(), size);
			data_files.flush();        // giải phóng bộ đệm
		}
		data_files.close(); // không có hoạt động nào khác trên đối tượng Files sau khi chức năng này được đóng 
	}
	//----------------------------------------------------------

	DynamicJsonDocument doc(2000);
	deserializeJson(doc, buf.get());

	const char* owner[10];
	const char* customer[10];

	String index[10] = {"0","1","2","3","4","5","6","7","8","9"};

	for (int i = 0; i < 10; i++)
	{
		owner[i] = doc["owner"][index[i]];
			OWNER[i] = owner[i];
		customer[i] = doc["customer"][index[i]];
			CUSTOMER[i] = customer[i];
	}

	//---------------------------------------------------------
}

void Saving_new_list_to_SPIFFS ( const char *add , String OWNER[10] , String CUSTOMER[10] )
{
	String index[10] = { "0","1","2","3","4","5","6","7","8","9" };

	DynamicJsonDocument doc(2000);

	doc["request"] = "get_allUser";
	doc["status"] = "success";

	JsonObject owner = doc.createNestedObject("owner");

	for (int i = 0; i < 10; i++)
	{
		owner[index[i]] = OWNER[i];
	}

	JsonObject customer = doc.createNestedObject("customer");

	for (int i = 0; i < 10; i++)
	{
		customer[index[i]] = CUSTOMER[i];
	}

	//---------------------------------------------------------

	File data_flies = SPIFFS.open(add, "w");

	if (data_flies)
	{
		serializeJsonPretty(doc, data_flies);
		data_flies.flush();
	}

	data_flies.close();

	//---------------------------------------------------------
}

void Save_Name_Array (String Owner[10] , String Customer[10], String Class , String Type, int Number_stt , String Name_acc)
{
		if (Class.equals("owner"))
		{
			if (Type.equals("add"))
			{
				Owner[Number_stt] = Name_acc;
			}
			if (Type.equals("delete"))
			{
				Owner[Number_stt] = "";
			}
		}
		//----------------------------------------------
		if (Class.equals("customer"))
		{
			if (Type.equals("add"))
			{
				Customer[Number_stt] = Name_acc;
			}
			if (Type.equals("delete"))
			{
				Customer[Number_stt] = "";
			}
		}
	
}

//----------------------------------------------------------

void Function_Handle_Get_Data_in_Payload(uint8_t channel, uint8_t*payload)
{
	DynamicJsonDocument doc(1024);
	Get_data*getdata = &GetDataJson;
	//---------------------------------------
		deserializeJson(doc, payload);

		const char* request			= doc["request"]; // "setting_key"
		const char* key				= doc["key"]; // "authKey"

		const char* auth_key		= doc["auth_key"]; // "esp012345789"
		const char* rf_key			= doc["rf_key"]; // "esp012345789"
		const char* id_key			= doc["id_key"]; // "esp012345789"

		const char* classify		= doc["classify"]; // "owner"
		const char* type			= doc["type"]; // "add"
		const char* name			= doc["name"]; // "name"
		int index					= doc["index"]; // 1

	//----------------------------------------

		getdata->request_json	= request;
		getdata->key_json		= key;

		getdata->auth_key_json	= auth_key;
		getdata->rf_key_json	= rf_key;
		getdata->id_key_json	= id_key;

		getdata->classify_json	= classify;
		getdata->type_json		= type;
		getdata->name_json		= name;
		getdata->index_json		= index;

	//----------------------------------------

}

void Function_Handle_Check_Key(uint8_t channel)
{
	if (GetDataJson.request_json.equals(CHECK_KEY))
	{
		if (GetDataJson.key_json.equals(AUTH_KEY))
		{
			const char* json = "{\"request\":\"check_key\",\"status\":\"success\"}";
			//webSocket.sendTXT(channel, json);
			webSocket.broadcastTXT(json);
		}
		else
		{
			const char* json = "{\"request\":\"check_key\",\"status\":\"error\",\"code\":\"err99\"}";
			//webSocket.sendTXT(channel, json);
			webSocket.broadcastTXT(json);
		}
	}
	
}
	
void Function_Handle_Input_Key(uint8_t channel)
{
	if (GetDataJson.request_json.equals(SETTING_KEY))
	{
		String t;
		DynamicJsonDocument doc(200);

		doc["request"] = "setting_key";
		doc["status"] = "success";

		if (GetDataJson.auth_key_json.equals(AUTH_KEY)) {
			doc["auth_key"] = true;
		}
		else {
			doc["auth_key"] = false;
		}

		if (GetDataJson.id_key_json.equals(ID_KEY)) {
			doc["rf_key"] = true;
		}
		else {
			doc["rf_key"] = false;
		}

		if (GetDataJson.rf_key_json.equals(RF_KEY)) {
			doc["id_key"] = true;
		}
		else {
			doc["id_key"] = false;
		}

		serializeJson(doc, t);
		//webSocket.sendTXT(channel, t);
		webSocket.broadcastTXT(t);
	}
}

void Function_Handle_Setting_User(uint8_t channel)
{
	if (GetDataJson.request_json.equals(SETTING_USER))
	{
		if (GetDataJson.key_json.equals(AUTH_KEY))
		{
			if (GetDataJson.type_json.equals("add"))
			{
				const char* json_user = "{\"request\":\"setting_user\",\"status\":\"success\"}";
				webSocket.broadcastTXT(json_user);
			}
			if (GetDataJson.type_json.equals("delete"))
			{
				const char* json = "{\"request\":\"setting_user\",\"status\":\"success\"}";
				webSocket.broadcastTXT(json);
				setting_fingerprint = true;
			}

		} else {
			const char* json = "{\"request\":\"setting_user\",\"status\":\"error\",\"code\":\"err99\"}";
			//webSocket.sendTXT(channel, json);
			webSocket.broadcastTXT(json);
			setting_fingerprint = false ;
		}
	}
}

void Function_Handle_ListUserName(uint8_t channel)
{
	if (GetDataJson.request_json.equals(GET_ALL_USER))
	{
		if (GetDataJson.key_json.equals(AUTH_KEY))
		{	
			str_data_json_in_SSD = read_files_in_SPIFFS_String(add_dataJson);
			webSocket.broadcastTXT(str_data_json_in_SSD);
			Serial.println(str_data_json_in_SSD);
		} else {
			//const char* json = "{\"request\":\"get_allUser\",\"status\":\"error\",\"code\":\"err99\"}";
			const char* json = "{\"request\":\"get_allUser\",\"status\":\"error\",\"code\":\"err01\",\"message\":\"sai key authen\"}";
			webSocket.broadcastTXT(json);
		}
	}
}

void Function_Handle_BEEP(uint8_t channel)
{
	if (GetDataJson.request_json.equals(SETTING_BEEP))
	{
		if (GetDataJson.key_json.equals(AUTH_KEY))
		{
			const char* json = "{\"request\":\"beep\",\"status\":\"success\"}";
			//webSocket.sendTXT(channel, json);
			webSocket.broadcastTXT(json);
			key_beep = true;
		}
		else {
			const char* json = "{\"request\":\"beep\",\"status\":\"error\",\"code\":\"err99\"}";
			//webSocket.sendTXT(channel, json);
			webSocket.broadcastTXT(json);
		}
	}
}

void Send_Data_To_STM32(String str[6] )
{
	String s[100];

	String k[6] = 
	{
		":",
		"=",
		"\"",
		",",
		"!",
	};
	
	s[0] = ":";
	s[1] = str[0];
	s[2] = "=";

	s[3] = "\"";
	s[4] = str[1];
	s[5] = "\"";
	s[6] = ",";

	s[7] = "\"";
	s[8] = str[2];
	s[9] = "\"";
	s[10] = ",";

	String send = 
		":" + str[0] + "="
		"\"" + str[1] + "\"" ","
		"\"" + str[2] + "\"" "," 
		"\"" + str[3] + "\"" ","
		"\"" + str[4] + "\"" ","
		"!";

	Serial.println(send);
}
//-------------------------------------------------------

void MAIN_SOCKET_EVENT(uint8_t channel , WStype_t type , uint8_t*payload , size_t length) // channel : kenh , type : trang thai , payload : data, string
{
  switch (type) // chuyển trạng thái hoạt động 
  {
	//---------------------------------------------------
	case WStype_DISCONNECTED:	// nếu WebS ngắt kết nối 
	{
		Serial.printf(" Disconnected %u \n ", channel);
		Serial.println(webSocket.remoteIP(channel));
		break;	
	}
	//------------------------------------------------
	case WStype_CONNECTED:		// nếu có kết nối mới 
	{
		Serial.printf("Connection %u \n", channel); 
		int ip = webSocket.remoteIP(channel);
		Serial.println(ip);

		webSocket.broadcastTXT("ok da ket noi");
		num_c = channel;
		break;
	}
	//------------------------------------------------
	case WStype_TEXT:			// nếu data text mới đc received
	{
		String str = (char*)payload; // kiểm tra chuỗi , fix bug chuỗi NULL gây reset esp 

		Serial.println(str);

		if (str.length() > 0)
		{
			//Serial.printf("channel: [%u] \n", channel);
			Function_Handle_Get_Data_in_Payload(channel, payload);
			Function_Handle_Check_Key(channel);
			Function_Handle_Input_Key(channel);
			Function_Handle_Setting_User(channel);
			Function_Handle_ListUserName(channel);
			Function_Handle_BEEP(channel);
			//webSocket.disconnect();
		}
		break;
	}
  }
}

//-------------------------------------------------------

void SETUP_WIFI_ACCESS_POINT()
{
	Serial.begin(115200);
	SPIFFS.begin();
	WiFi.mode(WIFI_AP);	
	//---------------------------------------------
	//WiFi.softAPConfig(local_IP, gateway, subnet);
	WiFi.softAP(ssid, password, 4, 0, 8); // 192.168.4.1
	Serial.println(WiFi.softAPIP());
	//---------------------------------------------
}

void SETUP_WIFI_STATION()
{
	Serial.begin(115200);
	SPIFFS.begin();
	WiFi.mode(WIFI_STA);
	//-----------------------------------------------
	WiFi.begin(ssid_wifi, password_wifi);             // Connect to the network
	Serial.print("Connecting to ");
	Serial.print(ssid_wifi);
	int i = 0;
	if (WiFi.status() != WL_CONNECTED) { // Wait for the Wi-Fi to connect
		delay(1000);
		Serial.print(++i); 
		Serial.print(' ');
	}
	Serial.print("IP address:\t");
	Serial.println(WiFi.localIP());

}

void SETUP_SERVER_SOCKET()
{
	webSocket.begin();
	webSocket.onEvent(MAIN_SOCKET_EVENT);
	//---------------------------------------------
	server.on("/", load_web);
	server.begin();
}

void SETUP_PIN_MODEN()
{
	pinMode(pinled, OUTPUT);
	pinMode(pinbut_D1, INPUT_PULLUP);
	pinMode(pinbut_D2, INPUT_PULLUP);
	digitalWrite(pinled,HIGH);
}


//-------------------------------------------------------

void setup() 
{
	SETUP_PIN_MODEN();
	//SETUP_WIFI_STATION();
	SETUP_WIFI_ACCESS_POINT();
	SETUP_SERVER_SOCKET();
	//--------------------------------------------------
	str_html = read_files_in_SPIFFS_String(htmlfile);
	//--------------------------------------------------
}

//-------------------------------------------------------

void loop() 
{
	webSocket.loop();

	server.handleClient();

	if (setting_fingerprint )
	{
		Get_old_list_from_SPIFFS (
			add_dataJson, 
			String_data_owner_ssd, 
			String_data_customer_ssd
		);

		Save_Name_Array (
			String_data_owner_ssd,
			String_data_customer_ssd,
			GetDataJson.classify_json,
			GetDataJson.type_json,
			GetDataJson.index_json,
			GetDataJson.name_json
		);

		Saving_new_list_to_SPIFFS (
			add_dataJson,
			String_data_owner_ssd,
			String_data_customer_ssd
		);

		setting_fingerprint = false; 
	}

	if (key_beep)
	{
		digitalWrite(pinled, LOW);
		delay(50);
		digitalWrite(pinled, HIGH);
		key_beep = false;
	}

	if (digitalRead(pinbut_D1) == 0)
	{
		delay(300);

		const char* json = "{\"request\":\"setting_fingerprint\",\"status\":\"success\"}";

		//webSocket.broadcastTXT(json);

		webSocket.sendTXT(num_c,json);

		setting_fingerprint = true;

		Serial.println("setting_fingerprint_success");
	}

	if (digitalRead(pinbut_D2) == 0)
	{
		delay(300);

		const char* json = "{\"request\":\"setting_fingerprint \",\"status\":\"error\",\"code\":\"err01\",\"message\":\"nhập lại dấu vân tay \"}";

		//webSocket.broadcastTXT(json);

		webSocket.sendTXT(num_c, json);

		Serial.println("setting_fingerprint_error");

	}
}
//-------------------------------------------------------
//-------------------------------------------------------