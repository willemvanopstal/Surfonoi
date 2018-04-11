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

#ifndef filter_Filter_h
#define filter_Filter_h

#include <proj_api.h>

#include <fstream>
#include <iostream>
#include <cstdlib>
#include <cmath>
#include <sstream>
#include <string>
#include <iomanip>

class Filter
{
public:
    Filter(const char*, const char*, bool pfW, bool fSOZ, bool fXY, int precise);
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
    int precision;
};


#endif
