project(sandbox)

# Source files
set(SRC sandbox_main.cpp sandbox_application.cpp sandbox_application.h)

# Includes
set(INCLUDE src)

add_executable(${PROJECT_NAME} ${SRC})
target_include_directories(${PROJECT_NAME} PRIVATE ${INCLUDE})

# Enable engine asserts in release build
add_compile_definitions(_ASSERTS_ENABLE=1)

set(LIBS engine)

target_link_libraries(${PROJECT_NAME} PRIVATE ${LIBS})

# add resources
add_resource_dir(resources)