set(source_files)
set(header_files
    ${CMAKE_CURRENT_SOURCE_DIR}/octree.h
    ${CMAKE_CURRENT_SOURCE_DIR}/quadtree.h
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

gaia_register_source_group("spatial/Source Files" FILES ${source_files})
gaia_register_source_group("spatial/Header Files" FILES ${header_files})
gaia_register_source_group("spatial/Interface Files" FILES ${interface_files})
gaia_register_source_group("spatial" FILES ${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt)
