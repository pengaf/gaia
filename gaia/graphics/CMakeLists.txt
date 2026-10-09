set(source_files
    ${CMAKE_CURRENT_SOURCE_DIR}/pass.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/compiled_render_graph.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/render_graph.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/render_graph_compiler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/static_mesh_renderable_3d.cpp
)

set(header_files
    ${CMAKE_CURRENT_SOURCE_DIR}/common.h
    ${CMAKE_CURRENT_SOURCE_DIR}/compiled_render_graph.h
    ${CMAKE_CURRENT_SOURCE_DIR}/depth_stencil_view.h
    ${CMAKE_CURRENT_SOURCE_DIR}/forward_renderer.h
    ${CMAKE_CURRENT_SOURCE_DIR}/frame_graph.h
    ${CMAKE_CURRENT_SOURCE_DIR}/material.h
    ${CMAKE_CURRENT_SOURCE_DIR}/mesh.h
    ${CMAKE_CURRENT_SOURCE_DIR}/parallel_pass.h
    ${CMAKE_CURRENT_SOURCE_DIR}/pass.h
    ${CMAKE_CURRENT_SOURCE_DIR}/render_graph.h
    ${CMAKE_CURRENT_SOURCE_DIR}/render_graph_compiler.h
    ${CMAKE_CURRENT_SOURCE_DIR}/render_output.h
    ${CMAKE_CURRENT_SOURCE_DIR}/render_state.h
    ${CMAKE_CURRENT_SOURCE_DIR}/render_target_view.h
    ${CMAKE_CURRENT_SOURCE_DIR}/render_window.h
    ${CMAKE_CURRENT_SOURCE_DIR}/renderable.h
    ${CMAKE_CURRENT_SOURCE_DIR}/renderable_2d.h
    ${CMAKE_CURRENT_SOURCE_DIR}/renderable_3d.h
    ${CMAKE_CURRENT_SOURCE_DIR}/serial_pass.h
    ${CMAKE_CURRENT_SOURCE_DIR}/shader_resource_view.h
    ${CMAKE_CURRENT_SOURCE_DIR}/static_mesh_renderable_2d.h
    ${CMAKE_CURRENT_SOURCE_DIR}/static_mesh_renderable_3d.h
    ${CMAKE_CURRENT_SOURCE_DIR}/unordered_access_view.h
    ${CMAKE_CURRENT_SOURCE_DIR}/utility.h
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

gaia_register_source_group("graphics/Source Files" FILES ${source_files})
gaia_register_source_group("graphics/Header Files" FILES ${header_files})
gaia_register_source_group("graphics/Interface Files" FILES ${interface_files})
gaia_register_source_group("graphics" FILES ${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt)

add_subdirectory(hal)
add_subdirectory(vk1)
add_subdirectory(rg)