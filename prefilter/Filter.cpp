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

#include "Filter.h"

Filter::Filter(const char *inFile, const char *outFile, bool pfW, bool fSOZ, bool fXY, int precise):
    ifs(inFile), ofs(outFile), lineCount(0), delimiter(' '), 
    projectFromWGS84(pfW), flipXY(fXY), flipSignOnZ(fSOZ), precision(precise)
{
    if(!ifs) {
        std::cerr << "Unable to open file" << std::endl;
        exit(1);
    }
    
    findBounds();
    
    if(projectFromWGS84) {
        
        if (!(pj_latlong = pj_init_plus("+init=epsg:4326")) )
            exit(1);
        
        std::stringstream utm_ss;
        int utmzone = 30+static_cast<int>( floor((minx+(maxx-minx)/2)/6) );
        
        utm_ss << "+proj=utm +zone=" << utmzone;
        if ((miny+(maxy-miny)/2) < 0) utm_ss <<" +south";
        utm_ss << " +ellps=WGS84" << " +datum=WGS84";
        
        std::cout << "Projecting on: " << utm_ss.str() << std::endl;
        
        if (!(pj_utm = pj_init_plus(utm_ss.str().c_str() )) ) {
            std::cout << "Projection error: " << pj_utm << std::endl;
            exit(1);
        }
        
        projectXY(minx, miny);
        projectXY(maxx, maxy);
    }
    
    // write .bounds file
    std::string outFile_bounds = outFile;
    outFile_bounds.append(".bounds");
    
    std::ofstream ofs_bounds(outFile_bounds.c_str());
    ofs_bounds <<std::setprecision(precision)<<std::fixed << minx << std::endl << maxx <<
    std::endl << miny << std::endl << maxy << std::endl;
    
    if(flipSignOnZ)
        ofs_bounds << -maxz << std::endl << -minz << std::endl;
    else
        ofs_bounds << minz << std::endl << maxz << std::endl;
    
    // set precision of output file
    ofs <<std::setprecision(precision)<<std::fixed;
}

void Filter::findBounds()
{
    double x, y, z;
    
    if (readLine(x,y,z)) {
        minx=maxx=x;
        miny=maxy=y;
        minz=maxz=z;
        ifs.clear();
        ifs.seekg(0);
    }
    
    while (readLine(x,y,z)) {
        if(x < minx) minx=x;
        if(x > maxx) maxx=x;
        if(y < miny) miny=y;
        if(y > maxy) maxy=y;
        if(z < minz) minz=z;
        if(z > maxz) maxz=z;
    }
    
    std::cout << "boundaries are:\n" << minx<<std::endl<<maxx<<std::endl<<miny<<std::endl<<maxy<<std::endl<<minz<<std::endl<<maxz<<std::endl; 
    
    ifs.clear();
    ifs.seekg(0);
}

bool Filter::readLine(double &x, double &y, double &z)
{
    if (flipXY)
        return ifs >> y >> x >> z;
    else
        return ifs >> x >> y >> z;
}

void Filter::projectXY(double &x, double &y)
{
    x *= DEG_TO_RAD;
    y *= DEG_TO_RAD;
    pj_transform(pj_latlong, pj_utm, 1, 1, &x, &y, NULL );
}

void Filter::writeLine(double &x, double &y, double &z)
{
    if (projectFromWGS84)
        projectXY(x,y);
        
        ofs << x << delimiter << y << delimiter;
    
    if(flipSignOnZ) ofs << -z;
    else            ofs << z;
    
    ofs << std::endl;
}

void Filter::copy_all()
{
    double x,y,z;

    while (readLine(x,y,z)){
        writeLine(x,y,z);
        lineCount++;
    }
}

void Filter::copy_nth(int n)
{
    double x,y,z;

    while (readLine(x,y,z)){
        if (lineCount++ % n == 0)
            writeLine(x,y,z);
    }
}

void Filter::copy_percentage(int percentage)
{
    double x,y,z;
    
    while (readLine(x,y,z)){
        if (percentage >= rand()%100) {
            writeLine(x,y,z);
            lineCount++;
        }
    }
}

size_t Filter::getLineCount()
{
    return lineCount;
}