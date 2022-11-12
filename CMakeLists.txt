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

# Set Macro Definitions
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DUNICODE -D_UNICODE -D_SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING")

#Build the Engine
add_subdirectory(Engine)

#Build the game
file(GLOB_RECURSE GAME_CPP_FILES "Game/*.cpp")
file(GLOB_RECURSE GAME_HEADER_FILES "Game/*.h")

include_directories(Engine/inc)
include_directories(Game/)
include_directories("$ENV{DXSDK_DIR}/include")

add_executable(${PROJECT_NAME} WIN32 ${GAME_CPP_FILES} ${GAME_HEADER_FILES})

target_link_libraries(${PROJECT_NAME} CatalystEngine Effects11 DirectXTK)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)

set_target_properties(${PROJECT_NAME} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
						VS_DEBUGGER_COMMAND "$<TARGET_FILE:${PROJECT_NAME}>"	)

# Copy Assets to outdir
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/Assets ${CMAKE_BINARY_DIR})

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
