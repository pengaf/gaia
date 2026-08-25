set(gaia_scene_interface_files
    ${CMAKE_CURRENT_SOURCE_DIR}/component.i
    ${CMAKE_CURRENT_SOURCE_DIR}/node.i
    ${CMAKE_CURRENT_SOURCE_DIR}/scene.i
)

set(gaia_scene_source_files
    ${CMAKE_CURRENT_SOURCE_DIR}/component.cpp
)

target_sources(gaia
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt
        ${gaia_scene_source_files}
        ${gaia_scene_interface_files}
)

gaia_add_idlcpp_sources(gaia
    INTERFACE_FILES ${gaia_scene_interface_files}
    IMPORT_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}
)

gaia_register_source_group("scene\\Interface Files" FILES ${gaia_scene_interface_files})
gaia_register_source_group("scene" FILES ${gaia_scene_source_files})
gaia_register_source_group("scene" FILES ${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt)
