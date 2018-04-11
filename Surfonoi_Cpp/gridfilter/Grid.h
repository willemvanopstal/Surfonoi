// Copyright (c) 2012, 2013
// Ravi Peters -- r.y.peters@tudelft.nl
// All rights reserved
// 
// This file is part of Surfonoi.
// 
// Surfonoi is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// Surfonoi is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with Surfonoi.  If not, see <http://www.gnu.org/licenses/>.

#ifndef filter_Grid_h
#define filter_Grid_h

#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <fstream>
#include <vector>
#include <string>

#include <proj_api.h>

#include "Point.h"

class Grid
{
public:
    //Grid(const char* inputFile, double cellSize);
    Grid(const char* inputFile, double cellSize, bool projectFromWGS84, bool flipZ, bool fXY);
    void calcGrid(void (*func)(Point&, double&, double&, double&));
    void write(const char* outFile, char delimiter = ' ');
    
private:
    void findBounds();
    bool readLine(double &x, double &y, double &z);
    void projectXY(double &x, double &y);
    size_t getCoord(double x, double y);
    
    std::ifstream ifs;
    size_t cols, rows;
    double cellSize;
    double minx, maxx, miny, maxy, minz, maxz;
    std::vector<Point> v;
    
    bool projectFromWGS84;
    projPJ pj_utm, pj_latlong;
    bool flipSignOnZ, flipXY;
};


#endif
