# Check that all repositories have been successfully cloned
if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/ASSIMP/CMakeLists.txt")
	message(FATAL_ERROR "ASSIMP was not cloned successfully! Please try again.")
endif()

# Configure dependencies
set(BUILD_SHARED_LIBS OFF)

# Build each dependency in turn
add_subdirectory(ASSIMP)


