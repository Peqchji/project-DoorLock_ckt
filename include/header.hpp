#include <ESP8266WiFi.h>
#include <Arduino.h>
#include <Scheduler.h>
#include <Task.h>
#include <LeanTask.h>

// Define Input port of esp8266 that connect to Signal port of mux
#define D0 16

// Define signal send port from esp8266 to mux 
#define S0 5
#define S1 4
#define S2 0
#define S3 2