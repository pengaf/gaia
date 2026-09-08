set(source_files
    ${CMAKE_CURRENT_SOURCE_DIR}/render_graph_builder.cpp
)

set(header_files
    ${CMAKE_CURRENT_SOURCE_DIR}/common.h
    ${CMAKE_CURRENT_SOURCE_DIR}/render_graph.h
    ${CMAKE_CURRENT_SOURCE_DIR}/render_graph_builder.h
    ${CMAKE_CURRENT_SOURCE_DIR}/render_pass.h
    ${CMAKE_CURRENT_SOURCE_DIR}/render_resource.h
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

gaia_register_source_group("graphics/rg/Source Files" FILES ${source_files})
gaia_register_source_group("graphics/rg/Header Files" FILES ${header_files})
gaia_register_source_group("graphics/rg/Interface Files" FILES ${interface_files})
gaia_register_source_group("graphics/rg" FILES ${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt)
