if (WIN32)
    add_compile_definitions(PHOENIX_WINDOWS)
endif()

file(GLOB_RECURSE Sandbox_CPP "src/*.cpp")
file(GLOB_RECURSE Sandbox_H "src/*.h")
file(GLOB_RECURSE Sandbox_SHADER "src/*.vert" "src/*.frag")

# Sandbox EXE
add_executable(
    Sandbox
    ${Sandbox_CPP}
    ${Sandbox_H}
    ${Sandbox_SHADER}
    )

target_link_libraries(Sandbox PUBLIC Phoenix)


if(WIN32)
	file(GLOB_RECURSE SANDBOX_SRC RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "*.cpp" "*.h" "*.vert" "*.frag")
	source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SANDBOX_SRC})
endif()

# Shader Stuff
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
    Shaders_App
    DEPENDS ${SPIRV_BINARY_FILES}
    )

add_dependencies(Sandbox Shaders_App)

add_custom_command(TARGET Sandbox POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:Sandbox>/shaders/"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${PROJECT_BINARY_DIR}/shaders"
        "$<TARGET_FILE_DIR:Sandbox>/shaders"
        )
if(MSVC)
    add_custom_command(TARGET Sandbox POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:Phoenix>/shaders/"
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${PROJECT_BINARY_DIR}/shaders"
            "$<TARGET_FILE_DIR:Sandbox>/../shaders"
            )
endif()

# Setup files in VS
if(WIN32)
    file(GLOB_RECURSE Sandbox_SRC RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "*.cpp" "*.h" "*.vert" "*.frag")
    source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${Sandbox_SRC})
endif()