set(source_files
    ${CMAKE_CURRENT_SOURCE_DIR}/buffer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/render_graph_compiler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/render_pass.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/render_system.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/render_window.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/texture.cpp
)
set(header_files
    ${CMAKE_CURRENT_SOURCE_DIR}/buffer.h
    ${CMAKE_CURRENT_SOURCE_DIR}/common.h
    ${CMAKE_CURRENT_SOURCE_DIR}/compute_element.h
    ${CMAKE_CURRENT_SOURCE_DIR}/graphics_pipeline.h
    ${CMAKE_CURRENT_SOURCE_DIR}/hal_fwd.h
    ${CMAKE_CURRENT_SOURCE_DIR}/mesh_shader_element.h
    ${CMAKE_CURRENT_SOURCE_DIR}/ray_tracing_element.h
    ${CMAKE_CURRENT_SOURCE_DIR}/render_element.h
    ${CMAKE_CURRENT_SOURCE_DIR}/render_graph_compiler.h
    ${CMAKE_CURRENT_SOURCE_DIR}/render_pass.h
    ${CMAKE_CURRENT_SOURCE_DIR}/render_system.h
    ${CMAKE_CURRENT_SOURCE_DIR}/render_window.h
    ${CMAKE_CURRENT_SOURCE_DIR}/shader.h
    ${CMAKE_CURRENT_SOURCE_DIR}/texture.h
    ${CMAKE_CURRENT_SOURCE_DIR}/utility.h
    ${CMAKE_CURRENT_SOURCE_DIR}/vertex_shader_element.h
    ${CMAKE_CURRENT_SOURCE_DIR}/view.h
)
set(interface_files)

target_sources(gaia
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt
        ${source_files}
    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS ${GAIA_SOURCE_DIR}
        FILES
            ${header_files}
)

gaia_register_source_group("graphics/vk1/Source Files" FILES ${source_files})
gaia_register_source_group("graphics/vk1/Header Files" FILES ${header_files})
gaia_register_source_group("graphics/vk1/Interface Files" FILES ${interface_files})
gaia_register_source_group("graphics/vk1" FILES ${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt)

add_subdirectory(detail)
