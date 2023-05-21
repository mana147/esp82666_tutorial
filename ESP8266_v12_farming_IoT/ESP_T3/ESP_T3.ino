#define BLYNK_PRINT Serial

#include <ESP8266WiFi.h>
#include <EEPROM.h>
#include <BlynkSimpleEsp8266.h>
#include <SimpleTimer-master\SimpleTimer.h>
#include <WidgetRTC.h>
#include <TimeLib.h>


//------------------------------------------
#define _value_nhiet_do   V0
#define _value_do_am      V1
#define _value_do_am_dat  V2 

#define _relay_1          V3
#define _relay_2          V4
#define _mofet            V5

#define _auto_relay_1     V6
#define _auto_relay_2     V7
#define _auto_mofet       V8

#define _control_temperature_relay V9
#define _control_temperature_mofet V17

#define _time_relay_1    V12
#define _time_relay_2    V13
#define _time_mofet      V14

#define _set_temperature_relay V15
#define _set_temperature_mofet V16

//------------------------------------------

#define DHT_ER  0
#define DHT_OK  1
#define DHT_DATA  13

byte HNhietDo, LNhietDo, HDoAm, LDoAm;

byte DHT_GetTemHumi(byte &tem1, byte &tem2, byte &humi1, byte &humi2);

WidgetRTC rtc;

SimpleTimer timer;

//------------------------------------------
String Time_RTC;
String Date_RTC;

IPAddress local_IP(192,168,1,1);  //  IP tĩnh muốn gán cho module
IPAddress gateway(192, 168, 4, 9);    //  IP của gateway (thường là router) để kết nối ra mạng bên ngoài
IPAddress subnet(255, 255, 255, 0);   //  subnet xác định phạm vi IP của mạng nội bộ

const char *ssid = "MODE_CONFIG_ESP8266_V12";          // ten wifi phát ra
const char *password = "123456789";     // mật khâu wifi phat ra ,

char *id = "Room @ Vip";
char *pass = "tdt@12345";
char *au = "11a5ad7e9439485a9202504d6e7059db";

char PIN_BUTTON = 12;
char PIN_LED_MODE_CONFIG = 14;
char PIN_LED = 2;
char PIN_RELAY_4 = 4;
char PIN_RELAY_5 = 5;
char PIN_MOFET_16 = 16;

//char PIN_LED_MODE_MAIN = 12;

signed char SSID_DATA[40];
signed char PASS_DATA[40];
signed char AUTH_DATA[40];

unsigned char _key = 0 ;

char Date[16];
char Time[16];

int add = 0;
int address = 0;
int value_adc;
int value_step_relay;
int value_step_mofet;

int pin_out_1 = 0 ;
int pin_out_2 = 0 ;
int pin_out_3 = 0 ;

int set_nhiet_do = 0;
int set_do_am = 0;
int set_solid = 0;

int time_relay_1;
int time_relay_2;
int time_mofet;
int key_temperature_relay;
int key_temperature_mofet;

long startsecondswd_relay;  // weekday start time in seconds
long stopsecondswd_relay;   // weekday stop  time in seconds

long startsecondswd_mofet;  // weekday start time in seconds
long stopsecondswd_mofet;   // weekday stop  time in seconds

long nowseconds;


//-----------------------------------------------

WiFiServer server_web_config(80);

WiFiClient Client_STA;

WidgetTerminal terminal(V9);

//-----------------------------------------------

unsigned int  so_sanh_str(String str1, String str2)
{
	unsigned int k, dem;

	for (unsigned dem = 0;dem<70;dem++)
	{
		for (unsigned char k = 0;k < 4;k++)
		{
			if (str1[k] != str2[dem + k])
				break;
			if (k == 3)
				return (dem + k + 2);
		}
	}
	return 0xFFFF;
}

String lay_ssid(String request)
{ 
	String str = "                     ";
	int  k = 0, dem = 0;
	k = so_sanh_str("SSID", request);
	if (k != 0xFFFF)
	{
		while (request[dem + k] != '&' && request[dem + k] != ' ')
		{
			str[dem] = request[dem + k];
			dem++;
		}
		str[dem + k + 1] = 0;
	}

	return str;
}

