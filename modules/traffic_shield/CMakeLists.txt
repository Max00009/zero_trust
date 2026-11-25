project(traffic_shield CXX)

# Include root: every target below will start the include path from here(traffic_shield/)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(analyzer/url_analyzer)
add_subdirectory(cache)
add_subdirectory(decision_engine)
add_subdirectory(gateway)