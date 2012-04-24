//
//  Filter.cpp
//  filter
//
//  Created by Ravi Peters on 30/1/12.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

#include "Filter.h"

Filter::Filter(const char *inFile, const char *outFile, bool pfW, bool fSOZ, bool fXY): 
    ifs(inFile), ofs(outFile), lineCount(0), delimiter(' '), 
    projectFromWGS84(pfW), flipXY(fXY), flipSignOnZ(fSOZ)
{
    if(!ifs) {
        std::cerr << "Unable to open file" << std::endl;
        exit(1);
    }
    
    if(projectFromWGS84) {
        findBounds();
        
        if (!(pj_latlong = pj_init_plus("+init=epsg:4326")) )
            exit(1);
        
        std::stringstream utm_ss;
//        std::cout << (miny+(maxy-miny)/2);
        int utmzone = 30+static_cast<int>( floor((minx+(maxx-minx)/2)/6) );
        
        utm_ss << "+proj=utm +zone=" << utmzone << " +ellps=WGS84";
        std::cout << "Projecting on: " << utm_ss.str() << std::endl;
        
        if (!(pj_utm = pj_init_plus(utm_ss.str().c_str() )) )
            exit(1);
        
        projectXY(minx, miny);
        projectXY(maxx, maxy);
    }
    
    ofs << std::setprecision(9);
    
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
    
    lineCount++;
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