target_sources(gaia
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt
        pass.cpp
        static_mesh_renderable_3d.cpp
    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS ${GAIA_SOURCE_DIR}
        FILES
            common.h
            depth_stencil_view.h
            forward_renderer.h
            frame_graph.h
            material.h
            mesh.h
            parallel_pass.h
            pass.h
            render_output.h
            render_pass.h
            render_state.h
            render_target_view.h
            render_window.h
            renderable.h
            renderable_2d.h
            renderable_3d.h
            serial_pass.h
            shader_resource_view.h
            static_mesh_renderable_2d.h
            static_mesh_renderable_3d.h
            unordered_access_view.h
            utility.h
)

gaia_register_source_group("graphics" FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/common.h
    ${CMAKE_CURRENT_SOURCE_DIR}/depth_stencil_view.h
    ${CMAKE_CURRENT_SOURCE_DIR}/forward_renderer.h
    ${CMAKE_CURRENT_SOURCE_DIR}/frame_graph.h
    ${CMAKE_CURRENT_SOURCE_DIR}/material.h
    ${CMAKE_CURRENT_SOURCE_DIR}/mesh.h
    ${CMAKE_CURRENT_SOURCE_DIR}/parallel_pass.h
    ${CMAKE_CURRENT_SOURCE_DIR}/pass.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/pass.h
    ${CMAKE_CURRENT_SOURCE_DIR}/render_output.h
    ${CMAKE_CURRENT_SOURCE_DIR}/render_pass.h
    ${CMAKE_CURRENT_SOURCE_DIR}/render_state.h
    ${CMAKE_CURRENT_SOURCE_DIR}/render_target_view.h
    ${CMAKE_CURRENT_SOURCE_DIR}/render_window.h
    ${CMAKE_CURRENT_SOURCE_DIR}/renderable.h
    ${CMAKE_CURRENT_SOURCE_DIR}/renderable_2d.h
    ${CMAKE_CURRENT_SOURCE_DIR}/renderable_3d.h
    ${CMAKE_CURRENT_SOURCE_DIR}/serial_pass.h
    ${CMAKE_CURRENT_SOURCE_DIR}/shader_resource_view.h
    ${CMAKE_CURRENT_SOURCE_DIR}/static_mesh_renderable_2d.h
    ${CMAKE_CURRENT_SOURCE_DIR}/static_mesh_renderable_3d.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/static_mesh_renderable_3d.h
    ${CMAKE_CURRENT_SOURCE_DIR}/unordered_access_view.h
    ${CMAKE_CURRENT_SOURCE_DIR}/utility.h
)

add_subdirectory(hal)
add_subdirectory(renderer)
add_subdirectory(vk1)
add_subdirectory(rg)
gaia_register_source_group("graphics" FILES ${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt)
