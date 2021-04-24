set(DOCUMENTATION "Blah")

function(add_resource_dir NAME)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_CURRENT_SOURCE_DIR}/${NAME}/ $<TARGET_FILE_DIR:${PROJECT_NAME}>/${NAME})
endfunction()