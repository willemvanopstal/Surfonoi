//
//  main.cpp
//  qcraster
//
//  Created by Ravi on 29/5/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include <iostream>
#include <cmath>
#include <fstream>

#include <GDAL/gdal_priv.h>
//#include <GDAL/cpl_string.h>
#include <GDAL/cpl_conv.h>
//#include <GDAL/ogr_spatialref.h>

int main(int argc, const char * argv[])
{
    
    GDALDataset  *poDataset;
    
    GDALAllRegister();
    
    poDataset = (GDALDataset *) GDALOpen( argv[1], GA_ReadOnly );
    if( poDataset != NULL ) {

        GDALRasterBand  *poBand;
        int             nBlockXSize, nBlockYSize;
        int             bGotMin, bGotMax;
        double          adfMinMax[2];
        double          adfGeoTransform[6];
        double          tl_x,tl_y,cellsize_x, cellsize_y;
        double          noDataVal;
        
        if( poDataset->GetGeoTransform( adfGeoTransform ) == CE_None )
        {
            tl_x = adfGeoTransform[0];
            tl_y = adfGeoTransform[3];
            cellsize_x = adfGeoTransform[1];
            cellsize_y = adfGeoTransform[5];            
        }
        
        poBand = poDataset->GetRasterBand( 1 );
        poBand->GetBlockSize( &nBlockXSize, &nBlockYSize );
        noDataVal = poBand->GetNoDataValue();
        adfMinMax[0] = poBand->GetMinimum( &bGotMin );
        adfMinMax[1] = poBand->GetMaximum( &bGotMax );
        
        float *pafImage;
        int   nXSize = poBand->GetXSize();
        int   nYSize = poBand->GetYSize();
        std::cerr << nXSize << " | " << nYSize << std::endl;
        
        pafImage = (float *) CPLMalloc(sizeof(float)*nXSize*nYSize);
        poBand->RasterIO( GF_Read, 0, 0, nXSize, nYSize, 
                         pafImage, nXSize, nYSize, GDT_Float32, 
                         0, 0 );

        double epsilon = 1e-5;
        double x, y, z;
        double vert_dist, squared_sum=0, max_vdist=0, min_vdist=0;
        int col, row;
        int safeCount=0;
        int pointCount=0;
        int lineCount=0;
        
        std::ifstream ifs(argv[2]);
        while (ifs >> x >> y >> z) {
            col = floor( (x-tl_x)/cellsize_x );
            row = floor( (y-tl_y)/cellsize_y );
//            std::cerr << col << ", " << row << std::endl;
//            std::cerr << row*nXSize+col << std::endl;
            
            if ( (col<nXSize and row<nYSize) and pafImage[row*nXSize+col] != noDataVal ) {
                
                vert_dist = pafImage[row*nXSize+col]-z;
                
                if (vert_dist > -epsilon) ++safeCount;
                squared_sum += (vert_dist*vert_dist);
                ++pointCount;
                
                max_vdist = max_vdist < vert_dist ? vert_dist : max_vdist;
                min_vdist = min_vdist > vert_dist ? vert_dist : min_vdist;
            } else std::cerr << "point outside raster boundaries" << std::endl;
            ++lineCount;
        }
        ifs.close();
        
        std::cout << "Safe points: " << safeCount << " out of " << pointCount << " ( " << lineCount << " )" << std::endl;
        std::cout << "RMS error: " << sqrt(squared_sum/pointCount) << std::endl;
        std::cout << "Max error: " << max_vdist << ",  Min error " << min_vdist << std::endl;
        
        CPLFree(pafImage);
    }
    return 0;
}

