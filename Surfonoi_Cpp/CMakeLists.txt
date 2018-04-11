#Copyright (c) 2012, 2013
#Ravi Peters -- r.y.peters@tudelft.nl
#All rights reserved
#
#This file is part of Surfonoi.
#
#Surfonoi is free software: you can redistribute it and/or modify
#it under the terms of the GNU General Public License as published by
#the Free Software Foundation, either version 3 of the License, or
#(at your option) any later version.
#
#Surfonoi is distributed in the hope that it will be useful,
#but WITHOUT ANY WARRANTY; without even the implied warranty of
#MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#GNU General Public License for more details.
#
#You should have received a copy of the GNU General Public License
#along with Surfonoi.  If not, see <http://www.gnu.org/licenses/>.

cmake_minimum_required(VERSION 2.6)
project(ContourUtilities)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")
find_package(GDAL)
find_package(GEOS)
find_package(PROJ4)
find_package(CGAL)

include(${CGAL_USE_FILE})
include_directories("${CMAKE_SOURCE_DIR}/include" "${CMAKE_SOURCE_DIR}/CgalProcessor")
include_directories(${GDAL_INCLUDE_DIR})

set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_FLAGS "-g -Wall")

add_subdirectory(prefilter)
add_subdirectory(gridfilter)
add_subdirectory(gridfilter2raster)
add_subdirectory(checkraster)
add_subdirectory(doublebuffer)
add_subdirectory(tinsimp)
add_subdirectory(smoothdens)
add_subdirectory(nn2raster)
add_subdirectory(nn2xyz)
