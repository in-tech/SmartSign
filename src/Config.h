#pragma once

#define WIFI_TIMEOUT    10000 // ms
#define TCP_TIMEOUT     15000 // ms
#define LOW_BAT_VOLTAGE 3.6f

/* Pin definition */
#define VCC_PIN         13
#define RC522_SS_PIN    21
#define RC522_RST_PIN   17
#define ENDSTOP_PIN     25
#define BUTTONA_PIN     26
#define BUTTONB_PIN     27
#define LED_PIN         5
#define BAT_LEVEL_PIN   36

#define EPD_RST_PIN     4
#define EPD_DC_PIN      12
#define EPD_CS_PIN      14
#define EPD_BUSY_PIN    16

/* Display resolution */
#define EPD_WIDTH       640
#define EPD_HEIGHT      384

/* Time zone */

// Central European Time (Frankfurt, Paris)
#define TIMEZONE CE 

// Australia Eastern Time Zone (Sydney, Melbourne)
// #define TIMEZONE ausET 

// Moscow Standard Time (MSK, does not observe DST)
// #define TIMEZONE tzMSK

// United Kingdom (London, Belfast)
// #define TIMEZONE UK 

// US Eastern Time Zone (New York, Detroit)
// #define TIMEZONE usET 

// US Central Time Zone (Chicago, Houston)
// #define TIMEZONE usCT 

// US Mountain Time Zone (Denver, Salt Lake City)
// #define TIMEZONE usMT 

// Arizona is US Mountain Time Zone but does not use DST
// #define TIMEZONE usAZ 

// US Pacific Time Zone (Las Vegas, Los Angeles)
// #define TIMEZONE usPT