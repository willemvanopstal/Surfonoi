//
//  Grid.h
//  filter
//
//  Created by Ravi Peters on 30/1/12.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

#ifndef filter_Grid_h
#define filter_Grid_h

#include <fstream>
#include <vector>

class Grid
{
public:
    //Grid(const char* inputFile, double cellSize);
    Grid(const char* inputFile, double cellSize);
    void buildGrid();
    void write(const char* outFile, char delimiter = ' ');
    
private:
    void findBounds();
    size_t getCoord(double x, double y);
    
    std::ifstream ifs;
    size_t cols, rows;
    double cellSize;
    double minx, maxx, miny, maxy, minz, maxz;
    int count[];
    double value[];
//    std::vector<Point> v;
};


#endif
