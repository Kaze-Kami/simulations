set(DOCUMENTATION "Blah")

function(add_resource_dir NAME)
    set(__src__ ${CMAKE_CURRENT_SOURCE_DIR}/${NAME})
    set(__dst__ ${CMAKE_CURRENT_BINARY_DIR}/${NAME})

    set(__dummy__ ${CMAKE_CURRENT_BINARY_DIR}/_${PROJECT_NAME}_${NAME}_.dummy)
    set(__target__ copy-resources-${PROJECT_NAME}-${NAME})

    add_custom_command(
            OUTPUT
                ${__dummy__}
                ${__dst__}

            DEPENDS
                ${__src__}

            COMMENT "Copying resources for ${PROJECT_NAME}: ${__src__} -> ${__dst__}"

            COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${__src__}
                ${__dst__}
    )

    add_custom_target(
            ${__target__}
            DEPENDS ${__dummy__}
    )

    add_dependencies(${PROJECT_NAME} ${__target__})
endfunction()