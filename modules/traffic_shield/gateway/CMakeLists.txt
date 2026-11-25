add_library(gateway SHARED gateway.cpp)

target_link_libraries(gateway PRIVATE
    url_analyzer
    cache
    decision
)

set_target_properties(gateway PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

# Custom Commands to copy tld_list in the same directory as libgateway.so cause url_parser runs as libgateway.so and it looks for tld_list.txt in the same direcory as the .so file
