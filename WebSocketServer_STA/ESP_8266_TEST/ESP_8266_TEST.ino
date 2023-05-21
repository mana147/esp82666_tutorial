
char t = OK;

const char *str = "hello";

const char *fingerprint = 
"#begin CC AA 48 48 66 46 0E 91 53 2C 9C 7C 23 2A B1 74 4D 29 9D 33\
CC AA 48 48 66 46 0E 91 53 2C 9C 7C 23 2A B1 74 4D 29 9D 33\
CC AA 48 48 66 46 0E 91 53 2C 9C 7C 23 2A B1 74 4D 29 9D 33\
CC AA 48 48 66 46 0E 91 53 2C 9C 7C 23 2A B1 74 4D 29 9D 33\
CC AA 48 48 66 46 0E 91 53 2C 9C 7C 23 2A B1 74 4D 29 9D 33\
CC AA 48 48 66 46 0E 91 53 2C 9C 7C 23 2A B1 74 4D 29 9D 33\
CC AA 48 48 66 46 0E 91 53 2C 9C 7C 23 2A B1 74 4D 29 9D 33\
CC AA 48 48 66 46 0E 91 53 2C 9C 7C 23 2A B1 74 4D 29 9D 33\
CC AA 48 48 66 46 0E 91 53 2C 9C 7C 23 2A B1 74 4D 29 9D 33\
CC AA 48 48 66 46 0E 91 53 2C 9C 7C 23 2A B1 74 4D 29 9D 33\
CC AA 48 48 66 46 0E 91 53 2C 9C 7C 23 2A B1 74 4D 29 9D 33\
CC AA 48 48 66 46 0E 91 53 2C 9C 7C 23 2A B1 74 4D 29 9D 33\
CC AA 48 48 66 46 0E 91 53 2C 9C 7C 23 2A B1 74 4D 29 9D 33\
CC AA 48 48 66 46 0E 91 53 2C 9C 7C 23 2A B1 74 4D 29 9D 33\
CC AA 48 48 66 46 0E 91 53 2C 9C 7C 23 2A B1 74 4D 29 9D 33\
CC AA 48 48 66 46 0E 91 53 2C 9C 7C 23 2A B1 74 4D 29 9D 33\
CC AA 48 48 66 46 0E 91 53 2C 9C 7C 23 2A B1 74 4D 29 9D 33\
CC AA 48 48 66 46 0E 91 53 2C 9C 7C 23 2A B1 74 4D 29 9D 33 #end";

void setup()
{
	Serial.begin(115200);
	//Serial.begin(921600);
}
void loop()
{
	char c;

	String str;

	if (Serial.available())
	{
		str = Serial.readString();

		if (str == "ok" )
		{
			//digitalWrite(LED_BUILTIN, LOW);
			delay(100);                    
			//digitalWrite(LED_BUILTIN, HIGH); 
		}
		Serial.println(">>>>"+ str);
	}

	Serial.print(fingerprint);

	delay(100);
}