String lay_pass(String request)
{
	String str = "                     ";
	int     k = 0, dem = 0;
	k = so_sanh_str("PASS", request) ;
	if (k != 0xFFFF)
	{
		while (request[dem + k] != '&' && request[dem + k] != ' ')
		{
			str[dem] = request[dem + k];
			dem++;
		}
		str[dem + k + 1] = 0;
	}

	return str;
}

String lay_auth(String request)
{
	String str = "                                                 ";
	int     k = 0, dem = 0;
	k = so_sanh_str("AUTH", request);
	if (k != 0xFFFF)
	{
		while (request[dem + k] != '&' && request[dem + k] != ' ')
		{
			str[dem] = request[dem + k];
			dem++;
		}
		str[dem + k + 1] = 0;
	}

	return str;
}

void chuyen_char_to_mang(String s,char *data)
{
	unsigned char  k = s.length();
	for (unsigned char i = 0; i < k; i++)
		data[i] = s[i];
}

//-----------------------------------------------

void Write_EEPROM (String Data, unsigned char length , unsigned int point_start)
{
	for (unsigned int i = 0 ;i < length  ;i++)
	{
	
		EEPROM.write(point_start + i, Data[i]);

		if (Data[i] == ' ' )
		{
			EEPROM.write(point_start + i, 0);
			EEPROM.commit();
			break;
		}
	}
	delay(10);
}

void Read_EEPROM (signed char *Data, unsigned int point_start)
{
	unsigned char i = 0;

	while (char(EEPROM.read(point_start + i)) !=0)
	{
		Data[i] = char(EEPROM.read(point_start + i));
		i++;
	}
	Data[i] = 0;
}

void Read_all() // debug
{
	for (unsigned i = 0; i < 80; i++)
	{
		Serial.print(i);
		Serial.print("\t");
		Serial.print(EEPROM.read(i));
		Serial.println();
		delay(50);
	}
}

void MODE_CONFIG()
{
	WiFi.mode(WIFI_AP);

	WiFi.disconnect();

	delay(100);

	WiFi.softAP(ssid, password);

	WiFi.softAPConfig(local_IP, gateway, subnet);

	Serial.printf(" khoi dong phat WiFi %s \n", ssid);

	server_web_config.begin();

	//---------------------------------------

	while (true)
	{
		int state = digitalRead(PIN_LED_MODE_CONFIG);

		digitalWrite(PIN_LED_MODE_CONFIG , !state);

		Serial.printf("Soft-AP = %d \n", WiFi.softAPgetStationNum());

	    if (server_web_config.hasClient())
		{
			if (Client_STA = server_web_config.available())
			{
				Serial.println(" Client connected to web congifg ");

				String req = Client_STA.readStringUntil('\r');

				// ---------- xu ly request -------------- //

				String Address_ID = lay_ssid(req);
				String Password = lay_pass(req);
				String Authen = lay_auth(req);
			
				//------------------------------------------

				Serial.println(req);

				Client_STA.flush();

				//--------------- WEB -------------------

				String s;

				s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
				s += "<head>";
				s += "</head>";

				s += "<div class=\"container\">";
				s += "<h2> MODE config esp8266 </h2>";
				s += "<div class=\"row voffset\">";

				s += "<div>" + Address_ID + "</div> \n";
				s += "<div>" + Password + "</div> \n";
				s += "<div>" + Authen + "</div> \n";

				s += "  <br><br>\n";

				s += " <form action = \"/action_page.php\">\n";

				s += "	SSID:<br>\n";
				s += "  <input type=\"text\" name=\"SSID\" value=\"\">\n";
				s += "  <br>\n";

				s += "  PASS:<br>\n";
				s += "  <input type=\"text\" name=\"PASS\" value=\"\">\n";
				s += "  <br>\n";

				s += "  AUTH:<br>\n";
				s += "  <input type=\"text\" name=\"AUTH\" value=\"\">\n";
				s += "  <br><br>\n";

				s += "  <input type=\"submit\" value=\"Submit\">\n";

				s += "</form> \n";

				//--------------- req WEB -------------------

				Client_STA.print(s);

				Client_STA.flush();

				Serial.println("DONE");

				// ---------  String to Array --------------- //

				//Address_ID.toCharArray((char *)SSID_DATA, Address_ID.length());
				chuyen_char_to_mang(Address_ID , (char*)SSID_DATA);

				//Password.toCharArray((char *)PASS_DATA, Password.length());
				chuyen_char_to_mang(Password, (char *)PASS_DATA);

				//Authen.toCharArray((char *)AUTH_DATA, Authen.length());
				chuyen_char_to_mang(Authen, (char*)AUTH_DATA);

				Serial.print((char*)SSID_DATA);
				Serial.println(Address_ID.length());

				Serial.print(Password);
				Serial.println(Password.length());

				Serial.print((char*)AUTH_DATA);
				Serial.println(Authen.length());

				//---------- CLEAR MEMORI -----------------//


			    // ------- SAVE memori EEPROM -------------//
			
				Write_EEPROM (Address_ID, Address_ID.length(),0);
				Write_EEPROM (Password, Password.length(),20);
				Write_EEPROM (Authen, Authen.length(), 40);

				req = "";

				delay(100);

			}
		}
		
		delay(500);

	}
}

