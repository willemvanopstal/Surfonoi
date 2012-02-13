//
//  Raster.h
//  filter2Raster
//
//  Created by Ravi Peters on 2/2/12.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

#ifndef filter2Raster_Raster_h
#define filter2Raster_Raster_h

#include <iostream>
#include <cmath>
#include <algorithm>
#include <cfloat>
#include <fstream>

#include <GDAL/gdal_priv.h>
#include <GDAL/cpl_string.h>
#include <GDAL/cpl_conv.h>
#include <GDAL/ogr_spatialref.h>

enum alg {AVG,MIN,MAX};
enum {buffN = 4*16184};
class Raster
{
public:
    Raster(const char* inputFile, const char* WKGCS, double cellsize);
    void compute(float val[], int16_t cnt[], alg);
    size_t getCoord(double &x, double &y);
    void write(float val[], const char* outFile);

    double cellSize, minx, miny, maxx, maxy;
    int dimx, dimy;
    size_t lineCount;
private:
    void getBounds();
    void avg(double &x, double &y, double &val, float [], int16_t []);
    void min(double &x, double &y, double &val, float [], int16_t []);
    void max(double &x, double &y, double &val, float [], int16_t []);
    std::ifstream ifs;
    OGRSpatialReference oSRS;
    char _buffer[buffN];
    double noDataVal;
};


#endif
