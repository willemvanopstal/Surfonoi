add_executable(gridfilter2raster main.cpp Raster.cpp)
target_link_libraries(gridfilter2raster ${PROJ4_LIBRARY} ${GDAL_LIBRARY})
