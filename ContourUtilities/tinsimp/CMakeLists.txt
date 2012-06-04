add_executable(tinsimp main.cpp "${CMAKE_SOURCE_DIR}/CgalProcessor/CgalProcessor.cpp")
target_link_libraries(tinsimp ${GDAL_LIBRARY} ${GEOS_LIBRARY} ${CGAL_CGAL_LIBRARY} ${CGAL_CORE_LIBRARY} ${CGAL_3RD_PARTY_LIBRARIES})