//------------------------------+----------------

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

	for (i = 0; i<5; i++)
	{
		for (ii = 0; ii<8; ii++)
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

//----------------------------------------------

void clockDisplay()
{
	String currentTime = String(hour()) + ":" + minute();
	String currentDate = String(day()) + "/" + month() + "/" + year();
	// Send time to the App
	Blynk.virtualWrite(V10, currentTime);
	//terminal.print(currentTime);
	//terminal.print(" ");
	// Send date to the App
	Blynk.virtualWrite(V11, currentDate);
	//terminal.println(currentDate);
	//----------------------------
	//terminal.flush();
}

void main_1()
{
	DHT_GetTemHumi(HNhietDo, LNhietDo, HDoAm, LDoAm);
	value_adc = (analogRead(A0));
	control_pin_digital();
}

void control_pin_digital()
{
	//-----------------------------------
	if (pin_out_1 == 1)
	{
		digitalWrite(PIN_RELAY_4, HIGH);
		digitalWrite(PIN_RELAY_5, HIGH);
	}
	if (pin_out_1 == 0)
	{
		digitalWrite(PIN_RELAY_4, LOW);
		digitalWrite(PIN_RELAY_5, LOW);
	}

	//------------------------------------
	//------------------------------------

	if (pin_out_3 == 1)
	{
		digitalWrite(PIN_MOFET_16, HIGH);
	}
	if (pin_out_3 == 0)
	{
		digitalWrite(PIN_MOFET_16, LOW);
	}

	//-------------------------------------
}

void main_2()
{
	nowseconds = ((hour() * 3600) + (minute() * 60) + second());

	//Serial.println(nowseconds);

	//------------------------------------

	if (HNhietDo >= value_step_relay ) 
	{
		pin_out_1 = 1;
		Blynk.virtualWrite(_relay_1, 1);
	}

	if (HNhietDo >= value_step_mofet)
	{
		pin_out_3 = 1;
		Blynk.virtualWrite(_mofet, 1);
	}
	//------------------------------------

	if (nowseconds == startsecondswd_relay)
	{
		pin_out_1 = 1;
		Blynk.virtualWrite(_relay_1 , 1);
	}

	if (nowseconds == stopsecondswd_relay)
	{
		pin_out_1 = 0;
		Blynk.virtualWrite(_relay_1, 0);
	}

	//------------------------------------

	if (nowseconds == startsecondswd_mofet)
	{
		pin_out_3 = 1;
		Blynk.virtualWrite(_mofet ,1);
	}

	if (nowseconds == stopsecondswd_mofet)
	{
		pin_out_3 = 0;
		Blynk.virtualWrite(_mofet, 0);
	}
}

//-----------------------------
// value =  nhiet do , do am , solid
//-----------------------------

BLYNK_READ(V0)
{
	Blynk.virtualWrite(V0, HNhietDo);
}

BLYNK_READ(V1)
{
	Blynk.virtualWrite(V1, HDoAm);
}

BLYNK_READ(V2)
{
	Blynk.virtualWrite(V2, value_adc);
}

//----------------------
//----- MANUAL ---------
//----------------------

BLYNK_WRITE(_relay_1)
{
	int v3 = param.asInt();

	if (v3 == 1) // on = 1
	{
		pin_out_1 = 1;
		Blynk.virtualWrite(_auto_relay_1, 0);
	}

	else  // off = 0 
	{
		pin_out_1 = 0;
		Blynk.virtualWrite(_auto_relay_1, 0);
	}

}

BLYNK_WRITE(_relay_2)
{

}

BLYNK_WRITE(_mofet)
{
	int v5 = param.asInt();

	if (v5 == 1) // on = 1
	{
		pin_out_3 = 1;
		Blynk.virtualWrite(_auto_mofet, 0);
	}

	else  // off = 0 
	{
		pin_out_3 = 0;
		Blynk.virtualWrite(_auto_mofet, 0);
	}
}

//----------------------
//------ AUTO ----------
//----------------------

BLYNK_WRITE(_auto_relay_1)
{
	int v6 = param.asInt();
	if (v6 == 1)
	{
		Blynk.virtualWrite(_relay_1, 0);
		time_relay_1 = 1;
		pin_out_1 = 0;
	}
	else 
	{
		Blynk.virtualWrite(_relay_1, 0);
		time_relay_1 = 0;
		pin_out_1 = 0;
	}

}

BLYNK_WRITE(_auto_relay_2)
{
	//
}

BLYNK_WRITE(_auto_mofet)
{
	int v8 = param.asInt();
	if (v8 == 1)
	{
		Blynk.virtualWrite(_mofet, 0);
		time_mofet = 1;
		pin_out_3 = 0;
	}
	else
	{
		Blynk.virtualWrite(_mofet, 0);
		time_mofet = 0;
		pin_out_3 = 0;
	}
}

//------------------------------

BLYNK_WRITE(_control_temperature_relay)
{
	int v9 = param.asInt();
	if (v9 == 1)
	{
		key_temperature_relay = 1;
	}
	else
	{
		key_temperature_relay = 0;
	}
}

BLYNK_WRITE(_control_temperature_mofet)
{
	int v17 = param.asInt();
	if (v17 == 1)
	{
		key_temperature_mofet = 1;
	}
	else
	{
		key_temperature_mofet = 0;
	}
}

//-------------------------------
//------  Setting time ----------
//-------------------------------

BLYNK_WRITE(_time_relay_1) 
{
	if (time_relay_1 == 1)
	{
		sprintf(Date, "%02d/%02d/%04d", day(), month(), year());
		sprintf(Time, "%02d:%02d:%02d", hour(), minute(), second());

		TimeInputParam t(param);

		//--------------------------------------------------------------------
		int day_adjustment = -1;

		if (weekday() == 1)
		{
			day_adjustment = 6; // needed for Sunday, Time library is day 1 and Blynk is day 7
		}
		//--------------------------------------------------------------------

		if (t.isWeekdaySelected(weekday() + day_adjustment))
		{
			Serial.println("cai dat vao ngay hom nay");
			//-------------------------------------------------------------------

			if (t.hasStartTime())
			{
				Serial.println(String("Start: ") + t.getStartHour() + ":" + t.getStartMinute());
				startsecondswd_relay = (t.getStartHour() * 3600) + (t.getStartMinute() * 60);
			}

			//--------------------------------------------------------------------

			if (t.hasStopTime())
			{
				Serial.println(String("Stop : ") + t.getStopHour() + ":" + t.getStopMinute());
				stopsecondswd_relay = (t.getStopHour() * 3600) + (t.getStopMinute() * 60);
			}

			//--------------------------------------------------------------------

			for (int i = 1; i <= 7; i++) 
			{
				if (t.isWeekdaySelected(i))
				{
					Serial.println(String(" ngay") + i + " da chon ");
					Serial.flush();
				}
			}

			//--------------------------------------------------------------------
			Serial.println(Time);
			Serial.println(Date);
			Serial.println(String("Time zone: ") + t.getTZ());
		    //--------------------------------------------------------------------
		}
		else 
		{
			Serial.println("khong hoat dong vao ngay hom nay");
			Serial.flush();   
		}

	}
}

BLYNK_WRITE(_time_relay_2)
{

}

BLYNK_WRITE(_time_mofet)
{
	if (time_mofet == 1)
	{
		sprintf(Date, "%02d/%02d/%04d", day(), month(), year());
		sprintf(Time, "%02d:%02d:%02d", hour(), minute(), second());

		TimeInputParam t(param);

		//--------------------------------------------------------------------
		int day_adjustment = -1;

		if (weekday() == 1)
		{
			day_adjustment = 6; // needed for Sunday, Time library is day 1 and Blynk is day 7
		}
		//--------------------------------------------------------------------

		if (t.isWeekdaySelected(weekday() + day_adjustment))
		{
			Serial.println("cai dat vao ngay hom nay");
			//-------------------------------------------------------------------

			if (t.hasStartTime())
			{
				Serial.println(String("Start: ") + t.getStartHour() + ":" + t.getStartMinute());
				startsecondswd_mofet = (t.getStartHour() * 3600) + (t.getStartMinute() * 60);
			}

			//--------------------------------------------------------------------

			if (t.hasStopTime())
			{
				Serial.println(String("Stop : ") + t.getStopHour() + ":" + t.getStopMinute());
				stopsecondswd_mofet = (t.getStopHour() * 3600) + (t.getStopMinute() * 60);
			}

			//--------------------------------------------------------------------

			for (int i = 1; i <= 7; i++)
			{
				if (t.isWeekdaySelected(i))
				{
					Serial.println(String(" ngay") + i + " da chon ");
					Serial.flush();
				}
			}

			//--------------------------------------------------------------------
			Serial.println(Time);
			Serial.println(Date);
			Serial.println(String("Time zone: ") + t.getTZ());
			//--------------------------------------------------------------------
		}
		else
		{
			Serial.println("khong hoat dong vao ngay hom nay");
			Serial.flush();
		}

	}
}

//--------------------------------------------

BLYNK_WRITE(_set_temperature_relay)
{
	if (key_temperature_relay == 1)
	{
		value_step_relay = param.asInt();
	}
}

BLYNK_WRITE(_set_temperature_mofet)
{
	if (key_temperature_mofet == 1)
	{
		value_step_mofet = param.asInt();
	}
}

//----------------------------------------------
//----------------------------------------------

void setup()
{
	Serial.begin(9600);

	EEPROM.begin(512);

	delay(10);

	//-------------------------------------

	pinMode(PIN_BUTTON, INPUT_PULLUP);
	pinMode(PIN_LED_MODE_CONFIG, OUTPUT);
	pinMode(PIN_LED , OUTPUT);
	pinMode(PIN_RELAY_4, OUTPUT);
	pinMode(PIN_RELAY_5, OUTPUT);
	pinMode(PIN_MOFET_16, OUTPUT);

	//---------- MODE CONFIG ---------------

	if (digitalRead(PIN_BUTTON) == 0) 
	{
		MODE_CONFIG();
	}

	//-------------------------------------

	Serial.println("Reading EEPROM : ");

	Read_EEPROM(SSID_DATA, 0);
		Serial.println((char*)SSID_DATA);

	Read_EEPROM(PASS_DATA, 20);
		Serial.println((char*)PASS_DATA);

	Read_EEPROM(AUTH_DATA, 40);
		Serial.println((char*)AUTH_DATA);
	
	//---------------------------------------------- 

	Serial.println(" BLYNK BEGIN: "); 

	Blynk.begin((char*)AUTH_DATA,(char*)SSID_DATA, (char*)PASS_DATA);

	//Blynk.begin(au, id, pass);

	//-----------------------------------------------

	digitalWrite(PIN_LED_MODE_CONFIG, HIGH); // led pin status program .

	//-----------------------------------------------

	rtc.begin();

	setSyncInterval(10 * 60);

	timer.setInterval(1000L , clockDisplay);

	timer.setInterval(50L , main_1);

	timer.setInterval(1000L, main_2);
}

void loop()
{

	Blynk.run();

	timer.run();

	//-------------------------------------------
}
