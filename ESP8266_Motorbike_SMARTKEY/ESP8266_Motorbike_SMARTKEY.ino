/*
< Phạm Trung Hiếu >
Web nhúng trên esp8266 v12
code web được tách riêng khỏi hệ thống , và lưu trên 1 phần vùng bộ nhớ riêng
trong quá trình chạy web được gọi ra thông qua thư viên FS.h và đẩy toàn bộ code lên phía client .
Phân vùng bộ nhớ SPIFFS
mã nguồn và tài liệu liên quan cập nhật tại https://github.com/esp8266/Arduino/blob/master/doc/filesystem.rst

*/
//-----------------------------------------
#include <ESP8266WiFi.h>
//-----------------------------------------
#include "FS.h"
//-----------------------------------------
#include "SimpleTimer.h"
//-----------------------------------------

#define muber_max_user 10

SimpleTimer timer;

WiFiServer Server_web(80);
WiFiClient Client_STA;

IPAddress local_IP(192, 168, 4, 99);	//  IP tĩnh muốn gán cho module
IPAddress gateway(192, 168, 4, 9);		//  IP của gateway (thường là router) để kết nối ra mạng bên ngoài
IPAddress subnet(255, 255, 255, 0);		//  subnet xác định phạm vi IP của mạng nội bộ

const char *ssid = "Motorbike_SMARTKEY";          // ten wifi phát ra
const char *password = "123456789";				  // mật khâu wifi phat ra ,

const char* htmlfile = "/new7.html";

const char* key_In_html = "esp8266";

char PIN_BUTTON = 12; //D6;

char*list_href[] = { "a0>","a1>","a2>","a3>","a4>","a5>","a6>","a7>","a8>","a9>" };

unsigned char STT = 0;

String nameString[muber_max_user];

String nameString_send[muber_max_user];

String request_from_clienst;

//-----------------------------------------

