set(source_files)
set(header_files
    ${CMAKE_CURRENT_SOURCE_DIR}/forward_renderer.h
    ${CMAKE_CURRENT_SOURCE_DIR}/utility.h
)
set(interface_files)

target_sources(gaia
    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS ${GAIA_SOURCE_DIR}
        FILES
            ${header_files}
)

target_sources(gaia PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt)

gaia_register_source_group("graphics/renderer/Source Files" FILES ${source_files})
gaia_register_source_group("graphics/renderer/Header Files" FILES ${header_files})
gaia_register_source_group("graphics/renderer/Interface Files" FILES ${interface_files})
gaia_register_source_group("graphics/renderer" FILES ${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt)
