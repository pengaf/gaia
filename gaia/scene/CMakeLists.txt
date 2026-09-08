set(source_files
    ${CMAKE_CURRENT_SOURCE_DIR}/component.cpp
)

set(header_files)

set(interface_files
    ${CMAKE_CURRENT_SOURCE_DIR}/component.i
    ${CMAKE_CURRENT_SOURCE_DIR}/node.i
    ${CMAKE_CURRENT_SOURCE_DIR}/scene.i
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

gaia_register_source_group("scene/Source Files" FILES ${source_files})
gaia_register_source_group("scene/Header Files" FILES ${header_files})
gaia_register_source_group("scene/Interface Files" FILES ${interface_files})
gaia_register_source_group("scene" FILES ${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt)
