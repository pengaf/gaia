target_sources(gaia
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt
        buffer.cpp
        render_system.cpp
        render_window.cpp
        texture.cpp
    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS ${GAIA_SOURCE_DIR}
        FILES
            buffer.h
            compute_element.h
            compute_pass.h
            graphics_pipeline.h
            hal_fwd.h
            mesh_shader_element.h
            ray_tracing_element.h
            render_element.h
            render_pass.h
            render_system.h
            render_window.h
            shader.h
            texture.h
            utility.h
            vertex_shader_element.h
            view.h
)

gaia_register_source_group("graphics\\vk1" FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/buffer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/buffer.h
    ${CMAKE_CURRENT_SOURCE_DIR}/compute_element.h
    ${CMAKE_CURRENT_SOURCE_DIR}/compute_pass.h
    ${CMAKE_CURRENT_SOURCE_DIR}/graphics_pipeline.h
    ${CMAKE_CURRENT_SOURCE_DIR}/hal_fwd.h
    ${CMAKE_CURRENT_SOURCE_DIR}/mesh_shader_element.h
    ${CMAKE_CURRENT_SOURCE_DIR}/ray_tracing_element.h
    ${CMAKE_CURRENT_SOURCE_DIR}/render_element.h
    ${CMAKE_CURRENT_SOURCE_DIR}/render_pass.h
    ${CMAKE_CURRENT_SOURCE_DIR}/render_system.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/render_system.h
    ${CMAKE_CURRENT_SOURCE_DIR}/render_window.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/render_window.h
    ${CMAKE_CURRENT_SOURCE_DIR}/shader.h
    ${CMAKE_CURRENT_SOURCE_DIR}/texture.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/texture.h
    ${CMAKE_CURRENT_SOURCE_DIR}/utility.h
    ${CMAKE_CURRENT_SOURCE_DIR}/vertex_shader_element.h
    ${CMAKE_CURRENT_SOURCE_DIR}/view.h
)

add_subdirectory(detail)
gaia_register_source_group("graphics\\vk1" FILES ${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt)
