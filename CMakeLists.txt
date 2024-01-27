cmake_minimum_required(VERSION 3.27)
project(dino_engine)

set(CMAKE_CXX_STANDARD 17)

include(FetchContent)
set(FETCHCONTENT_QUIET FALSE)

FetchContent_Declare(
        hermes
        GIT_REPOSITORY "https://github.com/facebook/hermes.git"
        GIT_TAG "static_h"
        GIT_PROGRESS TRUE
        CMAKE_ARGS -DHERMES_BUILD_APPLE_FRAMEWORK=OFF
)

FetchContent_MakeAvailable(hermes)

FetchContent_Declare(
        SFML
        GIT_REPOSITORY "https://github.com/SFML/SFML.git"
        GIT_TAG "2.6.x"
        GIT_PROGRESS TRUE
)

FetchContent_MakeAvailable(SFML)

set(SHERMES ${hermes_BINARY_DIR}/tools/shermes CACHE FILEPATH "shermes tool path")
set(FFIGEN ${hermes_SOURCE_DIR}/tools/ffigen/ffigen.py CACHE FILEPATH "ffigen tool path")

# Add Hermes include directories
include_directories("${hermes_SOURCE_DIR}/API")
include_directories("${hermes_SOURCE_DIR}/API/jsi")
include_directories("${hermes_SOURCE_DIR}/public")

# Add Hermes library directories
link_directories("${hermes_BINARY_DIR}/API/hermes")
link_directories("${hermes_BINARY_DIR}/jsi")
link_libraries(jsi)

link_libraries( sfml-audio
                sfml-graphics
                sfml-system
                sfml-window)

add_subdirectory(src)
