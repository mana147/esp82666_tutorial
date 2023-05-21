#include <SimpleKalmanFilter.h>
#include <ESP8266WiFi.h>
#include <String.h>

//--------------------------------------

#define DHT_DATA          5
#define DHT_ER            0
#define DHT_OK            1

#define ledPin 4
#define timer0_preload  80000000  // Note this is an artibrary value, in most applications use 80000000 for 80MHz or 16000000 at 160MHz
                                  // to achieve a 1-sec delay/timer time-out event.
#define my_delay 1                // Set interrupt time in secs. Value = 2 x Numbesr of Seconds, so 10-Secs = 10 x 2 = 20

volatile int toggle;

//--------------------------------------

IPAddress staticIP(192, 168, 1, 101);
IPAddress gateway(192, 168, 1, 9);
IPAddress subnet(255, 255, 255, 0);
IPAddress sever_ip(192, 168, 4, 22);

WiFiClient client_1;

const char *ssid = "DUC-HANG";
const char *password = "123456789";

uint8_t macAddr[6];
String  nhiet_do = " nhiet do : ";
String  do_am;

byte  T1, T2, H1, H2;

String  cm_temp = "get_temp";
String  cm_humi = "get_humi";
String  cm_npk = "get_npkt";
String  cm_doph = "get_doph";


SimpleKalmanFilter bo_loc(1, 1, 0.009);

int u_kalman1;
//--------------------------------------

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


void Init_baurate(void)
{
	Serial.begin(115200);
	Serial.println();
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


void Get_Comman_ap(void)
{
	unsigned char status_esp = 0;
	unsigned char st[4] = { 0,0,0,0 };

	while (client_1.available())
	{
		String line = client_1.readStringUntil('\r');
		for (unsigned char i = 0;i<8;i++)
		{
			if (cm_temp[i] != line[i])
				st[0] = 1;
			if (cm_humi[i] != line[i])
				st[1] = 1;
			if (cm_npk[i] != line[i])
				st[2] = 1;
			if (cm_doph[i] != line[i])
				st[3] = 1;
		}

		status_esp = 1;
	}

	if (status_esp == 1)
	{
		if (st[0] == 0)
		{
			String str = "nhiet do:";
			client_1.write(str.c_str());
			client_1.print(T1);
			client_1.print("*C \r");
		}

		if (st[1] == 0)
		{
			String str = "Do am:";
			client_1.write(str.c_str());
			client_1.print(H1);
			client_1.print("% \r");
		}

		if (st[2] == 0)
		{
			String str = "dinh duong :";
			client_1.write(str.c_str());
			client_1.print(413);
			client_1.print("% \r");
		}

		if (st[3] == 0)
		{
			String str = "Do PH:";
			client_1.write(str.c_str());
			client_1.print(6);
			client_1.print("\r");
		}

	}
}

//---------------------------------------------------

void setup(void)
{
	Init_baurate();
	Init_esp8266();
	Wait_conect();
	
}

void loop()
{
	int value1 = analogRead(A0);
	u_kalman1 = bo_loc.updateEstimate(value1);
	Serial.println(u_kalman1);

	DHT_GetTemHumi(T1, T2, H1, H2);
	Get_Comman_ap();

}


//-------- interrupts --------





