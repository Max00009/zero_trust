add_library(cache SHARED cache.cpp)

set_target_properties(cache PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(cache PUBLIC ${CMAKE_DL_LIBS})