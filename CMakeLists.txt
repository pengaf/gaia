cmake_minimum_required(VERSION 3.23)

project(gaia
    VERSION 0.1.0
    DESCRIPTION "Gaia C++ rendering and scene library"
    LANGUAGES CXX
)

option(GAIA_BUILD_SHARED "Build gaia as a shared library" ${BUILD_SHARED_LIBS})
option(GAIA_BUILD_VULKAN "Build the Vulkan backend when available" ON)
set(VULKAN_SDK "C:/VulkanSDK/1.4.341.1" CACHE PATH "Path to the Vulkan SDK root")
set(IDLCPP_EXECUTABLE "D:/pengaf/idlcpp2/build-vs2026/bin/Debug/idlcpp.exe" CACHE FILEPATH "Path to the idlcpp executable")
set(GAIA_PAF_PATH "D:/pengaf/idlcpp2/paf/" CACHE PATH "Path to the PAF include/runtime root")

function(gaia_register_source_group group)
    set(group_files ${ARGN})
    if(group_files AND group_files[0] STREQUAL "FILES")
        list(REMOVE_AT group_files 0)
    endif()
    if(group STREQUAL "")
        set(group_id ROOT)
    else()
        string(MAKE_C_IDENTIFIER "${group}" group_id)
    endif()
    set_property(GLOBAL APPEND PROPERTY GAIA_SOURCE_GROUP_IDS "${group_id}")
    set_property(GLOBAL PROPERTY "GAIA_SOURCE_GROUP_NAME_${group_id}" "${group}")
    set_property(GLOBAL APPEND PROPERTY "GAIA_SOURCE_GROUP_FILES_${group_id}" ${group_files})
endfunction()

