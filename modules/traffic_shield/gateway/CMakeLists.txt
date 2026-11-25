add_library(gateway SHARED gateway.cpp)

target_link_libraries(gateway PRIVATE
    url_analyzer
    cache
    decision
)

set_target_properties(gateway PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
