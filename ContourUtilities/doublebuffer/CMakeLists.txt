add_executable(doublebuffer main.cpp DoubleBuffer.cpp)
target_link_libraries(doublebuffer ${GDAL_LIBRARY} ${GEOS_LIBRARY})
