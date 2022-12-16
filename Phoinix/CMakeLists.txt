if (MSVC)
    add_compile_definitions(PHOINIX_WINDOWS)
endif()
add_compile_definitions(PHOINIX_LIB)

file(GLOB_RECURSE Phoinix_CPP "src/*.cpp")
file(GLOB_RECURSE Phoinix_H "src/*.h")
file(GLOB_RECURSE Phoinix_SHADER "src/*.vert" "src/*.frag")

# Phoinix Library
add_library(
    Phoinix
    ${Phoinix_CPP}
    ${Phoinix_H}
    ${Phoinix_SHADER}
)

# GLFW
add_subdirectory( ${CMAKE_CURRENT_SOURCE_DIR}/external/glfw modules/glfw )
target_link_libraries(Phoinix PUBLIC glfw)

# GLM
add_subdirectory( ${CMAKE_CURRENT_SOURCE_DIR}/external/glm modules/glm EXCLUDE_FROM_ALL )
target_link_libraries(Phoinix PUBLIC glm)

# spdlog
add_subdirectory( ${CMAKE_CURRENT_SOURCE_DIR}/external/spdlog modules/spdlog)
target_link_libraries(Phoinix PUBLIC spdlog::spdlog)


# Vulkan
find_package(Vulkan REQUIRED)
# message("Vulkan found? " ${Vulkan_FOUND})
# message("Alright, where is it? " ${Vulkan_LIBRARY})
# message("And I can include it? " ${Vulkan_INCLUDE_DIR})
target_link_libraries(Phoinix PUBLIC Vulkan::Vulkan)

# Dear ImGUI
add_subdirectory( ${CMAKE_CURRENT_SOURCE_DIR}/external/imgui modules/imgui)
target_link_libraries(Phoinix PUBLIC imgui)

target_include_directories(Phoinix PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")

target_precompile_headers(Phoinix PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src/pch.h")

# Shader Stuff
# TODO shaders are not copied properly for using VS
if (WIN32)
    message("${Vulkan_GLSLANG_VALIDATOR_EXECUTABLE}")
    message("${Vulkan_GLSLC_EXECUTABLE}")
    set(GLSL_VALIDATOR "${Vulkan_GLSLANG_VALIDATOR_EXECUTABLE}")
    set(GLSL_COMPILER "${Vulkan_GLSLC_EXECUTABLE}")
    else()
    message(FATAL_ERROR "This platform is not supported for shader compilation from this")
    endif()
    
    file(GLOB_RECURSE SHADER_SRC 
    "src/shaders/*.vert"
    "src/shaders/*.frag"
    )
    
foreach(GLSL ${SHADER_SRC})
  get_filename_component(FILE_NAME ${GLSL} NAME)
  set(SPIRV "${PROJECT_BINARY_DIR}/shaders/${FILE_NAME}.spv")
  add_custom_command(
    OUTPUT ${SPIRV}
    COMMAND ${CMAKE_COMMAND} -E make_directory "${PROJECT_BINARY_DIR}/shaders/"
    COMMAND ${GLSL_VALIDATOR} -V ${GLSL} -o ${SPIRV}
    DEPENDS ${GLSL})
  list(APPEND SPIRV_BINARY_FILES ${SPIRV})
  endforeach(GLSL)
  
add_custom_target(
    Shaders_Engine 
    DEPENDS ${SPIRV_BINARY_FILES}
    )
    
add_dependencies(Phoinix Shaders_Engine)

# Setup files in VS
if(WIN32)
    file(GLOB_RECURSE PHOINIX_SRC RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "*.cpp" "*.h" "*.vert" "*.frag")
    # message("Alright, my PHOINIX_SRC files: " ${PHOINIX_SRC})
    source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${PHOINIX_SRC})
endif()