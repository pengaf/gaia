add_executable(test_slang
    main.cpp
)

target_compile_features(test_slang PRIVATE cxx_std_20)
##target_include_directories(test_slang PRIVATE "${VULKAN_SDK}/Include")

##target_link_directories(test_slang PRIVATE "${VULKAN_SDK}/Lib")
target_link_libraries(test_slang PRIVATE slang gaia)

if(MSVC)
    target_compile_options(test_slang PRIVATE /utf-8)
endif()

add_custom_command(TARGET test_slang POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${VULKAN_SDK}/Bin/slang.dll"
        "$<TARGET_FILE_DIR:test_slang>"
    VERBATIM
)

set_target_properties(test_slang PROPERTIES
    FOLDER tests
)