String read_files_in_SPIFFS(String string)
{
	String s;
	File data_files;
	data_files = SPIFFS.open(string, "r");
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

int write_files_in_SPIFFS(String string, String path)
{
	char key;
	String s;
	File data_flies;
	Dir dir;

	data_flies = SPIFFS.open(path, "w");

	//dir = SPIFFS.openDir(path);

	if (data_flies)
	{
		data_flies.print(string);
		data_flies.flush();
		key = 1;
	}
	else
	{
		key = 0;
	}

	data_flies.close();
	return key;
}

//-----------------------------------------

String take_name_from_request(String request)
{
	String buffer_str = request;

	int k1 = buffer_str.indexOf("name");

	if (k1 != -1)
	{
		buffer_str.remove(0, k1 + 5);

		int k2 = buffer_str.indexOf('&');

		buffer_str.remove(k2);

		if (buffer_str == 0)
		{
			buffer_str = "NULL";
		}

		return buffer_str;
	}
}

String take_name_from_html(String html_file, String take)
{
	String buffer;
	int k1 = html_file.indexOf(take, 2029);
	if (k1 != -1)
	{
		html_file.remove(0, k1 + 3);
		int k2 = html_file.indexOf('<');
		html_file.remove(k2);
		return html_file;
	}
}

String take_name_from_delete(String html_file, String take )
{
	String t = ">" + take + "<";  Serial.println(t);
	int k1 = html_file.indexOf(t, 2029);
	if (k1 != -1)
	{
		Serial.println("ok");
		return take;
	}
	else
	{
		Serial.println("fail");
		return "NULL";
	}
}

//-----------------------------------------

String str_one(String html_cut, String key)
{
	int k1 = html_cut.indexOf(key);
	if (k1 != -1)
	{
		html_cut.remove(k1);
	}
	return html_cut;
}

String str_two(String html_cut, String key)
{
	int k1 = html_cut.indexOf(key);
	if (k1 != -1) {
		html_cut.remove(0, k1);
	}
	return html_cut;
}

String add_name_in_html(String html_cut, String name_replace, String stt) // a0> 
{
	String str = html_cut;
	if (stt != NULL)
	{
		int k1 = str.indexOf(stt);
		if (k1 != -1)
		{
			str.remove(0, k1 + 3);

			int k2 = str.indexOf('<');

			str.remove(k2);

			html_cut.replace(stt + str, stt + name_replace);

			return html_cut;
		}
	}
	else
	{
		return html_cut;
	}

}

//-----------------------------------------

void main_web(void)
{
	if (Server_web.hasClient()) // kiem tra xem co client
	{
		if (Client_STA = Server_web.available())
		{
			String str_html = read_files_in_SPIFFS(htmlfile);
			String Str1 = str_one(str_html, key_In_html);
			String Str2 = str_two(str_html, key_In_html);

			String str_request = Client_STA.readStringUntil('\r');

			int bf_get = str_request.indexOf("GET");
			int bf_add = str_request.indexOf("add");
			int bf_dell = str_request.indexOf("dell");

			//------------------------ load web --------------------
			//------------------------------------------------------

			if ((bf_get != -1) && (bf_add == -1) && (bf_dell == -1))
			{
				Client_STA.flush();
				Client_STA.print(str_html);
				Client_STA.flush();
			}

			//------------------------ add name --------------------
			//------------------------------------------------------

			if ((bf_get != -1) && (bf_add != -1) && (bf_dell == -1))
			{
				int stt = 0;

				String ts;

				String name_request = take_name_from_request(str_request);

				for (char i = 0; i < muber_max_user; i++)
				{
					nameString[i] = take_name_from_html(str_html, list_href[i]);

					if (nameString[i] == "NULL")
					{
						stt = i;
						break;
					}
					stt = 0xFF;
				}

				for (char i = 0; i < muber_max_user; i++)
				{
					nameString[i] = take_name_from_html(str_html, list_href[i]);

					if (name_request == nameString[i])
					{
						stt = 0xFF;
						break;
					}
				}

				if (stt != 0xFF)
				{
					char data_n[4];
					data_n[3] = 0;
					sprintf(data_n, "a%d>", stt);
					ts = add_name_in_html(Str2, name_request, data_n);
				}
				else
				{
					ts = Str2;
				}

				//-------------------------------------------
				//-------------------------------------------

				String add2String = Str1 + ts;
				Client_STA.flush();
				Client_STA.print(add2String);
				Client_STA.flush();

				//-------------------------------------------

				char data_send_ad[7];
				data_send_ad[6] = 0;
				sprintf(data_send_ad, "#a%d/", stt);
				Serial.println(data_send_ad);

				//-------------------------------------------
				write_files_in_SPIFFS(add2String, htmlfile);
			}

			//---------------------- delete name -------------------
			//------------------------------------------------------

			if ((bf_get != -1) && (bf_add == -1) && (bf_dell != -1))
			{
				String name_request = take_name_from_request(str_request);

				String name_html = take_name_from_delete( str_html , name_request );

				//--------------------------------------------------------

				int ID;

				if (name_html.equalsIgnoreCase(name_request))
				{
					for (char i = 0; i < muber_max_user; i++)
					{
						nameString[i] = take_name_from_html(str_html, list_href[i]);

						if (nameString[i] == name_request)
						{
							ID = i;
							break;
						}

						ID = 0xFF;
					}

					str_html.replace(">"+name_request+"<", ">NULL<");
				}
				else
				{
					name_html = "NULL";
				}

				//--------------------------------------------------------

				Client_STA.flush();
				Client_STA.print(str_html);
				Client_STA.flush();

				//--------------------------------------------------------

				char data_send_de[7];
					 data_send_de[6] = 0;

				if (name_html == "NULL")
				{
					sprintf(data_send_de, "#d255/");
					Serial.println(data_send_de);
				}
				else
				{
					sprintf(data_send_de, "#d%d/", ID);  //thay 1 bằng id vừa lấy 
					Serial.println(data_send_de);
				}

				//--------------------------------------------------------

				write_files_in_SPIFFS(str_html, htmlfile);

			}

		}
	}
	delay(10);
}

//-----------------------------------------

//================================================//
//======================= MAIN ===================//
//================================================//

void setup()
{
	Serial.begin(9600);
	WiFi.begin();
	SPIFFS.begin();
	Server_web.begin();
	WiFi.mode(WIFI_AP);
	WiFi.softAPConfig(local_IP, gateway, subnet);
	WiFi.softAP(ssid, password);
	//---------------------------------------------
	pinMode(PIN_BUTTON, INPUT_PULLUP);
	//----------------------------------------------

	if (digitalRead(PIN_BUTTON) == 0) {  }

	//----------------------------------------------

	timer.setInterval(0L, main_web);
	//timer.setInterval(0L, main_system);
	//----------------------------------------------
}

void loop()
{
	timer.run();
}

//-----------------------------------------
