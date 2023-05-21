

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <SimpleKalmanFilter.h>
#include <SimpleTimer-master\SimpleTimer.h>


char auth[] = "0e2eaaaac06e4cc4aab85ea7909c42e0";    //AuthToken copy ở Blynk Project
												 //char auth[] = "7f6558273ab34460a80e9493f6f2cc1e";
char ssid[] = "WIFI_TANG1";  //Tên wifi
char pass[] = "08031993";     //Mật khẩu wifi

unsigned char ledPinOut = D1;
unsigned char buttonPin = D2;
unsigned char button1 = 0;

static unsigned int dem = 0;
static unsigned int dem2 = 0;


//---------------------------------------------

SimpleKalmanFilter bo_loc(1, 1, 0.009);

int u_kalman1;

SimpleTimer timer_esp;
SimpleTimer button_esp;


//=========================================	

void analogR() // ham doc bien tro
{
	int value1 = analogRead(A0);
	u_kalman1 = bo_loc.updateEstimate(value1);
	Blynk.virtualWrite(V1, u_kalman1);
}

//--------------------------------------------

BLYNK_WRITE(V2)
{
	int virtual_v2 = param.asInt();
	//----------------------------
	if (virtual_v2 == 1)
	{
		button1 = 1;
	}
}

//--------------------------------------------
void digitalR()
{
	if (digitalRead(buttonPin) == 0)
	{
		dem++;
		if (dem == 5)
			button1 = 1;
		if (dem == 150)
			dem = 100;
	}
	else
	{
		dem = 0;
	}

	//-----------------------------------

	if (button1 == 1)
	{
		button1 = 0;
		if (digitalRead(ledPinOut) == 0)
		{
			digitalWrite(ledPinOut, HIGH);
			Blynk.virtualWrite(V3, HIGH);
		}
		else
		{
			digitalWrite(ledPinOut, LOW);
			Blynk.virtualWrite(V3, LOW);
		}

	}

}
//=========================================

void setup()
{
	Serial.begin(9600);

	//-----------------------------

	pinMode(ledPinOut, OUTPUT);

	pinMode(buttonPin, INPUT_PULLUP);

	//-----------------------------

	Blynk.begin(auth, ssid, pass);

	//-----------------------------

	timer_esp.setInterval(50L, analogR);

	button_esp.setInterval(5L, digitalR);

	//-----------------------------
}

void loop()
{
	Blynk.run();
	timer_esp.run();
	button_esp.run();
}

//=========================================