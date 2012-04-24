//
//  Filter.h
//  filter
//
//  Created by Ravi Peters on 30/1/12.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

#ifndef filter_Filter_h
#define filter_Filter_h

#include <proj_api.h>

#include <fstream>
#include <iostream>
#include <cstdlib>
#include <cmath>
#include <sstream>
#include <iomanip>

class Filter
{
public:
    Filter(const char*, const char*, bool pfW, bool fSOZ, bool fXY);
    void copy_all();
    void copy_nth(int);
    void copy_percentage(int);
    size_t getLineCount();
private:
    bool readLine(double &x, double &y, double &z);
    void writeLine(double &x, double &y, double &z);
    void projectXY(double &x, double &y);
    void findBounds();
    
    std::ifstream ifs;
    std::ofstream ofs;

    size_t lineCount;
    double minx,maxx,miny,maxy,minz,maxz;
    char delimiter;
    bool projectFromWGS84;
    projPJ pj_utm, pj_latlong;
    bool flipXY;
    bool flipSignOnZ;
};


#endif
