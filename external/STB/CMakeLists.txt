find_path(STB_INCLUDE_DIR
    NAMES stb_image.h
    PATHS
        /usr/include
        /usr/local/include
        /opt/homebrew/include
        /usr/include/stb
        /usr/local/include/stb
    PATH_SUFFIXES
        stb
)

if(NOT STB_INCLUDE_DIR)
    include(FetchContent)
    FetchContent_Declare(
        stb
        GIT_REPOSITORY https://github.com/nothings/stb.git
        GIT_TAG        master
        GIT_SHALLOW    TRUE
    )
    FetchContent_MakeAvailable(stb)

    add_library(stb INTERFACE)
    target_include_directories(stb INTERFACE ${stb_SOURCE_DIR})
endif()
