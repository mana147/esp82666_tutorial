/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
  /* USER CODE END Header */

  /* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus

extern "C"
{
#endif

/* Includes ------------------------------------------------------------------*/

#include <WebSockets.h>

#include <WiFiClientSecure.h>

#include <ESP8266WebServer.h>

#include "lib/ESP8266HTTPClient.h"

#include <WebSocketsClient.h>

#include <WebSocketsServer.h>

#include <ESP8266WiFi.h>

#include <ESP8266Ping.h>

#include <ArduinoJson.h>

#include "Function_detech.h"

#include "Function_web.h"

#include <Hash.h>

#include "FS.h"

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
