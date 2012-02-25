//
//  Grid.h
//  filter
//
//  Created by Ravi Peters on 30/1/12.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

#ifndef filter_Grid_h
#define filter_Grid_h

#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <fstream>
#include <vector>

#include <proj_api.h>

#include "Point.h"

class Grid
{
public:
    //Grid(const char* inputFile, double cellSize);
    Grid(const char* inputFile, double cellSize, bool projectFromWGS84);
    void calcGrid(void (*func)(Point&, double&, double&, double&));
    void write(const char* outFile, char delimiter = ' ');
    
private:
    void findBounds();
    size_t getCoord(double x, double y);
    
    std::ifstream ifs;
    size_t cols, rows;
    double cellSize;
    double minx, maxx, miny, maxy, minz, maxz;
    std::vector<Point> v;
    
    bool projectFromWGS84;
    projPJ pj_utm, pj_latlong;
};


#endif