function(gaia_add_idlcpp_sources target)
    cmake_parse_arguments(
        GAIA_IDLCPP
        ""
        "EXPORT_MACRO;PAF_PATH"
        "INTERFACE_FILES;IMPORT_DIRECTORIES"
        ${ARGN}
    )

    if(NOT TARGET "${target}")
        message(FATAL_ERROR "gaia_add_idlcpp_sources: target '${target}' does not exist")
    endif()
    if(NOT GAIA_IDLCPP_INTERFACE_FILES)
        message(FATAL_ERROR "gaia_add_idlcpp_sources: INTERFACE_FILES is required")
    endif()
    if(NOT IDLCPP_EXECUTABLE)
        message(FATAL_ERROR "IDLCPP_EXECUTABLE is not set")
    endif()
    if(NOT EXISTS "${IDLCPP_EXECUTABLE}")
        message(FATAL_ERROR "idlcpp executable was not found: ${IDLCPP_EXECUTABLE}")
    endif()

    set(paf_path "${GAIA_IDLCPP_PAF_PATH}")
    if(NOT paf_path)
        set(paf_path "${GAIA_PAF_PATH}")
    endif()

    set(import_options)
    if(paf_path)
        list(APPEND import_options "-pc${paf_path}/pafcore")
    endif()
    foreach(import_directory IN LISTS GAIA_IDLCPP_IMPORT_DIRECTORIES)
        list(APPEND import_options "-I${import_directory}")
    endforeach()

    set(interface_paths)
    foreach(interface_file IN LISTS GAIA_IDLCPP_INTERFACE_FILES)
        if(IS_ABSOLUTE "${interface_file}")
            list(APPEND interface_paths "${interface_file}")
        else()
            list(APPEND interface_paths "${CMAKE_CURRENT_SOURCE_DIR}/${interface_file}")
        endif()
    endforeach()

    set(generated_files)
    foreach(interface_file IN LISTS GAIA_IDLCPP_INTERFACE_FILES)
        if(IS_ABSOLUTE "${interface_file}")
            set(interface_path "${interface_file}")
        else()
            set(interface_path "${CMAKE_CURRENT_SOURCE_DIR}/${interface_file}")
        endif()
        if(NOT EXISTS "${interface_path}")
            message(FATAL_ERROR "idlcpp interface file was not found: ${interface_path}")
        endif()

        get_filename_component(interface_name "${interface_path}" NAME_WE)
        get_filename_component(interface_directory "${interface_path}" DIRECTORY)
        set(generated_header "${interface_directory}/${interface_name}.h")
        set(generated_internal "${interface_directory}/${interface_name}.ic")
        set(generated_meta_header "${interface_directory}/${interface_name}.mh")
        set(generated_meta_source "${interface_directory}/${interface_name}.mc")
        set(generator_options ${import_options})
        if(GAIA_IDLCPP_EXPORT_MACRO)
            list(APPEND generator_options "-em${GAIA_IDLCPP_EXPORT_MACRO}")
        endif()

        list(APPEND generated_files
            "${generated_header}" "${generated_internal}"
            "${generated_meta_header}" "${generated_meta_source}"
        )

        set(generated_group_root "${CMAKE_CURRENT_SOURCE_DIR}")
        if(DEFINED GAIA_SOURCE_DIR)
            set(generated_group_root "${GAIA_SOURCE_DIR}")
        endif()
        file(RELATIVE_PATH generated_group_prefix
            "${generated_group_root}" "${interface_directory}")
        if(generated_group_prefix STREQUAL ".")
            set(generated_group "IDL Generated")
        else()
            set(generated_group "${generated_group_prefix}\\IDL Generated")
        endif()
        source_group("${generated_group}" FILES
            "${generated_header}" "${generated_internal}"
            "${generated_meta_header}" "${generated_meta_source}"
        )

        string(MAKE_C_IDENTIFIER "${CMAKE_CURRENT_SOURCE_DIR}" idlcpp_directory_id)
        set_property(GLOBAL APPEND PROPERTY GAIA_IDL_GROUP_IDS "${idlcpp_directory_id}")
        set_property(GLOBAL APPEND PROPERTY "GAIA_IDL_FILES_${idlcpp_directory_id}"
            "${generated_header}" "${generated_internal}"
            "${generated_meta_header}" "${generated_meta_source}")

        string(MAKE_C_IDENTIFIER "${interface_path}" idlcpp_request_id)
        set_property(GLOBAL APPEND PROPERTY GAIA_IDL_REQUEST_IDS "${idlcpp_request_id}")
        set_property(GLOBAL PROPERTY "GAIA_IDL_INTERFACE_${idlcpp_request_id}" "${interface_path}")
        set_property(GLOBAL PROPERTY "GAIA_IDL_OUTPUTS_${idlcpp_request_id}"
            "${generated_header};${generated_internal};${generated_meta_header};${generated_meta_source}")
        set_property(GLOBAL PROPERTY "GAIA_IDL_OPTIONS_${idlcpp_request_id}" "${generator_options}")
        set_property(GLOBAL PROPERTY "GAIA_IDL_DEPENDS_${idlcpp_request_id}" "${interface_paths}")
        set_property(GLOBAL PROPERTY "GAIA_IDL_WORKING_DIRECTORY_${idlcpp_request_id}" "${interface_directory}")
    endforeach()

    set_property(GLOBAL APPEND PROPERTY GAIA_IDL_ALL_GENERATED_FILES ${generated_files})
endfunction()

function(gaia_finalize_idlcpp_sources target)
    get_property(idl_request_ids GLOBAL PROPERTY GAIA_IDL_REQUEST_IDS)
    list(REMOVE_DUPLICATES idl_request_ids)
    foreach(idl_request_id IN LISTS idl_request_ids)
        get_property(interface_path GLOBAL PROPERTY "GAIA_IDL_INTERFACE_${idl_request_id}")
        get_property(generated_outputs GLOBAL PROPERTY "GAIA_IDL_OUTPUTS_${idl_request_id}")
        get_property(generator_options GLOBAL PROPERTY "GAIA_IDL_OPTIONS_${idl_request_id}")
        get_property(interface_dependencies GLOBAL PROPERTY "GAIA_IDL_DEPENDS_${idl_request_id}")
        get_property(interface_directory GLOBAL PROPERTY "GAIA_IDL_WORKING_DIRECTORY_${idl_request_id}")

        add_custom_command(
            OUTPUT ${generated_outputs}
            MAIN_DEPENDENCY "${interface_path}"
            COMMAND "${IDLCPP_EXECUTABLE}" ${generator_options} "${interface_path}"
            DEPENDS "${IDLCPP_EXECUTABLE}" ${interface_dependencies}
            WORKING_DIRECTORY "${interface_directory}"
            VERBATIM
        )
    endforeach()
endfunction()

if(EMSCRIPTEN)
    set(GAIA_BUILD_VULKAN OFF CACHE BOOL "Build the Vulkan backend when available" FORCE)
endif()

add_subdirectory(gaia)
