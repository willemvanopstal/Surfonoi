//
//  Grid.cpp
//  filter
//
//  Created by Ravi Peters on 30/1/12.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

#include "Grid.h"

Grid::Grid(const char* inputFile, double cellSize, bool pfW, bool fSOZ): 
ifs(inputFile), cellSize(cellSize), projectFromWGS84(pfW), flipSignOnZ(fSOZ)
{
    findBounds();
    
    if(projectFromWGS84) {
        if (!(pj_latlong = pj_init_plus("+init=epsg:4326")) )
            exit(1);

        std::stringstream utm_ss;
        int utmzone = 30+static_cast<int>( floor((minx+(maxx-minx)/2)/6) );
        
        utm_ss << "+proj=utm +zone=" << utmzone << " +ellps=WGS84";
        std::cout << "Projecting on: " << utm_ss.str() << std::endl;

        if (!(pj_utm = pj_init_plus(utm_ss.str().c_str() )) )
            exit(1);
        
        minx *= DEG_TO_RAD;
        maxx *= DEG_TO_RAD;
        miny *= DEG_TO_RAD;
        maxy *= DEG_TO_RAD;
        
        pj_transform(pj_latlong, pj_utm, 1, 1, &minx, &miny, NULL );
        pj_transform(pj_latlong, pj_utm, 1, 1, &maxx, &maxy, NULL );
    }
    
    cols = static_cast<size_t> ((maxx-minx)/cellSize + 1);
    rows = static_cast<size_t> ((maxy-miny)/cellSize + 1);
    std::cout << cols << " " << rows << std::endl;
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
    
    std::cout << minx << " " << maxx << " " << miny << " " << maxy << " " << minz << " " << maxz <<std::endl;
    
    ifs.clear();
    ifs.seekg(0);
}

void Grid::calcGrid(void (*func)(Point&, double&, double&, double&))
{
    double x,y,z;

    while (ifs >> x >> y >> z) {

        if(projectFromWGS84){
            x *= DEG_TO_RAD;
            y *= DEG_TO_RAD;
            pj_transform(pj_latlong, pj_utm, 1, 1, &x, &y, NULL );
        }
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
        
    // print boundaries to first 6 lines
    ofs << minx << std::endl << maxx << std::endl <<  miny << std::endl << maxy << std::endl;
    if(flipSignOnZ)
        ofs << -maxz << std::endl << -minz << std::endl;
    else
        ofs << minz << std::endl << maxz << std::endl;

    for(it=v.begin(); it!=v.end(); ++it)
    {
        if(it->isSet()) {
            
            ofs << std::setprecision(9) << it->x() << d << it->y() << d;
            if(flipSignOnZ)
                ofs << -it->z() << std::endl;
            else
                ofs << it->z() << std::endl;
        }
    }
    
    ofs.close();
}

size_t Grid::getCoord(double x, double y)
{
    size_t r = static_cast<size_t>( floor(fabs(y-miny) / cellSize) );
    size_t c = static_cast<size_t>( floor(fabs(x-minx) / cellSize) );

    return r * cols + c;
}
