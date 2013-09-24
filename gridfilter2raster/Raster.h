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

#ifndef filter2Raster_Raster_h
#define filter2Raster_Raster_h

#include <iostream>
#include <cmath>
#include <algorithm>
#include <cfloat>
#include <fstream>

#include <gdal_priv.h>
#include <cpl_string.h>
#include <cpl_conv.h>
#include <ogr_spatialref.h>

enum alg {AVG,MIN,MAX,CNT};
//enum {buffN = 4*16184};
class Raster
{
public:
    Raster(const char* inputFile, const char* WKGCS, double cellsize);
    void compute(float val[], int16_t cnt[], alg);
    size_t getCoord(double &x, double &y);
    void write(alg a, void * dataPtr, const char* outFile);

    double cellSize, minx, miny, maxx, maxy;
    int dimx, dimy;
    size_t lineCount;
private:
    void getBounds();
    void avg(double &x, double &y, double &val, float [], int16_t []);
    void min(double &x, double &y, double &val, float []);
    void max(double &x, double &y, double &val, float []);
    void cnt(double &x, double &y, int16_t []);
    std::ifstream ifs;
    OGRSpatialReference oSRS;
//    char _buffer[buffN];
    double noDataVal;
};


#endif
