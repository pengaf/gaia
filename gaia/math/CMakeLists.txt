set(source_files)
set(header_files
    ${CMAKE_CURRENT_SOURCE_DIR}/axis_aligned_box.h
    ${CMAKE_CURRENT_SOURCE_DIR}/matrix.h
    ${CMAKE_CURRENT_SOURCE_DIR}/oriented_box.h
    ${CMAKE_CURRENT_SOURCE_DIR}/utility.h
    ${CMAKE_CURRENT_SOURCE_DIR}/vector.h
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

gaia_register_source_group("math/Source Files" FILES ${source_files})
gaia_register_source_group("math/Header Files" FILES ${header_files})
gaia_register_source_group("math/Interface Files" FILES ${interface_files})
gaia_register_source_group("math" FILES ${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt)
