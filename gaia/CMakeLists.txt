set(gaia_public_headers
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
    graphics/common.h
    graphics/compute_element.h
    graphics/depth_stencil_view.h
    graphics/forward_renderer.h
    graphics/frame_graph.h
    graphics/material.h
    graphics/mesh.h
    graphics/mesh_shader_element.h
    graphics/parallel_pass.h
    graphics/pass.h
    graphics/ray_tracing_element.h
    graphics/render_element.h
    graphics/render_output.h
    graphics/render_pass.h
    graphics/render_state.h
    graphics/render_target_view.h
    graphics/render_window.h
    graphics/renderable.h
    graphics/renderable_2d.h
    graphics/renderable_3d.h
    graphics/serial_pass.h
    graphics/shader_resource_view.h
    graphics/static_mesh_renderable_2d.h
    graphics/static_mesh_renderable_3d.h
    graphics/unordered_access_view.h
    graphics/utility.h
    graphics/vertex_shader_element.h
    graphics/renderer/forward_renderer.h
    graphics/renderer/utility.h
    math/axis_aligned_box.h
    math/matrix.h
    math/oriented_box.h
    math/utility.h
    math/vector.h
    spatial/octree.h
    spatial/quadtree.h
    spatial/utility.h
)

set(gaia_sources
    graphics/pass.cpp
    graphics/static_mesh_renderable_3d.cpp
    unit_mesh_source.cpp
)

set(gaia_vulkan_headers
    graphics/vk1/buffer.h
    graphics/vk1/compute_element.h
    graphics/vk1/compute_pass.h
    graphics/vk1/graphics_pipeline.h
    graphics/vk1/mesh_shader_element.h
    graphics/vk1/ray_tracing_element.h
    graphics/vk1/render_element.h
    graphics/vk1/render_pass.h
    graphics/vk1/render_system.h
    graphics/vk1/render_window.h
    graphics/vk1/shader.h
    graphics/vk1/texture.h
    graphics/vk1/utility.h
    graphics/vk1/vertex_shader_element.h
    graphics/vk1/view.h
    graphics/vk1/detail/mapping.h
    graphics/vk1/detail/single_thread_render_scheduler.h
    graphics/vk1/detail/utility.h
    graphics/vk1/detail/vk_mem_alloc.h
    graphics/vk1/detail/vulkan.h
)

set(gaia_vulkan_sources
    graphics/vk1/buffer.cpp
    graphics/vk1/render_system.cpp
    graphics/vk1/render_window.cpp
    graphics/vk1/texture.cpp
    graphics/vk1/detail/single_thread_render_scheduler.cpp
    graphics/vk1/detail/vk_mem_alloc.cpp
)

set(gaia_library_type STATIC)
if(GAIA_BUILD_SHARED)
    set(gaia_library_type SHARED)
endif()

add_library(gaia ${gaia_library_type})
add_library(gaia::gaia ALIAS gaia)

target_sources(gaia
    PRIVATE
        ${gaia_sources}
    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS
            ${CMAKE_CURRENT_SOURCE_DIR}
        FILES
            ${gaia_public_headers}
)

target_compile_features(gaia PUBLIC cxx_std_20)

target_include_directories(gaia
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
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

if(GAIA_BUILD_VULKAN)
    find_package(Vulkan REQUIRED)

    target_sources(gaia
        PRIVATE
            ${gaia_vulkan_sources}
        PUBLIC
            FILE_SET HEADERS
            BASE_DIRS
                ${CMAKE_CURRENT_SOURCE_DIR}
            FILES
                ${gaia_vulkan_headers}
    )

    target_link_libraries(gaia PUBLIC Vulkan::Vulkan)
    target_compile_definitions(gaia PUBLIC GAIA_HAS_VULKAN=1)
else()
    target_compile_definitions(gaia PUBLIC GAIA_HAS_VULKAN=0)
endif()

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES
    ${gaia_public_headers}
    ${gaia_sources}
    ${gaia_vulkan_headers}
    ${gaia_vulkan_sources}
)

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
