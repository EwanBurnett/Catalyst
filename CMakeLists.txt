CMAKE_MINIMUM_REQUIRED(VERSION 3.14)

project(Catalyst VERSION 1.0)

#Validate all git submodules
find_package(Git QUIET)
if(GIT_FOUND AND EXISTS "${PROJECT_SOURCE_DIR}/.git")
	#Check submodules during build by default.
	option(GIT_SUBMODULES "Check Submodules during build" ON)
	if(GIT_SUBMODULES)
		message(STATUS "Checking Submodules...")
		execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} RESULT_VARIABLE GIT_SUBMOD_RESULT)
		if(NOT GIT_SUBMOD_RESULT EQUAL "0")
			message(FATAL_ERROR "command [git submodule update --init --recursive] failed with status ${GIT_SUBMOD_RESULT}")
		endif()
	endif()
elseif(NOT EXISTS "${PROJECT_SOURCE_DIR}/.git")
	message(FATAL_ERROR "Repository is invalid! Please re-clone. from source.")
elseif(NOT GIT_FOUND)
	message(FATAL_ERROR "Git Executable not found!")
endif()




#Build the Engine
add_subdirectory(Engine)

#Build the game
file(GLOB_RECURSE GAME_CPP_FILES "Game/*.cpp")
file(GLOB_RECURSE GAME_HEADER_FILES "Game/*.h")

include_directories(Engine/inc)
include_directories(Game/)
add_executable(${PROJECT_NAME} ${GAME_CPP_FILES} ${GAME_HEADER_FILES})
target_link_libraries(${PROJECT_NAME} CatalystEngine)
