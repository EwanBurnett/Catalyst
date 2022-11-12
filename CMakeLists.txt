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

add_subdirectory(External)

# Find and validate all local subdependencies

set(GLOBAL PROPERTY FIND_LIBRARY_USE_LIB64_PATHS TRUE)

# D3D11.lib
find_library(D3D11_LIB d3d11.lib PATHS "$ENV{DXSDK_DIR}/Lib/x64" "C:/Program Files (x86)/Microsoft DirectX SDK/Lib/x64"     DOC "The directory where d3d11.lib resides")
message(STATUS "Search for DirectX 11 returned directory: ${D3D11_LIB}")

# D3DCOMPILER.lib
find_library(D3DCOMPILER_LIB d3dcompiler.lib "$ENV{DXSDK_DIR}/Lib/x64" "C:/Program Files (x86)/Microsoft DirectX SDK/Lib/x64" REGISTRY_VIEW 64 DOC "The directory where d3dcompiler.lib resides")
message(STATUS "Search for D3DCompiler returned directory: ${D3DCOMPILER_LIB}")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DUNICODE -D_UNICODE -D_SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING")

#Build the Engine
add_subdirectory(Engine)

#Build the game
file(GLOB_RECURSE GAME_CPP_FILES "Game/*.cpp")
file(GLOB_RECURSE GAME_HEADER_FILES "Game/*.h")

include_directories(Engine/inc)
include_directories(Game/)
add_executable(${PROJECT_NAME} WIN32 ${GAME_CPP_FILES} ${GAME_HEADER_FILES})

target_link_libraries(${PROJECT_NAME} CatalystEngine)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)

#Build Docs
find_package(Doxygen QUIET)
if(DOXYGEN_FOUND AND EXISTS "${PROJECT_SOURCE_DIR}/Docs/Catalyst")
	#Build Documentation optionally
	option(BUILD_DOCS "Build Catalyst Documentation" ON)
	if(BUILD_DOCS)
		message(STATUS "Building Documentation")
		execute_process(COMMAND ${DOXYGEN_EXECUTABLE} Catalyst WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/Docs/" RESULT_VARIABLE DOXY_RESULT)
		if(NOT DOXY_RESULT EQUAL "0")
			message(FATAL_ERROR"command [Doxygen Catalayst] failed with status ${DOXY_RESULT}")
		endif()
	endif()
endif()
