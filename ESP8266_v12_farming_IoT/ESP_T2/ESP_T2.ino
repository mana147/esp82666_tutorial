// station

#include <ESP8266WiFi.h>
#include <String.h>

#include <SPI.h>
#include <Wire.h>

#include <SimpleTimer-master\SimpleTimer.h>

//----------------------------

#define DHT_ER  0
#define DHT_OK  1
#define DHT_DATA  0

byte HNhietDo, LNhietDo, HDoAm, LDoAm;

byte DHT_GetTemHumi(byte &tem1, byte &tem2, byte &humi1, byte &humi2);

//----------------------------

SimpleTimer time_1, 
			time_2, 
			time_3, 
			time_4, 
			time_5;

IPAddress staticIP(192, 168, 1, 101);
IPAddress gateway(192, 168, 1, 9);
IPAddress subnet(255, 255, 255, 0);

IPAddress sever_ip(192, 168, 4, 22);

WiFiClient client_1;

//----------------------------

//const char *ssid = "DUC-HANG";
const char *ssid = "MODE_AC";
const char *password = "123456789";

uint8_t macAddr[6];

String  cm_temp  = "get_temp";
String  cm_humi  = "get_humi";
String  cm_solid = "get_solid";

//----------------------------	

const byte add_DS1307 = 0x68;

const byte byte_data = 7;

int second, minute, hour, day, wday, month, year;

int giay, phut, gio, thu, ngay, thang, nam;

//----------------------------	

int value_adc;
int d_value_adc;

char h_1;
char t_1;



//----------------------------
//----------------------------
// ---------------------------

void read_time_DS1307()
{
	Wire.beginTransmission(add_DS1307);
	Wire.write((byte)0x00);
	Wire.endTransmission();
	Wire.requestFrom(add_DS1307, byte_data);

	giay = bcd2dec(Wire.read() & 0x7f);
	phut = bcd2dec(Wire.read());
	gio = bcd2dec(Wire.read() & 0x3f); // chế độ 24h.
	thu = bcd2dec(Wire.read());
	ngay = bcd2dec(Wire.read());
	thang = bcd2dec(Wire.read());
	nam = bcd2dec(Wire.read());
	nam = nam + 2000;
}

void set_time_DS1307(byte hr, byte min, byte sec, byte wd, byte d, byte mth, byte yr)
{
	Wire.beginTransmission(add_DS1307);
	Wire.write(byte(0x00));
	Wire.write(dec2bcd(sec));
	Wire.write(dec2bcd(min));
	Wire.write(dec2bcd(hr));
	Wire.write(dec2bcd(wd)); // day of week: Sunday = 1, Saturday = 7
	Wire.write(dec2bcd(d));
	Wire.write(dec2bcd(mth));
	Wire.write(dec2bcd(yr));
	Wire.endTransmission();
}

int bcd2dec(byte num)
{
	return ((num / 16 * 10) + (num % 16));
}

int dec2bcd(byte num)
{
	return ((num / 10 * 16) + (num % 10));
}

//----------------------------

void Init_esp8266(void)
{
	WiFi.mode(WIFI_STA);
	Serial.printf("ket noi wifi %s \n", ssid);
	WiFi.begin(ssid, password);

	while (WiFi.status() != WL_CONNECTED)
	{
		delay(500);
		Serial.printf(".");
	}

	Serial.println();
	Serial.printf(" Da ket noi den wifi %s \n", ssid);
	Serial.print(" Local IP : ");
	Serial.println(WiFi.localIP());
	WiFi.macAddress(macAddr);
	Serial.printf(" MAC address: %02x:%02x:%02x:%02x:%02x:%02x\n", macAddr[0], macAddr[1], macAddr[2], macAddr[3], macAddr[4], macAddr[5]);
	Serial.print("Subnet mask: ");
	Serial.println(WiFi.subnetMask());
	Serial.printf("GateWay IP: %s\n", WiFi.gatewayIP().toString().c_str());
	Serial.printf("BSSID: %s\n", WiFi.BSSIDstr().c_str());
	Serial.printf("RSSI: %d dBm\n", WiFi.RSSI());
}

void Wait_conect(void)
{
	if (WiFi.status() == WL_CONNECTED)
	{
		while (!client_1.connected())
		{
			if (!client_1.connect(sever_ip, 23))
			{
				return;
			}
		}
	}
}

void Get_Comman_ap(void)
{
	unsigned char status_esp = 0; // cờ hiệu , khóa chốt
	unsigned char st[3] = { 0,0,0 };

	while (client_1.available())
	{
		String line = client_1.readStringUntil('\r');
		for (unsigned char i = 0;i<8;i++)
		{
			if (cm_temp[i]  != line[i])
				st[0] = 1;
			if (cm_humi[i]  != line[i])
				st[1] = 1;
			if (cm_solid[i] != line[i])
				st[2] = 1;
		}

		status_esp = 1;
	}

	if (status_esp == 1)
	{
		if (st[0] == 0)
		{
			t_1 = HNhietDo / 1;
			String str = "nhiet do:";
			client_1.write(str.c_str());
			client_1.print(HNhietDo);
			client_1.print("*C \r");
		}

		if (st[1] == 0)
		{
			h_1 = HDoAm / 1;
			String str = "Do am:";
			client_1.write(str.c_str());
			client_1.print(HDoAm);
			client_1.print("*R \r");
		}

		if (st[2] == 0)
		{
			d_value_adc = (value_adc * 100) / 1024;
			String str = "solid:";
			client_1.write(str.c_str());
			client_1.print(d_value_adc);
			client_1.print("*D \r");
		}
	}
}

//-----------------------------

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

//-----------------------------

void Display_Serial()
{
	Serial.print(gio);
	Serial.print(":");
	Serial.print(phut);
	Serial.print(":");
	Serial.print(giay);
	Serial.print("---");
	//----------------------
	Serial.print(thu);
	Serial.print("/");
	Serial.print(ngay);
	Serial.print("/");
	Serial.print(thang);
	Serial.print("/");
	Serial.print(nam);
	Serial.println();
	//----------------------
	Serial.print(" V = ");
	Serial.print(value_adc);

	Serial.print(" H = ");
	Serial.print(HNhietDo);

	Serial.print(" T = ");
	Serial.print(HDoAm);

	Serial.println();
}

//----------------------------
//-------------main ----------
//----------------------------

void setup()
{
	Serial.begin(9600);

	Wire.begin();

	set_time_DS1307(12, 30, 00, 6, 28, 12, 17);// 00h:00p:00s // thu-00n-00t-0000N

	Init_esp8266();

	Wait_conect();

	time_1.setInterval(1000L , Display_Serial );

}

void loop()
{
	DHT_GetTemHumi(HNhietDo, LNhietDo, HDoAm, LDoAm);

	value_adc = analogRead(A0);

	read_time_DS1307();

	Get_Comman_ap();

	time_1.run(); //DISPLAY_SERIAL

}
