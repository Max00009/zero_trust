add_library(url_analyzer STATIC
    url_analyzer.cpp
    url_parser.cpp
)

set_target_properties(url_analyzer PROPERTIES 
    POSITION_INDEPENDENT_CODE ON
)

target_include_directories(url_analyzer PUBLIC ${PROJECT_SOURCE_DIR})
target_link_libraries(url_analyzer PUBLIC ${CMAKE_DL_LIBS}) #for dladdr