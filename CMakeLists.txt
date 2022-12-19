cmake_minimum_required(VERSION 3.18)
project( Project_Phoinix VERSION 0.0.0 )
set(CMAKE_CXX_STANDARD 20)

if (WIN32)
    set_property(GLOBAL PROPERTY USE_FOLDERS ON)
endif()

if (NOT WIN32)
    option(CMAKE_POSITION_INDEPENDENT_CODE "CMAKE_POSITION_INDEPENDENT_CODE" ON)
endif()

# Add sub-directories
# Engine Library
add_subdirectory(Phoinix)
# Main Executable
add_subdirectory(Raytracer)

if (MSVC)
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Raytracer)
endif()

# Testing Module
# add_subdirectory(PTests)
# include(CTest)
# enable_testing()
# add_executable(tester tests/test_memory.cpp)
# target_link_libraries(tester PRIVATE glfw)
# target_link_libraries(tester PRIVATE libglew_static)
# target_link_libraries(tester PRIVATE glm)
# add_test(Tester tester)