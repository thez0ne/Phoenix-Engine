if (WIN32)
    add_compile_definitions(PHOINIX_WINDOWS)
endif()

file(GLOB_RECURSE Raytracer_CPP "src/*.cpp")
file(GLOB_RECURSE Raytracer_H "src/*.h")
file(GLOB_RECURSE Raytracer_SHADER "src/*.vert" "src/*.frag")

# Raytracer EXE
add_executable(
    Raytracer
    ${Raytracer_CPP}
    ${Raytracer_H}
    ${Raytracer_SHADER}
    )

target_link_libraries(Raytracer PUBLIC Phoinix)


if(WIN32)
	file(GLOB_RECURSE RAYTRACER_SRC RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "*.cpp" "*.h" "*.vert" "*.frag")
	source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${RAYTRACER_SRC})
endif()

# # Shader Stuff
# file(GLOB_RECURSE SHADER_SRC
#     "src/shaders/*.vert"
#     "src/shaders/*.frag"
#     )

# foreach(GLSL ${SHADER_SRC})
#     get_filename_component(FILE_NAME ${GLSL} NAME)
#     set(SPIRV "${PROJECT_BINARY_DIR}/shaders/${FILE_NAME}.spv")
#     add_custom_command(
#         OUTPUT ${SPIRV}
#         COMMAND ${CMAKE_COMMAND} -E make_directory "${PROJECT_BINARY_DIR}/shaders/"
#         COMMAND ${Vulkan_GLSLC_EXECUTABLE} -o ${SPIRV} ${GLSL}
#         DEPENDS ${GLSL}
#         COMMENT "Compiling ${FILE_NAME}"
#     )
#     list(APPEND SPIRV_BINARY_FILES ${SPIRV})
# endforeach(GLSL)

# add_custom_target(
#     Shaders_App
#     DEPENDS ${SPIRV_BINARY_FILES}
#     )

# add_dependencies(Raytracer Shaders_App)

# add_custom_command(TARGET Raytracer POST_BUILD
#     COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:Raytracer>/shaders/"
#     COMMAND ${CMAKE_COMMAND} -E copy_directory
#         "${PROJECT_BINARY_DIR}/shaders"
#         "$<TARGET_FILE_DIR:Raytracer>/shaders"
#         )
# if(MSVC)
#     add_custom_command(TARGET Raytracer POST_BUILD
#         COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:Phoinix>/shaders/"
#         COMMAND ${CMAKE_COMMAND} -E copy_directory
#             "${PROJECT_BINARY_DIR}/shaders"
#             "$<TARGET_FILE_DIR:Raytracer>/../shaders"
#             )
# endif()

# Setup files in VS
if(WIN32)
    file(GLOB_RECURSE Raytracer_SRC RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "*.cpp" "*.h" "*.vert" "*.frag")
    source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${Raytracer_SRC})
endif()