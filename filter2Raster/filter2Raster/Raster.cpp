//
//  Raster.cpp
//  filter2Raster
//
//  Created by Ravi Peters on 2/2/12.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

#include "Raster.h"

Raster::Raster(const char* inputFile, const char* WKGCS, double cellsize):
        ifs(inputFile), cellSize(cellsize), lineCount(0)
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

void Raster::compute(float val[], int16_t cnt[], alg a)
{
    noDataVal = a==MIN ? 99999 : -99999;
    std::fill(val+0, val+dimx*dimy, noDataVal);
    std::fill(cnt+0, cnt+dimx*dimy, 0);
    std::cout << "Filled arrays" <<std::endl;

    double x, y, z;
    while (ifs >> x >> y >> z)
    {
        if (a==MIN) {
            min(x,y,z, val, cnt);
        } else if (a==MAX) {
            max(x,y,z, val, cnt);
        } else if (a==AVG) {
            avg(x,y,z, val, cnt);
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

inline void Raster::min(double &x, double &y, double &val, float vals[], int16_t cnt[])
{
    size_t c = getCoord(x,y);
    if (vals[c]>val) vals[c] = val;
    ++cnt[c];
}

inline void Raster::max(double &x, double &y, double &val, float vals[], int16_t cnt[])
{
    size_t c = getCoord(x,y);
    if (vals[c]<val) vals[c] = val;
    ++cnt[c];
}

size_t Raster::getCoord(double &x, double &y)
{
    size_t r = static_cast<size_t>( floor((y-miny) / cellSize) );
    size_t c = static_cast<size_t>( floor((x-minx) / cellSize) );
    
    return r * dimx + c;
}

void Raster::write(float val[], const char* outFile)
{
    GDALDriver *poDriver = GetGDALDriverManager()->GetDriverByName("GTiff");    
    GDALDataset *poDstDS;
    
    char **papszOptions = NULL;
    poDstDS = poDriver->Create( outFile, dimx, dimy, 1, GDT_Float32, 
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
                     val, dimx, dimy, GDT_Float32, 0, 0 );    
    poBand->SetNoDataValue(noDataVal);
    /* Once we're done, close properly the dataset */
    GDALClose( (GDALDatasetH) poDstDS );
}
