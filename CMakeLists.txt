cmake_minimum_required(VERSION 3.20)  # Minimum CMake version

project(Vulkan LANGUAGES CXX)  

# C++ settings
set(CMAKE_CXX_STANDARD 20)            
set(CMAKE_CXX_STANDARD_REQUIRED ON)   


# Add executable from source files
file(GLOB_RECURSE CPP_SOURCES "src/*.cpp")    # C++ sources
file(GLOB_RECURSE HEADER_FILES "include/*.hpp")  # Header files
add_executable(${PROJECT_NAME} ${CPP_SOURCES} ${HEADER_FILES})


# Find and link GLM, GLFW, Vulkan
find_package(glfw3 CONFIG REQUIRED)
find_package(Vulkan REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(Freetype REQUIRED)


# Link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE 
    glfw 
    Vulkan::Vulkan
    glm::glm 
    imgui::imgui
    Freetype::Freetype
)

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE 
    "include"
)

# Add a definition so we can toggle validation layers in our C++ code
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(${PROJECT_NAME} PRIVATE DEBUG_MODE)
endif()

