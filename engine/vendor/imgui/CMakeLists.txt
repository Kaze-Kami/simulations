set(SRC
        imgui.cpp
        imgui_demo.cpp
        imgui_draw.cpp
        imgui_tables.cpp
        imgui_widgets.cpp

        imconfig.h
        imgui.h
        imstb_rectpack.h
        imstb_textedit.h
        imstb_truetype.h

        backends/imgui_impl_glfw.h
        backends/imgui_impl_glfw.cpp
        backends/imgui_impl_opengl3.h
        backends/imgui_impl_opengl3.cpp
        )

set(INCLUDE ../imgui)

add_library(imgui STATIC ${SRC})
target_link_libraries(imgui PRIVATE glfw)
target_link_libraries(imgui PRIVATE Glad)
target_include_directories(imgui PUBLIC ${INCLUDE})

add_executable(imgui_opengl_glfw3_glad_example ${SRC} examples/example_glfw_opengl3/main.cpp)
target_link_libraries(imgui_opengl_glfw3_glad_example PRIVATE glfw)
target_link_libraries(imgui_opengl_glfw3_glad_example PRIVATE Glad)
target_include_directories(imgui_opengl_glfw3_glad_example PUBLIC ${INCLUDE})

add_compile_definitions(IMGUI_IMPL_OPENGL_LOADER_GLAD=1 IMGUI_DISABLE_WIN32_FUNCTIONS=1)