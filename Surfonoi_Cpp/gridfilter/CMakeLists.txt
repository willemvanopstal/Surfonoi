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

add_executable(gridfilter main.cpp Grid.cpp Point.cpp)
target_link_libraries(gridfilter ${PROJ4_LIBRARY})

install (TARGETS gridfilter DESTINATION bin)