cmake_minimum_required(VERSION 3.23)

project(gaia
    VERSION 0.1.0
    DESCRIPTION "Gaia C++ rendering and scene library"
    LANGUAGES CXX
)

option(GAIA_BUILD_SHARED "Build gaia as a shared library" ${BUILD_SHARED_LIBS})
option(GAIA_BUILD_VULKAN "Build the Vulkan backend when available" ON)
set(GAIA_PAF_PATH "D:/pengaf/idlcpp/paf/" CACHE PATH "Path to the PAF include/runtime root")

if(EMSCRIPTEN)
    set(GAIA_BUILD_VULKAN OFF CACHE BOOL "Build the Vulkan backend when available" FORCE)
endif()

add_subdirectory(gaia)
