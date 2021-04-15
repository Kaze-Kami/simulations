project(engine)

# Source files
set(SRC
        # entry point
        src/core/entry_point.h

        # core/application
        src/core/application/application.h
        src/core/application/application.cpp

        # core/message_queue
        src/core/message_queue/message_queue.h
        src/core/message_queue/message_queue.cpp

        # core/events
        src/core/events/event.h
        src/core/events/window_events.h
        src/core/events/mouse_events.h
        src/core/events/key_events.h

        # core/input
        src/core/input/key_codes.h
        src/core/input/mouse_codes.h
        src/core/input/input_controller.h

        # core/logging
        src/core/logging/logger.h
        src/core/logging/logger.cpp
        src/core/logging/engine_log.h
        src/core/logging/app_log.h

        # core/context
        src/core/renderer/context/context.h
        src/core/renderer/renderSurface.h

        # core/renderer
        src/core/renderer/shader_program/shader.h
        src/core/renderer/shader_program/shader.cpp
        src/core/renderer/shader_program/shader_program.h
        src/core/renderer/shader_program/shader_program.cpp
        src/core/renderer/shader_program/shader_list.cpp
        src/core/renderer/shader_program/shader_list.h
        src/core/renderer/buffer/buffer.h
        src/core/renderer/vertex_array/vertex_array.h
        src/core/renderer/vertex_array/vertex_array_layout.h

        # core/window
        src/core/window/window.h
        src/core/window/window_props.h

        # core/instrumentor
        src/core/instrumentor/instrumentor.cpp
        src/core/instrumentor/instrumentor.h

        # macros
        src/macros/definitions.h
        src/macros/assert.h
        src/macros/debug.h
        src/macros/bind.h
        src/macros/gl_call.h
        src/macros/no_op.h
        src/macros/timing.h

        # platform/glfw
        src/platform/glfw/impl_windows.cpp
        src/platform/glfw/glfw_window.h
        src/platform/glfw/glfw_window.cpp
        src/platform/glfw/glfw_input_controller.cpp
        src/platform/glfw/glfw_input_controller.h

        # platform/opengl
        src/platform/opengl/impl_opengl.cpp
        src/platform/opengl/opengl_context.h
        src/platform/opengl/opengl_context.cpp
        src/core/renderer/camera/camera.h src/core/renderer/camera/camera_controller.h)

# Includes
set(INCLUDE src)

# Create the library
add_library(${PROJECT_NAME} STATIC ${SRC})
target_include_directories(${PROJECT_NAME} PUBLIC ${INCLUDE})

# Definitions for GLFW

# add_compile_definitions(GLFW_INCLUDE_NONE=1)

# Vendor (3rd party)
function(add_vendor_folder NAME)
    add_subdirectory(vendor/${NAME})
    target_link_libraries(${PROJECT_NAME} PUBLIC ${NAME})
endfunction()

add_vendor_folder(Glad)
add_vendor_folder(glfw)
add_vendor_folder(glm)
add_vendor_folder(spdlog)