target_sources(gaia
    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS ${GAIA_SOURCE_DIR}
        FILES
            axis_aligned_box.h
            matrix.h
            oriented_box.h
            utility.h
            vector.h
)

target_sources(gaia PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt)

gaia_register_source_group("math" FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/axis_aligned_box.h
    ${CMAKE_CURRENT_SOURCE_DIR}/matrix.h
    ${CMAKE_CURRENT_SOURCE_DIR}/oriented_box.h
    ${CMAKE_CURRENT_SOURCE_DIR}/utility.h
    ${CMAKE_CURRENT_SOURCE_DIR}/vector.h
)
gaia_register_source_group("math" FILES ${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt)
