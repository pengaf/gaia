set(source_files
    ${CMAKE_CURRENT_SOURCE_DIR}/file_system.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/native_file_system.cpp
)

set(header_files)

set(interface_files
    ${CMAKE_CURRENT_SOURCE_DIR}/file_system.i
    ${CMAKE_CURRENT_SOURCE_DIR}/native_file_system.i
)

target_sources(gaia
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt
        ${source_files}
        ${interface_files}
)

gaia_add_idlcpp_sources(gaia
    INTERFACE_FILES ${interface_files}
    IMPORT_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}
)

gaia_register_source_group("storage/$Source Files" FILES ${source_files})
gaia_register_source_group("storage/$Header Files" FILES ${header_files})
gaia_register_source_group("storage/$Interface Files" FILES ${interface_files})
gaia_register_source_group("storage" FILES ${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt)
