set(source_files
    ${CMAKE_CURRENT_SOURCE_DIR}/mapping.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/single_thread_render_scheduler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/vk_mem_alloc.cpp
)
set(header_files
    ${CMAKE_CURRENT_SOURCE_DIR}/mapping.h
    ${CMAKE_CURRENT_SOURCE_DIR}/single_thread_render_scheduler.h
    ${CMAKE_CURRENT_SOURCE_DIR}/utility.h
    ${CMAKE_CURRENT_SOURCE_DIR}/vk_mem_alloc.h
    ${CMAKE_CURRENT_SOURCE_DIR}/vulkan.h
)
set(interface_files)

target_sources(gaia
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt
        ${source_files}
    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS ${GAIA_SOURCE_DIR}
        FILES
            ${header_files}
)

gaia_register_source_group("graphics/vk1/detail/Source Files" FILES ${source_files})
gaia_register_source_group("graphics/vk1/detail/Header Files" FILES ${header_files})
gaia_register_source_group("graphics/vk1/detail/Interface Files" FILES ${interface_files})
gaia_register_source_group("graphics/vk1/detail" FILES ${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt)
