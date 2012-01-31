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

void Grid::findBounds()
{
    double x, y, z;

    if (ifs >> x >> y >> z)
    {
        minx=maxx=x;
        miny=maxy=y;
        minz=maxz=z;
        ifs.clear();
        ifs.seekg(0);
    }
    
    while (ifs >> x >> y >> z){
        if(x < minx) minx=x;
        if(x > maxx) maxx=x;
        if(y < miny) miny=y;
        if(y > maxy) maxy=y;
        if(z < minz) minz=z;
        if(z > maxz) maxz=z;
    }
    
    ifs.clear();
    ifs.seekg(0);
}

void Grid::calcGrid(void (*func)(Point&, double&, double&, double&))
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
