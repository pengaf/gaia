target_sources(gaia
    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS ${GAIA_SOURCE_DIR}
        FILES
            compute_element.h
            hal_fwd.h
            mesh_shader_element.h
            ray_tracing_element.h
            render_element.h
            render_element_common.h
            utility.h
            vertex_shader_element.h
)

target_sources(gaia PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt)

gaia_register_source_group("graphics\\hal" FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/compute_element.h
    ${CMAKE_CURRENT_SOURCE_DIR}/hal_fwd.h
    ${CMAKE_CURRENT_SOURCE_DIR}/mesh_shader_element.h
    ${CMAKE_CURRENT_SOURCE_DIR}/ray_tracing_element.h
    ${CMAKE_CURRENT_SOURCE_DIR}/render_element.h
    ${CMAKE_CURRENT_SOURCE_DIR}/render_element_common.h
    ${CMAKE_CURRENT_SOURCE_DIR}/utility.h
    ${CMAKE_CURRENT_SOURCE_DIR}/vertex_shader_element.h
)
gaia_register_source_group("graphics\\hal" FILES ${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt)
