set(HERMES_BUILD_APPLE_FRAMEWORK OFF)

add_subdirectory(hermes)

add_library(vendor INTERFACE)
target_link_libraries(vendor INTERFACE hermesvm jsi)