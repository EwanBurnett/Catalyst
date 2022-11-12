project(CatalystEngine VERSION 1.0)

#Configure the Verisoning File
configure_file(CatalystConfig.h.in CatalystConfig.h)

#Add all files to be compiled
file(GLOB_RECURSE ENGINE_CPP_FILES "src/*.cpp")
file(GLOB_RECURSE ENGINE_HEADER_FILES "inc/*.h")

#Propogate this project's include files to other projects
set(${PROJECT_NAME}_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/inc CACHE INTERNAL "${PROJECT_NAME}: Include Directories" FORCE)

#Build the library itself

add_library(${PROJECT_NAME} STATIC ${ENGINE_CPP_FILES} ${ENGINE_HEADER_FILES})

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_BINARY_DIR} ${ASSIMP_INCLUDE_DIR} "${Effects11_SOURCE_DIR}/inc/" ${DIRECTXTK_INCLUDE_DIR} )

#Configure the library
set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "Catalyst-Engine")

# Link subdependencies
target_link_libraries(${PROJECT_NAME} PRIVATE assimp DirectXTK Effects11 d3d11.lib d3dcompiler.lib) 
