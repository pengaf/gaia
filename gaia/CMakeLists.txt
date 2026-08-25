set(GAIA_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

set(gaia_library_type STATIC)
if(GAIA_BUILD_SHARED)
    set(gaia_library_type SHARED)
endif()

add_library(gaia ${gaia_library_type})
add_library(gaia::gaia ALIAS gaia)

target_sources(gaia
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt
        unit_mesh_source.cpp
    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS ${GAIA_SOURCE_DIR}
        FILES
            bit_mask_enum.h
            file_mesh_source.h
            mesh_source.h
            ref_ptr.h
            scene.h
            scene_component.h
            scene_node.h
            static_mesh_component.h
            unit_mesh_source.h
            utility.h
)

gaia_register_source_group("" FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/bit_mask_enum.h
    ${CMAKE_CURRENT_SOURCE_DIR}/file_mesh_source.h
    ${CMAKE_CURRENT_SOURCE_DIR}/mesh_source.h
    ${CMAKE_CURRENT_SOURCE_DIR}/ref_ptr.h
    ${CMAKE_CURRENT_SOURCE_DIR}/scene.h
    ${CMAKE_CURRENT_SOURCE_DIR}/scene_component.h
    ${CMAKE_CURRENT_SOURCE_DIR}/scene_node.h
    ${CMAKE_CURRENT_SOURCE_DIR}/static_mesh_component.h
    ${CMAKE_CURRENT_SOURCE_DIR}/unit_mesh_source.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/unit_mesh_source.h
    ${CMAKE_CURRENT_SOURCE_DIR}/utility.h
)

target_compile_features(gaia PUBLIC cxx_std_20)

target_include_directories(gaia
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        ${GAIA_PAF_PATH}
        $<INSTALL_INTERFACE:include>
)

set(gaia_platform_definition GAIA_PLATFORM=GAIA_PLATFORM_WIN32)
if(EMSCRIPTEN)
    set(gaia_platform_definition GAIA_PLATFORM=GAIA_PLATFORM_WEB)
elseif(ANDROID)
    set(gaia_platform_definition GAIA_PLATFORM=GAIA_PLATFORM_ANDROID)
elseif(UNIX AND NOT APPLE)
    set(gaia_platform_definition GAIA_PLATFORM=GAIA_PLATFORM_XCB)
endif()

target_compile_definitions(gaia
    PUBLIC
        ${gaia_platform_definition}
        GAIA_PAF_PATH="$<SHELL_PATH:${GAIA_PAF_PATH}>"
)

if(MSVC)
    target_compile_options(gaia PRIVATE /utf-8)
endif()

add_subdirectory(graphics)
add_subdirectory(math)
add_subdirectory(scene)
add_subdirectory(spatial)

get_property(gaia_idl_generated_files GLOBAL PROPERTY GAIA_IDL_ALL_GENERATED_FILES)
if(gaia_idl_generated_files)
    target_sources(gaia PRIVATE ${gaia_idl_generated_files})
    set_source_files_properties(${gaia_idl_generated_files} PROPERTIES HEADER_FILE_ONLY TRUE)
endif()
gaia_finalize_idlcpp_sources(gaia)

if(GAIA_BUILD_VULKAN)
    if(NOT EXISTS "${VULKAN_SDK}/Include")
        message(FATAL_ERROR "Vulkan SDK include directory was not found: ${VULKAN_SDK}/Include")
    endif()
    if(NOT EXISTS "${VULKAN_SDK}/Lib")
        message(FATAL_ERROR "Vulkan SDK library directory was not found: ${VULKAN_SDK}/Lib")
    endif()

    list(APPEND CMAKE_PREFIX_PATH "${VULKAN_SDK}")
    find_package(Vulkan REQUIRED)
    add_subdirectory(graphics/vk1)
    target_include_directories(gaia PUBLIC "${VULKAN_SDK}/Include")
    target_link_directories(gaia PUBLIC "${VULKAN_SDK}/Lib")
    target_link_libraries(gaia PUBLIC Vulkan::Vulkan)
    target_compile_definitions(gaia PUBLIC GAIA_HAS_VULKAN=1)
else()
    target_compile_definitions(gaia PUBLIC GAIA_HAS_VULKAN=0)
endif()

get_property(gaia_source_group_ids GLOBAL PROPERTY GAIA_SOURCE_GROUP_IDS)
foreach(source_group_id IN LISTS gaia_source_group_ids)
    get_property(source_group_name GLOBAL PROPERTY "GAIA_SOURCE_GROUP_NAME_${source_group_id}")
    get_property(source_group_files GLOBAL PROPERTY "GAIA_SOURCE_GROUP_FILES_${source_group_id}")
    source_group("${source_group_name}" FILES ${source_group_files})
endforeach()

get_property(gaia_idl_all_generated_files GLOBAL PROPERTY GAIA_IDL_ALL_GENERATED_FILES)
get_property(gaia_idl_group_ids GLOBAL PROPERTY GAIA_IDL_GROUP_IDS)
foreach(idl_group_id IN LISTS gaia_idl_group_ids)
    get_property(idl_generated_files GLOBAL PROPERTY "GAIA_IDL_FILES_${idl_group_id}")
    if(idl_generated_files)
        list(GET idl_generated_files 0 idl_generated_file)
        get_filename_component(idl_generated_directory "${idl_generated_file}" DIRECTORY)
        file(RELATIVE_PATH idl_generated_prefix
            "${GAIA_SOURCE_DIR}"
            "${idl_generated_directory}"
        )
        if(idl_generated_prefix STREQUAL ".")
            set(idl_generated_group "IDL Generated")
        else()
            set(idl_generated_group "${idl_generated_prefix}\\IDL Generated")
        endif()
        source_group("${idl_generated_group}" FILES ${idl_generated_files})
    endif()
endforeach()

foreach(idl_generated_file IN LISTS gaia_idl_all_generated_files)
    get_filename_component(idl_generated_directory "${idl_generated_file}" DIRECTORY)
    file(RELATIVE_PATH idl_generated_prefix
        "${GAIA_SOURCE_DIR}"
        "${idl_generated_directory}"
    )
    if(idl_generated_prefix STREQUAL ".")
        set(idl_generated_group "IDL Generated")
    else()
        set(idl_generated_group "${idl_generated_prefix}\\IDL Generated")
    endif()
    source_group("${idl_generated_group}" FILES "${idl_generated_file}")
endforeach()

include(GNUInstallDirs)

install(TARGETS gaia
    EXPORT gaiaTargets
    FILE_SET HEADERS
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/gaia
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

install(EXPORT gaiaTargets
    NAMESPACE gaia::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/gaia
)
