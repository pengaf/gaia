target_sources(gaia
    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS ${GAIA_SOURCE_DIR}
        FILES
            forward_renderer.h
            utility.h
)

target_sources(gaia PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt)

gaia_register_source_group("graphics\\renderer" FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/forward_renderer.h
    ${CMAKE_CURRENT_SOURCE_DIR}/utility.h
)
gaia_register_source_group("graphics\\renderer" FILES ${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt)
