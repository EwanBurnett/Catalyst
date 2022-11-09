#include "../inc/Engine.h"
#include <cstdio>
#include <CatalystConfig.h>


void Engine::OutputGreeting(){
	printf("[%d.%d] - %s", ENGINE_VERSION_MAJOR, ENGINE_VERSION_MINOR, Greeting.c_str());		
}


