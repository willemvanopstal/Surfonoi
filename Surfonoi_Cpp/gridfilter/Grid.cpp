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

#include "Grid.h"

Grid::Grid(const char* inputFile, double cellSize, bool pfW, bool fSOZ, bool fXY): 
ifs(inputFile), cellSize(cellSize), projectFromWGS84(pfW), flipSignOnZ(fSOZ), flipXY(fXY)
{
//    findBounds();
    std::string inputFile_bounds = inputFile;
    inputFile_bounds.append(".bounds");
    
    std::ifstream in_bounds(inputFile_bounds.c_str());
    in_bounds >> minx >> maxx >> miny >> maxy >> minz >> maxz;
    in_bounds.close();
    
    std::cout << minx << " " << maxx << " " << miny << " " << maxy << " " << minz << " " << maxz <<std::endl;

    if(projectFromWGS84) {
        if (!(pj_latlong = pj_init_plus("+init=epsg:4326")) )
            exit(1);

        std::stringstream utm_ss;
        std::cout << (miny+(maxy-miny)/2);
        int utmzone = 30+static_cast<int>( floor((minx+(maxx-minx)/2)/6) );
        
        utm_ss << "+proj=utm +zone=" << utmzone << " +ellps=WGS84";
        std::cout << "Projecting on: " << utm_ss.str() << std::endl;

        if (!(pj_utm = pj_init_plus(utm_ss.str().c_str() )) )
            exit(1);
        
        projectXY(minx, miny);
        projectXY(maxx, maxy);
    }
    
    std::cout << minx << " " << maxx << " " << miny << " " << maxy << " " << minz << " " << maxz <<std::endl;
    
    cols = static_cast<size_t> ((maxx-minx)/cellSize + 1);
    rows = static_cast<size_t> ((maxy-miny)/cellSize + 1);
    std::cout << cols << " " << rows << std::endl;
    v = std::vector<Point>(cols*rows);
}

bool Grid::readLine(double &x, double &y, double &z)
{
    if (flipXY)
        return ifs >> y >> x >> z;
    else
        return ifs >> x >> y >> z;
}

void Grid::projectXY(double &x, double &y)
{
    x *= DEG_TO_RAD;
    y *= DEG_TO_RAD;
    pj_transform(pj_latlong, pj_utm, 1, 1, &x, &y, NULL );
}

void Grid::findBounds()
{
    double x, y, z;

    if (readLine(x,y,z))
    {
        minx=maxx=x;
        miny=maxy=y;
        minz=maxz=z;
        ifs.clear();
        ifs.seekg(0);
    }
    
    while (readLine(x,y,z)){
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

    while (readLine(x, y, z)) {

        if(projectFromWGS84) projectXY(x,y);
        size_t t = getCoord(x,y);
        
        if (!v[t].isSet())
            v[t] = Point(x, y, z);
        else
            func(v[t],x, y, z);
    }
}

void Grid::write(const char* outFile, char d)
{
    std::ofstream ofs(outFile);
    std::vector<Point>::const_iterator it;
        

    
    // print boundaries to first 6 lines
//    ofs << minx << std::endl << maxx << std::endl <<  miny << std::endl << maxy << std::endl;


    for(it=v.begin(); it!=v.end(); ++it)
    {
        if(it->isSet()) {
            ofs << std::setprecision(2)<<std::fixed << it->x() << d << it->y() << d;
            if(flipSignOnZ)
                ofs << -it->z() << std::endl;
            else
                ofs << it->z() << std::endl;
        }
    }
    
    ofs.close();
    
    std::string outFile_bounds = outFile;
    outFile_bounds.append(".bounds");
    
    std::ofstream ofs_bounds(outFile_bounds.c_str());
    ofs_bounds <<std::setprecision(2)<<std::fixed << minx << std::endl << maxx << 
    std::endl << miny << std::endl << maxy << std::endl;

    if(flipSignOnZ)
        ofs_bounds << -maxz << std::endl << -minz << std::endl;
    else
        ofs_bounds << minz << std::endl << maxz << std::endl;
    
    ofs_bounds.close();
}

size_t Grid::getCoord(double x, double y)
{
    size_t r = static_cast<size_t>( floor(fabs(y-miny) / cellSize) );
    size_t c = static_cast<size_t>( floor(fabs(x-minx) / cellSize) );

    return r * cols + c;
}
