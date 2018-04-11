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

#include "Raster.h"

Raster::Raster(const char* inputFile, const char* WKGCS, double cellsize):
        cellSize(cellsize), lineCount(0), ifs(inputFile)
{
//    ifs.rdbuf()->pubsetbuf(_buffer, buffN);
    
    getBounds();
    
    if( EQUALN(WKGCS, "EPSG:",5) ) {
        oSRS.importFromEPSG( atoi(WKGCS+5) );
    } else if (EQUALN(WKGCS, "EPSGA:",6)) {
        oSRS.importFromEPSGA( atoi(WKGCS+6) );
    }
    
//    int utmzone = 30+static_cast<int>( floor((minx+(maxx-minx)/2)/6) );
//    bool north = miny<0;
//    oSRS.SetUTM( utmzone, north );
//
//    char * str;
//    cellSize = cellsizeMeters/oSRS.GetLinearUnits(&str);
//    std::cout<<"lu: " << oSRS.GetLinearUnits() <<std::endl;
    dimx = (maxx-minx)/cellSize + 1;
    dimy = (maxy-miny)/cellSize + 1;
    GDALAllRegister();
}

void Raster::getBounds()
{
    double x, y, z;
    
    if (ifs >> x >> y >> z)
    {
        minx=maxx=x;
        miny=maxy=y;
        ifs.clear();
        ifs.seekg(0);
    }
    
    while (ifs >> x >> y >> z){
        if(x < minx) minx=x;
        if(x > maxx) maxx=x;
        if(y < miny) miny=y;
        if(y > maxy) maxy=y;
        ++lineCount;
    }
    ifs.clear();
    ifs.seekg(0);
    std::cout << "Found bounds" <<std::endl;

}

void Raster::compute(float val[], int16_t count[], alg a)
{
    if (a==MIN)
        noDataVal = 99999;
    else if (a==CNT)
        noDataVal = 0;
    else
        noDataVal = -99999;
    
    std::fill(val+0, val+dimx*dimy, noDataVal);
    std::fill(count+0, count+dimx*dimy, 0);
    std::cout << "Filled arrays" <<std::endl;

    double x, y, z;
    while (ifs >> x >> y >> z)
    {
        if (a==MIN) {
            min(x,y,z, val);
        } else if (a==MAX) {
            max(x,y,z, val);
        } else if (a==AVG) {
            avg(x,y,z, val, count);
        } else if (a==CNT) {
            cnt(x,y, count);
        }
        
    }
    ifs.clear();
    ifs.seekg(0);
    std::cout << "Completed computations" <<std::endl;
}

inline void Raster::avg(double &x, double &y, double &val, float vals[], int16_t cnt[])
{
    size_t c = getCoord(x,y);
    vals[c]= (vals[c]*cnt[c]+val)/(cnt[c]+1);
    ++cnt[c];
}

inline void Raster::min(double &x, double &y, double &val, float vals[])
{
    size_t c = getCoord(x,y);
    if (vals[c]>val) vals[c] = val;
}

inline void Raster::max(double &x, double &y, double &val, float vals[])
{
    size_t c = getCoord(x,y);
    if (vals[c]<val) vals[c] = val;
}

inline void Raster::cnt(double &x, double &y, int16_t cnt[])
{
    size_t c = getCoord(x,y);
    ++cnt[c];
}

size_t Raster::getCoord(double &x, double &y)
{
    size_t r = static_cast<size_t>( floor((y-miny) / cellSize) );
    size_t c = static_cast<size_t>( floor((x-minx) / cellSize) );
    
    return r * dimx + c;
}

void Raster::write(alg a, void * dataPtr, const char* outFile)
{
    GDALDriver *poDriver = GetGDALDriverManager()->GetDriverByName("GTiff");
    GDALDataset *poDstDS;
    GDALDataType dataType;

    if (a == CNT)
        dataType = GDT_UInt16;
    else
        dataType = GDT_Float32;
    
    char **papszOptions = NULL;
    poDstDS = poDriver->Create( outFile, dimx, dimy, 1, dataType,
                               papszOptions );
    double adfGeoTransform[6] = { minx, cellSize, 0, miny, 0, cellSize };
    GDALRasterBand *poBand;
    
    poDstDS->SetGeoTransform( adfGeoTransform );
    
    //    std::cout << oSRS.SetWellKnownGeogCS( WKGCS );
    //    std::cout << pszSRS_WKT <<std::endl;
    
    char *pszSRS_WKT = NULL;
    oSRS.exportToWkt( &pszSRS_WKT );
    poDstDS->SetProjection( pszSRS_WKT );
    CPLFree( pszSRS_WKT );
    
    poBand = poDstDS->GetRasterBand(1);
    poBand->RasterIO( GF_Write, 0, 0, dimx, dimy,
                     dataPtr, dimx, dimy, dataType, 0, 0 );
    poBand->SetNoDataValue(noDataVal);
    /* Once we're done, close properly the dataset */
    GDALClose( (GDALDatasetH) poDstDS );
}