if (MSVC)
    add_compile_definitions(PHOENIX_WINDOWS)
endif()
add_compile_definitions(PHOENIX_LIB)

file(GLOB_RECURSE Phoenix_CPP "src/*.cpp")
file(GLOB_RECURSE Phoenix_H "src/*.h")
file(GLOB_RECURSE Phoenix_SHADER "src/*.vert" "src/*.frag")

# Phoenix Library
add_library(
    Phoenix
    ${Phoenix_CPP}
    ${Phoenix_H}
    ${Phoenix_SHADER}
)

# GLFW
add_subdirectory( ${CMAKE_CURRENT_SOURCE_DIR}/external/glfw modules/glfw )
target_link_libraries(Phoenix PUBLIC glfw)

# GLM
add_subdirectory( ${CMAKE_CURRENT_SOURCE_DIR}/external/glm modules/glm EXCLUDE_FROM_ALL )
target_link_libraries(Phoenix PUBLIC glm)

# spdlog
add_subdirectory( ${CMAKE_CURRENT_SOURCE_DIR}/external/spdlog modules/spdlog)
target_link_libraries(Phoenix PUBLIC spdlog::spdlog)


# Vulkan
find_package(Vulkan REQUIRED)
# find_package(Vulkan REQUIRED COMPONENTS glslc)
# find_program(GLSLC_EXECUTABLE NAMES glslc HINTS Vulkan::glslc)
# message("Vulkan found? " ${Vulkan_FOUND})
# message("Alright, where is it? " ${Vulkan_LIBRARY})
# message("And I can include it? " ${Vulkan_INCLUDE_DIR})
target_link_libraries(Phoenix PUBLIC Vulkan::Vulkan)

# Dear ImGUI
add_subdirectory( ${CMAKE_CURRENT_SOURCE_DIR}/external/imgui modules/imgui)
target_link_libraries(Phoenix PUBLIC imgui)
add_compile_definitions(IMGUI_USER_CONFIG="${CMAKE_CURRENT_SOURCE_DIR}/src/ImGui/ImConfig.h")

# stb
target_include_directories(Phoenix PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/external/stb")

target_include_directories(Phoenix PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")

target_precompile_headers(Phoenix PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src/pch.h")


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
    COMMAND ${Vulkan_GLSLC_EXECUTABLE} -o ${SPIRV} ${GLSL}
    DEPENDS ${GLSL}
    COMMENT "Compiling ${FILE_NAME}"
  )
  list(APPEND SPIRV_BINARY_FILES ${SPIRV})
endforeach(GLSL)

add_custom_target(
    Shaders_Engine
    DEPENDS ${SPIRV_BINARY_FILES}
    )

add_dependencies(Phoenix Shaders_Engine)

# Setup files in VS
if(WIN32)
    file(GLOB_RECURSE PHOENIX_SRC RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "*.cpp" "*.h" "*.vert" "*.frag")
    # message("Alright, my PHOENIX_SRC files: " ${PHOENIX_SRC})
    source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${PHOENIX_SRC})
endif()