/*
lap trinh phia ( Access poirt )
*/
// -------- khai bao thu vien --------
#include <ESP8266WiFi.h>

#define DHT_ER  0
#define DHT_OK  1
#define DHT_DATA  13

String cm_get_temp = "get_temp\r";
String cm_get_humi = "get_humi\r";
String cm_get_npkt = "get_npkt\r";
String cm_get_doph = "get_doph\r";


String Message;

//#define max 6;	 // MAXIMUM NUMBER OF CLIENTS

//------------------------------------
const char *ssid = "MODE_AC" ;          // ten wifi phát ra
const char *password = "123456789" ;    // mật khâu wifi phat ra ,

const char* ssid2 = "Room.Vip___";
const char* password2 = "123456788";

//----------------------------------

uint8_t macAddr[6];  // khai báo địa chỉ MAC là 1 mảng gồm 6 phần tử 
uint8_t idAddr[5][6];

IPAddress local_IP(192, 168, 4, 22); //  IP tĩnh muốn gán cho module
IPAddress gateway(192, 168, 4, 9);   //  IP của gateway (thường là router) để kết nối ra mạng bên ngoài
IPAddress subnet(255, 255, 255, 0);  //   subnet xác định phạm vi IP của mạng nội bộ


WiFiServer Server_AP(23);   // TCP	UDP	ETL Service Manager

WiFiServer server(80);

WiFiClient Client_STA[6];

//WiFiClient Client_web;

//=================================================================
void Setup_wifi()
{
	// ------------------------------------------

	WiFi.disconnect(); // stop wifi previous

	WiFi.softAPConfig(local_IP, gateway, subnet); // config cứng moden wifi

	WiFi.begin(ssid2, password2);

	while (WiFi.status() != WL_CONNECTED) 
	{
		delay(500);
		Serial.print(".");
	}

	Serial.println("");
	Serial.println("Da ket noi WiFi ");

	// Khoi dong web server
	server.begin();
	Serial.println("Khoi dong web Server ");

	// In ra dia chi IP
	Serial.println(WiFi.localIP());

	WiFi.mode(WIFI_AP_STA);

	WiFi.softAP(ssid, password);

	// ------------------------------------------
	Serial.println();
	Serial.printf(" khoi dong phat WiFi %s \n", ssid);

	//-------  get add MAC for ESP ----------
	Serial.printf("MAC address = %s\n", WiFi.softAPmacAddress().c_str());

	// -------- Khoi dong server -------------

	Server_AP.begin();
	//Server_AP.setNoDelay(true);
	Serial.println("Khoi dong Server AP");
	Serial.print(" Local IP : ");
	Serial.println(WiFi.softAPIP()); // lấy địa chỉ IP của thiết bị

    //---------------------------------------


}

void Get_data(void)
{
	if (Server_AP.hasClient())
	{
		for (uint8_t i = 0; i < 1; i++)
		{
			//find free/disconnected spot
			if (!Client_STA[i] || !Client_STA[i].connected())
			{
				// Checks If Previously The Client Is Taken
				if (Client_STA[i])
				{
					Client_STA[i].stop();
				}

				// Checks If Clients Connected To The Server
				if (Client_STA[i] = Server_AP.available())
				{
					Serial.println("New Client: " + String(i));
				}

				// Continue Scanning
				continue;
			}
		}

		WiFiClient Client_STA = Server_AP.available();
		Client_STA.stop();

	}


	for (uint8_t i = 0; i < 1; i++)
	{
		if (Client_STA[i] && Client_STA[i].connected() && Client_STA[i].available())
		{
			while (Client_STA[i].available())
			{
			    Message = Client_STA[i].readStringUntil('\r');
				Client_STA[i].flush();
				Serial.println("Client No " + String(i) + " - " + Message);
				Serial.println();
			}
		}
	}

}

void get_dulieu(String str)
{
	Client_STA[0].print(str);
	delay(50);
	Get_data();
}

//=========================== SET UP ==============================

void setup(void)
{
	Serial.begin(115200);
	Setup_wifi();
}


//=========================== LOOP ==============================

void loop()
{
	// Kiem tra khi co client ket noi
	WiFiClient client = server.available();
	if (!client)
	{
		return;
	}

	// Doi client gui ket noi
	Serial.println("Co mot client moi ket noi xem du lieu");
	while (!client.available())
	{
		delay(1);
	}
	
	// Doc dong dau tien cua yeu cau gui len.
	String req = client.readStringUntil('\r');
	Serial.println(req);
	client.flush();

	// Chuan bi tao web de phan hoi
	String s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
	s += "<head>";
	s += "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">";
	s += "<meta http-equiv=\"refresh\" content=\"60\" />";
	s += "<script src=\"https://code.jquery.com/jquery-2.1.3.min.js\"></script>";
	s += "<link rel=\"stylesheet\" href=\"https://maxcdn.bootstrapcdn.com/bootstrap/3.3.4/css/bootstrap.min.css\">";
	s += "<style>body{font-size: 24px;} .voffset {margin-top: 30px;}</style>";
	s += "</head>";

	s += "<div class=\"container\">";
	s += "<h1>Theo doi nhiet do va do am</h1>";
	s += "<div class=\"row voffset\">";

	get_dulieu(cm_get_temp);

	s += "<div class=\"col-md-3\"> </div><div class=\"col-md-3\">" + Message + "</div>";

	get_dulieu(cm_get_humi);

	s += "<div class=\"col-md-3\"> </div><div class=\"col-md-3\">" + Message + "</div>";
	s += "</div>";

	// Gui phan hoi toi client (o day la giao dien web)
	client.print(s);
	delay(1);
	Serial.println("Client da thoat");

	// Sau khi nhan duoc thong tin thi se tu dong ngat ket noi 

}



