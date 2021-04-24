project(n_bodies)

# Source files
set(SRC n_bodies_main.cpp n_bodies_application.h n_bodies_application.cpp)

# Includes
set(INCLUDE src)

add_executable(${PROJECT_NAME} ${SRC})
target_include_directories(${PROJECT_NAME} PRIVATE ${INCLUDE})

# Enable engine asserts in release build
add_compile_definitions(_ASSERTS_ENABLE=1)

set(LIBS engine)

target_link_libraries(${PROJECT_NAME} PRIVATE ${LIBS})