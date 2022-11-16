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
set(BUILD_SHARED_LIBS OFF CACHE INTERNAL "BUILD_SHARED_LIBS")
set(FX_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/FX11/inc/")

# Build each dependency in turn
add_subdirectory(ASSIMP)
add_subdirectory(FX11)
add_subdirectory(DirectXTK)

set(IMGUI_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/IMGUI/" CACHE INTERNAL "IMGUI_INCLUDE_DIR")
file(GLOB_RECURSE IMGUI_SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/IMGUI/*.cpp")

set(IMGUI_SOURCES 
	"${CMAKE_CURRENT_SOURCE_DIR}/IMGUI/imgui.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/IMGUI/imgui_demo.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/IMGUI/imgui_draw.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/IMGUI/imgui_widgets.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/IMGUI/imgui_tables.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/IMGUI/backends/imgui_impl_win32.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/IMGUI/backends/imgui_impl_dx11.cpp"

	CACHE INTERNAL "IMGUI_SOURCES")

