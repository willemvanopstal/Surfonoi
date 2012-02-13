//
//  Grid.cpp
//  filter
//
//  Created by Ravi Peters on 30/1/12.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

#include <iostream>
#include <iomanip>
#include <cmath>
#include "Grid.h"

Grid::Grid(const char* inputFile, double cellSize): 
ifs(inputFile), cellSize(cellSize)//, minx(0), maxx(0), miny(0), maxy(0)
{
    findBounds();
    cols = static_cast<size_t> ((maxx-minx)/cellSize + 1);
    rows = static_cast<size_t> ((maxy-miny)/cellSize + 1);
    
    v = std::vector<Point>(cols*rows);
}



void Grid::calcGrid()
{
    double x,y,z;
    
    while (ifs >> x >> y >> z) {
        size_t t = getCoord(x,y);
        if (!v[t].isSet())
            v[t] = Point::Point(x, y, z);
        else
            func(v[t],x, y, z);
    }
}

void Grid::write(const char* outFile, char d)
{
    std::ofstream ofs(outFile);
    std::vector<Point>::const_iterator it;
    
    for(it=v.begin(); it!=v.end(); ++it)
    {
        if((*it).isSet()) ofs << std::setprecision(9) << (*it).x() << d << (*it).y() << d << (*it).z() << std::endl;
    }
    
    ofs.close();
}

size_t Grid::getCoord(double x, double y)
{
    size_t r = static_cast<size_t>( floor(fabs(y-miny) / cellSize) );
    size_t c = static_cast<size_t>( floor(fabs(x-minx) / cellSize) );
    
    return r * cols + c;
}
