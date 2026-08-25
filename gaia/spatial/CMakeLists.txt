target_sources(gaia
    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS ${GAIA_SOURCE_DIR}
        FILES
            octree.h
            quadtree.h
            utility.h
)

target_sources(gaia PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt)

gaia_register_source_group("spatial" FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/octree.h
    ${CMAKE_CURRENT_SOURCE_DIR}/quadtree.h
    ${CMAKE_CURRENT_SOURCE_DIR}/utility.h
)
gaia_register_source_group("spatial" FILES ${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt)
