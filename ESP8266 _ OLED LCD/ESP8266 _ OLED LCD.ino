/*********************************************************************
Following code is by Electromania based on Adafruit graphics and DallasTemperature libraries
The video is available at https://www.youtube.com/watch?v=WVe1YggGTIk
The code is available at http://theelectromania.blogspot.it/2016/03/esp8266-esp-12e-nodemcu-and-ds18b20.html
Use this code at your own risk and can be freely circulated and used without any permission or credits.
you can support my projects by subscribing to my channel and giving thumbs UP :).
--------------------------------------------------------------------

// following text is from standard adafruit library disclaimer
This is an example for our Monochrome OLEDs based on SSD1306 drivers

Pick one up today in the adafruit shop!
------> http://www.adafruit.com/category/63_98

This example is for a 128x64 size display using SPI to communicate
4 or 5 pins are required to interface

Adafruit invests time and resources providing this open source code,
please support Adafruit and open-source hardware by purchasing
products from Adafruit!

Written by Limor Fried/Ladyada  for Adafruit Industries.
BSD license, check license.txt for more information
All text above, and the splash screen must be included in any redistribution
*********************************************************************/

//Libraries available at:  https://github.com/adafruit/Adafruit_SSD1306 
//                   https://github.com/adafruit/Adafruit-GFX-Library
//https://github.com/milesburton/Arduino-Temperature-Control-Library

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <OneWire.h>
#include <DallasTemperature.h>

//uncomment  lines 41 to 57 if you get errors about pin definations (eg. error: unknown type name 'D1' etc) while compiling. This is to map exact GPIOs of ESP8266 with NodeMCU pins
// refer following link for more details - https://github.com/esp8266/Arduino/blob/master/variants/nodemcu/pins_arduino.h#L37-L59 
// if ESP8266 package for Arduino is updated to latest version 2.1.0 or above, then pins_arduino.h should be automatically downloaded and errors for unknown type name should be solved
/*
static const uint8_t SDA = 4;
static const uint8_t SCL = 5;

static const uint8_t LED_BUILTIN = 16;
static const uint8_t BUILTIN_LED = 16;

static const uint8_t D0   = 16;
static const uint8_t D1   = 5;
static const uint8_t D2   = 4;
static const uint8_t D3   = 0;
static const uint8_t D4   = 2;
static const uint8_t D5   = 14;
static const uint8_t D6   = 12;
static const uint8_t D7   = 13;
static const uint8_t D8   = 15;
static const uint8_t D9   = 3;
static const uint8_t D10  = 1;
*/

#define ONE_WIRE_BUS D1               // this defines the pin number of NodeMCU at which you have connected DQ or Data or yellow wire of DS18B20
// i have used pin D1, you can use anyother digital pin (try to avoid using pin D0, i faced some problems when i use pin D0)

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);   // constructor for our DS18B20 sensor to read temperature using DallasTemperature libary


									   // If using software SPI (the default case):   please stick to these pin numbers unless you have some other board
									   //Pin connections are as follows - (D5, D7, D3, D4, 12); // (SCL,SDA,RES,DC,CS)

#define OLED_SDA   D7  //MOSI
#define OLED_SCL   D5  //CLK
#define OLED_DC    D4  //
#define OLED_CS    12  // no need of connecting, just use some pin number
#define OLED_RESET D3  //RES

Adafruit_SSD1306 display(OLED_SDA, OLED_SCL, OLED_DC, OLED_RESET, OLED_CS);      // constructor to call OLED display using adafruit library

#define LOGO16_GLCD_HEIGHT 16 
#define LOGO16_GLCD_WIDTH  16 
static const unsigned char PROGMEM logo16_glcd_bmp[] =    // // since i am using adafruit library, i have to display their logo
{ B00000000, B11000000,
B00000001, B11000000,
B00000001, B11000000,
B00000011, B11100000,
B11110011, B11100000,
B11111110, B11111000,
B01111110, B11111111,
B00110011, B10011111,
B00011111, B11111100,
B00001101, B01110000,
B00011011, B10100000,
B00111111, B11100000,
B00111111, B11110000,
B01111100, B11110000,
B01110000, B01110000,
B00000000, B00110000 };

