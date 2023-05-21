// 
// 
// 

#include "Function_web.h"

String load_web_page_demo(void)
{
	String s = "<!DOCTYPE html>\n";
	s += "<!DOCTYPE html>\n";
	s += "<html>\n";
	s += "<body>\n";
	s += "<form action=\"/upload\">\n";
	s += "Select a file: <input type=\"file\" name=\"myFile\">\n";
	s += "<input type=\"submit\">\n";
	s += "</form>\n";
	s += "</body>\n";
	s += "</html>"; ;
	s += "";
	return s;
}

String load_web_page_root(void)
{
	return String();
}

String load_web_page_upload(void)
{
	String s = "<!DOCTYPE html>\n";
	s += "<html>\n";
	s += "<body>\n";
	s += "<form action=\"fupload\" method=\"post\" enctype=\"multipart/form-data\">\n";
	s += "<input type=\"file\" name=\"fileToUpload\">\n";
	s += "<input type=\"submit\" value=\"Upload\" name=\"submit\">\n";
	s += "</form>\n";
	s += "</body>\n";
	s += "</html>";

	return s;
}

String load_web_page_download(void)
{	  
	String s = "<!DOCTYPE html>\n";
	s += "<html>\n";
	s += "<body>\n";
	s += "<p>Hien thi hinh anh dc tai len tu bo nho SPIFFS :</p>\n";
	s += "<img src=\"\\img\" alt=\"Image from ESP8266\" width=\"400\" height=\"200\">\n";
	s += "</body>\n";
	s += "</html>";
	return s;
}
