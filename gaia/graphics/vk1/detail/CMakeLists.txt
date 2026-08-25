target_sources(gaia
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt
        mapping.cpp
        single_thread_render_scheduler.cpp
        vk_mem_alloc.cpp
    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS ${GAIA_SOURCE_DIR}
        FILES
            mapping.h
            single_thread_render_scheduler.h
            utility.h
            vk_mem_alloc.h
            vulkan.h
)

gaia_register_source_group("graphics\\vk1\\detail" FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/mapping.h
    ${CMAKE_CURRENT_SOURCE_DIR}/mapping.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/single_thread_render_scheduler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/single_thread_render_scheduler.h
    ${CMAKE_CURRENT_SOURCE_DIR}/utility.h
    ${CMAKE_CURRENT_SOURCE_DIR}/vk_mem_alloc.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/vk_mem_alloc.h
    ${CMAKE_CURRENT_SOURCE_DIR}/vulkan.h
)
gaia_register_source_group("graphics\\vk1\\detail" FILES ${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt)
