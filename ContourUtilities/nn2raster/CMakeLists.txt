add_executable(nn2raster main.cpp "${CMAKE_SOURCE_DIR}/CgalProcessor/CgalProcessor.cpp")
target_link_libraries(nn2raster ${GDAL_LIBRARY} ${GEOS_LIBRARY} ${CGAL_LIBRARIES} ${CGAL_3RD_PARTY_LIBRARIES})

install (TARGETS nn2raster DESTINATION bin)