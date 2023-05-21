/*
lap trinh phia ( Access poirt )
*/
#define BLYNK_PRINT Serial

#include <BlynkSimpleEsp8266.h>
#include <ESP8266WiFi.h> 
#include "SimpleTimer-master\SimpleTimer.h"


//--------------------------------------

#define DHT_ER  0
#define DHT_OK  1
#define DHT_DATA  0

byte HNhietDo, LNhietDo, HDoAm, LDoAm;

byte DHT_GetTemHumi(byte &tem1, byte &tem2, byte &humi1, byte &humi2);

//--------------------------------------

String cm_get_temp = "get_temp\r";
String cm_get_humi = "get_humi\r";
String cm_get_solid = "get_solid\r";

String Message;

char do_am[2] = { '0','0' };
char nhiet_do[2] = { '0','0' };
char solid[2] = { '0','0' };

SimpleTimer time_1, time_2;

//#define max 6;	                             // MAXIMUM NUMBER OF CLIENTS

//------------------------------------	   

const char *ssid1 = "MODE_AC";                   // ten wifi phát ra
const char *pass1 = "123456789";                 // mật khâu wifi phat ra ,

char auth[] = "505412bb03764f6da6282d03d086d48c";    //AuthToken copy ở Blynk Project	
													 //char auth[] = "7f6558273ab34460a80e9493f6f2cc1e";
char ssid[] = "WIFI_TANG1";                       //Tên wifi
char pass[] = "08031993";                        //Mật khẩu wifi

//char ssid[] = "DUC-HANG";                        //Tên wifi
//char pass[] = "123456789";

uint8_t macAddr[6];                             // khai báo địa chỉ MAC là 1 mảng gồm 6 phần tử 
uint8_t idAddr[5][6];

IPAddress local_IP(192, 168, 4, 22);            //  IP tĩnh muốn gán cho module
IPAddress gateway(192, 168, 4, 9);              //  IP của gateway (thường là router) để kết nối ra mạng bên ngoài
IPAddress subnet(255, 255, 255, 0);             //   subnet xác định phạm vi IP của mạng nội bộ

WiFiServer Server_AP(23);                       // TCP	UDP	ETL Service Manager

WiFiServer server(80);

WiFiClient Client_STA[1];

WidgetLCD lcd_1(V4);
WidgetLCD lcd_2(V5);
WidgetLCD lcd_3(V6);

//-----------------------------------------

int value_adc;
int d_value_adc;

//==========================================================

void Setup_wifi()
{
	// ------------------------------------------

	Blynk.begin(auth, ssid, pass);

	WiFi.softAP(ssid1, pass1);

	WiFi.softAPConfig(local_IP, gateway, subnet); // config cứng moden wifi

	Server_AP.begin();

		Serial.println("Khoi dong Server AP");

		Serial.print(" Local IP : ");

		Serial.println(WiFi.softAPIP()); // lấy địa chỉ IP của thiết bị

}

//----------------------------------------

void send_cm_to_client(String str)
{
	Client_STA[0].print(str);
}

void read_data_from_client(void)
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

void xu_ly_string(String data)
{
	unsigned char i = 0;

	while (data[i] != '*')
	{
		if (++i == 20)
			break;

	}

	if (data[i + 1] == 'C')
	{
		if (data[i - 2] > 47 && data[i - 2] < 58)
		{
			nhiet_do[0] = data[i - 2];
			nhiet_do[1] = data[i - 1];
		}
	}

	if (data[i + 1] == 'R')
	{
		if (data[i - 2] > 47 && data[i - 2] < 58)
		{
			do_am[0] = data[i - 2];
			do_am[1] = data[i - 1];
		}
	}

	if (data[i + 1] == 'D')
	{
		if (data[i - 2] > 47 && data[i - 2] < 58)
		{
			solid[0] = data[i - 2];
			solid[1] = data[i - 1];
		}
	}
}

//-----------------------------------------

