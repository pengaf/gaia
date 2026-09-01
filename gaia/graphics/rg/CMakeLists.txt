target_sources(gaia
    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS ${GAIA_SOURCE_DIR}
        FILES
            common.h
            render_graph.h
            render_pass.h
            utility.h
)

target_sources(gaia PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt)

gaia_register_source_group("graphics\\rg" FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/common.h
    ${CMAKE_CURRENT_SOURCE_DIR}/render_graph.h
    ${CMAKE_CURRENT_SOURCE_DIR}/render_pass.h
    ${CMAKE_CURRENT_SOURCE_DIR}/utility.h
)
gaia_register_source_group("graphics\\rg" FILES ${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt)
