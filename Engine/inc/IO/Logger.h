#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <cstdarg>
#include <stdio.h>
#include <fstream>
#include "..\Framework.h"
#include "..\Core\Time.h"

#define HR(x, msg) {HRESULT hr; if(FAILED(hr = (x))){ Engine::LogTime(); Engine::Log("\nError: "); Engine::Log(msg); Engine::Log("\nFILE:\t%s\nLINE:\t%d\n", __FILE__, __LINE__); assert(false && msg);}}
#define HR_WARN(x, msg) {HRESULT hr; if(FAILED(hr = (x))){ Engine::LogTime(); Engine::Log("\nError: "); Engine::Log(msg); Engine::Log("\nFILE:\t%s\nLINE:\t%d\n", __FILE__, __LINE__);}}
#define ERR(x, msg) {if((x)){ Engine::LogTime(); Engine::Log("\nError: "); Engine::Log(msg); Engine::Log("\nFILE:\t%s\nLINE:\t%d\n", __FILE__, __LINE__); assert(false && msg);}}
#define WARN(x, msg) {if((x)){ Engine::LogTime(); Engine::Log("\Warning: "); Engine::Log(msg); Engine::Log("\nFILE:\t%s\nLINE:\t%d\n", __FILE__, __LINE__);}}
#define ALIGNUP(adr, bytes)  { (((uint64_t)(char*)(adr) + (bytes) - 1) & (~((bytes) - 1)));}

constexpr bool ENABLE_LOGGING = true;   //TODO: Config file
constexpr bool LOG_TO_FILE = true;

namespace Engine {
    void Log(const char* fmt, ...);
    void LogTime();
}
