cmake_minimum_required(VERSION 2.6)
project(ContourUtilities)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")
find_package(GDAL)
find_package(GEOS)
find_package(PROJ4)
find_package(CGAL)

include_directories("${CMAKE_SOURCE_DIR}/include" "${CMAKE_SOURCE_DIR}/CgalProcessor")

set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_FLAGS "-g -Wall -frounding-math")

add_subdirectory(prefilter)
add_subdirectory(gridfilter)
add_subdirectory(gridfilter2raster)
add_subdirectory(checkraster)
add_subdirectory(doublebuffer)
add_subdirectory(tinsimp)
add_subdirectory(smoothdens)
add_subdirectory(nn2raster)
