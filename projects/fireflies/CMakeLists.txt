project(fireflies)

# Source files
set(SRC fireflies_main.cpp fireflies_application.cpp fireflies_application.h)

set(RESOURCES
        resources/nudge_compute_shader.glsl
        resources/fragment_shader.glsl
        resources/geometry_shader.glsl
        resources/vertex_shader.glsl
        )

# Includes
set(INCLUDE src)

add_executable(${PROJECT_NAME} ${SRC} ${RESOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE ${INCLUDE})

# Enable engine asserts in release build
add_compile_definitions(_ASSERTS_ENABLE=1)

set(LIBS engine)

target_link_libraries(${PROJECT_NAME} PRIVATE ${LIBS})

# add resources
add_resource_dir(resources)