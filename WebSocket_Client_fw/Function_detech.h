// Function_detech.h

#ifndef _FUNCTION_DETECH_h
#define _FUNCTION_DETECH_h

#include <Arduino.h>

#define SIZE 10	

class DETECH_AT_CODE
{
public:
	String STR_AT[SIZE];
	String STR_CODE[SIZE];
	void D_AT(String payload)
	{
		if (payload.startsWith("AT") && payload.endsWith(">"))
		{
			payload.remove(payload.indexOf("="), payload.lastIndexOf(">"));

			for (int i = 0; i < SIZE; i++)
			{
				STR_AT[i] = payload.substring(0, payload.indexOf("+"));

				if (payload.indexOf("+") == -1) {
					STR_AT[i] = payload;
					break;
				}
				else {
					payload.remove(0, payload.indexOf("+") + 1);
				}
			}

		}
		else
		{
			for (int i = 0; i < SIZE; i++)
			{
				STR_AT[i] = "";
			}
		}
	}
	void D_CODE(String payload)
	{
		if (payload.startsWith("AT") && payload.endsWith(">"))
		{
			payload.remove(payload.indexOf("AT"), payload.indexOf("<"));

			for (int i = 0; i < SIZE; i++)
			{
				STR_CODE[i] = payload.substring(payload.indexOf("<") + 1, payload.indexOf(">"));
				payload.remove(0, payload.indexOf(">") + 1);
			}

		}
		else
		{
			for (int i = 0; i < SIZE; i++)
			{
				STR_CODE[i] = "";
			}
		}
	}
};
class DETECH_CONFIG_JSON
{
public:
	int Wifi_BaudRate;
	int Wifi_Channel;
	String Wifi_mode;
	String Wifi_ssid;
	String Wifi_pass;

	String socket_mode;
	String socket_host;
	int socket_port;
	String socket_url;
	String socket_wss;

	String web_mode;
	int web_port;
	String web_url;
};
class DETECH_INDEX_JSON
{
public:
	String sn;
	String ac;
	String cm;
	String rx_data;
	String rx_addr;
};

#endif
