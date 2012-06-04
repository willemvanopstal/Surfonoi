add_executable(gridfilter main.cpp Grid.cpp Point.cpp)
target_link_libraries(gridfilter ${PROJ4_LIBRARY})

install (TARGETS gridfilter DESTINATION bin)