//#if (SSD1306_LCDHEIGHT != 64)
//#error("Height incorrect, please fix Adafruit_SSD1306.h!");
//#endif

void setup() {
	Serial.begin(9600);
	sensors.begin();
	// by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
	display.begin(SSD1306_SWITCHCAPVCC);   // since i am using adafruit library, i have to display their logo
										   // init done
										   // Show image buffer on the display hardware.
										   // Since the buffer is intialized with an Adafruit splashscreen
										   // internally, this will display the splashscreen.
	display.display();
	delay(500);
	// Clear the buffer.
	display.clearDisplay();
	// NOTE: You _must_ call display after making any drawing commands
	// to make them visible on the display hardware!

	// following OPTIONAL part is just to display the message only during the startup--- you can modify or just remove it 
	display.setTextColor(WHITE);
	display.setTextSize(1);
	display.setCursor(25, 11);
	display.print("Electromania");
	//  display.print("Temp. reading");
	display.display();
	delay(2000);
	display.clearDisplay();
	display.setCursor(0, 3);
	display.print("PHAM TRUNG HIEU");
	display.display();
	delay(4000);
	display.clearDisplay();
	// OPTIONAL part ends here
}

void loop() {
	//dispTemp();
	display.clearDisplay();
}

void dispTemp(void) {   // function to read and display temperature

	sensors.requestTemperatures();           // Send the command to DS18B20 to get temperatures
	float temperatureDegC = sensors.getTempCByIndex(0);
	//  Serial.print("Temperature (Deg C):");   // enable just for debugging
	// Serial.println(temperatureDegC);  // enable just for debugging prints the temperature in deg C 
	// Serial.println(display.width());   // enable just for debugging prints actual width of your OLED display in pixels
	//    Serial.println(display.height()); // enable just for debugging prints actual height of your OLED display  in pixels


	displaytemp(temperatureDegC, 'C');   // call to function void displaytemp(float temp, char C_F) giving temperature in Deg C and character C

	delay(3000);  // wait for 3 seconds and then clear display, you can adjust this time to increase or decrease duration of displaying Deg C or Deg F
	display.clearDisplay();
	displaytemp(DallasTemperature::toFahrenheit(temperatureDegC), 'F');   // call to function void displaytemp(float temp, char C_F) giving temperature in Deg F and character F
	delay(3000); // wait for 3 seconds and then clear display, you can adjust this time to increase or decrease duration of displaying Deg C or Deg F

}


void displaytemp(float temp, char C_F)   // function to display temp, takes temperature and character C or F from calling function void dispTemp(void)
{
	display.drawRect(1, 1, display.width() - 1, display.height() - 1, WHITE);   // draws the outer rectangular boundary on the screen
	display.setTextColor(WHITE);   // i have white OLED display, you can use other colors in case you have multicolored display
	display.setTextSize(1);        // i have used large font to display temperature, it can be varied as per your taste
	display.setCursor(104, 3);
	display.print("o");            // this prints the "o" symbol to show Degree 
	display.setTextSize(2);
	display.setCursor(112, 10);
	display.print(C_F);           // this takes character from function call either C or F for centigrade of farenheit 

	if (temp >= 100 || temp < 0) {    //i have reduced font size if temp goes in 3 digits or is -ve, keeps text in center of display , it can be varied as per your taste
		display.setTextSize(2);
		display.setCursor(15, 10);
	}
	else if (temp < 10 && temp >= 0) {  //some adjustments to keep text in center of display , it can be varied as per your taste
		display.setTextSize(3);
		display.setCursor(25, 6);
	}

	else
	{
		display.setTextSize(3);    //i have used large font to display temperature, it can be varied as per your taste
		display.setCursor(10, 6);
	}

	display.print(temp);   // finally prints the temperature on your OLED display
	display.display();

	int x;
	x = constrain(x, 0, 100);


}



