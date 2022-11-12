# Check that all repositories have been successfully cloned
if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/ASSIMP/CMakeLists.txt")
	message(FATAL_ERROR "ASSIMP was not cloned successfully! Please try again.")
endif()
if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/FX11/CMakeLists.txt")
	message(FATAL_ERROR "FX11 was not cloned successfully! Please try again.")
endif()

if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/DirectXTK/CMakeLists.txt")
	message(FATAL_ERROR "DirectXTK was not cloned successfully! Please try again.")
endif()


# Configure dependencies
set(BUILD_SHARED_LIBS OFF)
set(FX_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/FX11/inc/")

# Build each dependency in turn
add_subdirectory(ASSIMP)
add_subdirectory(FX11)
add_subdirectory(DirectXTK)