byte DHT_GetTemHumi(byte &tem1, byte &tem2, byte &humi1, byte &humi2)
{
	byte buffer[5] = { 0,0,0,0,0 };
	byte ii, i, checksum;

	pinMode(DHT_DATA, OUTPUT);//set la cong ra
	digitalWrite(DHT_DATA, HIGH);//binh thuong khi chua giao tiep voi cam bien dht11 thi chan tin hieu lun o muc cao
	delayMicroseconds(60);//cho doi tam 60 micro giay
	digitalWrite(DHT_DATA, LOW);//gui tin hieu bat dau cho cam bien, va mat tam it nhat la 18ms
	delay(25);// it nhat 18ms, de bao dam rang DHT phat hien duoc tin hieu bat dau nay
	digitalWrite(DHT_DATA, HIGH);//sau do MCU se keo tin hieu nay len cao va cho tu 20us-40us de DHT phan hoi lai    
	pinMode(DHT_DATA, INPUT);//set la cong vao
	delayMicroseconds(60);//cho doi tam 60 micro giay
	if (digitalRead(DHT_DATA) == 1) return DHT_ER;//neu tin hieu nay van o muc cao nghia la DHT khong phan hoi thi bao loi
	else
	{
		while (digitalRead(DHT_DATA) == 0);//sau khi DHT11 phan hoi thi no se keo tin hieu nay ve muc thap tam 80us
	}
	delayMicroseconds(60);//sau do DHT11 lai chuyen tin hieu nay tu muc thap len muc cao tam 80us de chuan bi bat dau gui du lieu cho MCU
	if (digitalRead(DHT_DATA) == 0) return DHT_ER;//neu duong tin hieu van o muc cao sau 80us thi bao loi
	else
	{
		while (digitalRead(DHT_DATA) == 1);//cho duong tin hieu nay ve muc thap de bat dau gui du lieu cho MCU
	}

	for (i = 0;i<5;i++)
	{
		for (ii = 0;ii<8;ii++)
		{
			while (digitalRead(DHT_DATA) == 0);//doi data len 1
			delayMicroseconds(50);//kiem tra do dai bit 0 hay la 1
			if (digitalRead(DHT_DATA) == 1)
			{
				buffer[i] |= (1 << (7 - ii));
				while (digitalRead(DHT_DATA) == 1);//doi data xuong 0
			}
		}
	}
	checksum = buffer[0] + buffer[1] + buffer[2] + buffer[3];
	if ((checksum) != buffer[4])return DHT_ER;
	tem1 = buffer[2];//gia tri phan nguyen
	tem2 = buffer[3];//gia tri phan thap phan
	humi1 = buffer[0];//gia tri phan nguyen
	humi2 = buffer[1];//gia tri phan thap phan
	return DHT_OK;
}

//-----------------------------------------

void send_read_xl()
{
	send_cm_to_client(cm_get_solid);
		delay(500);
		read_data_from_client();
		xu_ly_string(Message);

	send_cm_to_client(cm_get_temp);
		delay(500);
		read_data_from_client();
		xu_ly_string(Message);

	send_cm_to_client(cm_get_humi);
		delay(500);
		read_data_from_client();
		xu_ly_string(Message);
}

void sen_data_to_LCD_1()
{
	lcd_1.clear();
	lcd_1.print(0, 0, "T1 H   T    V");
	lcd_1.print(3, 1, HNhietDo);
	lcd_1.print(7, 1, HDoAm);
	lcd_1.print(11, 1, d_value_adc);
}

void sen_data_to_LCD_2()
{
	lcd_2.clear();
	lcd_2.print(0, 0, "T2 H   T    V");
	lcd_2.print(3, 1, nhiet_do);
	lcd_2.print(7, 1, do_am);
	lcd_2.print(11, 1, solid);
}


//========== SET UP ===========.

void setup(void)
{
	Serial.begin(9600);

	Setup_wifi();

	time_1.setInterval(1000L, send_read_xl);

	time_1.setInterval(1000L, sen_data_to_LCD_2);

	time_1.setInterval(1000l, sen_data_to_LCD_1);

}

//========== LOOP ============

void loop()
{
	Blynk.run();

	//------------------------------------------

	DHT_GetTemHumi(HNhietDo, LNhietDo, HDoAm, LDoAm);

	value_adc = analogRead(A0);
		d_value_adc = (value_adc * 100) / 1024;

	//-------------------------------------------

	time_1.run();

}



