/** \file main.cpp
* a debug library main file. 
*/

#include "../inc/Engine.h"
#include <cstdio>
#include <CatalystConfig.h>

/**
 * @brief A Debug function, Which prints the current version + a greeting message to the console.
 *
 * @return Nothing.
 *
 */
void Engine::OutputGreeting(){
	printf("[%d.%d] - %s", ENGINE_VERSION_MAJOR, ENGINE_VERSION_MINOR, Greeting.c_str());		
}


