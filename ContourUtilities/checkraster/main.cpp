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
#include <iomanip>


#include <gdal/gdal_priv.h>
#include <gdal/cpl_conv.h>

int main(int argc, const char * argv[])
{
    
    GDALDataset  *poDataset;
    
    GDALAllRegister();
    
    poDataset = (GDALDataset *) GDALOpen( argv[1], GA_ReadOnly );
    if( poDataset != NULL ) {

        GDALRasterBand  *poBand;
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
        noDataVal = poBand->GetNoDataValue();
        
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
        std::ofstream ofs(argv[3]);
        ofs <<std::setprecision(2)<<std::fixed;
        while (ifs >> x >> y >> z) {
            col = floor( (x-tl_x)/cellsize_x );
            row = floor( (y-tl_y)/cellsize_y );
//            std::cerr << col << ", " << row << std::endl;
//            std::cerr << row*nXSize+col << std::endl;
            
            if ( (col<nXSize and row<nYSize) and pafImage[row*nXSize+col] != noDataVal ) {
                
                vert_dist = pafImage[row*nXSize+col]-z;
//                std::cout << pafImage[row*nXSize+col] << " " << z  << " diff: " << vert_dist <<std::endl;
                if ((vert_dist < epsilon) and (vert_dist > -epsilon)) vert_dist=0;
                squared_sum += (vert_dist*vert_dist);
                ++pointCount;                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                     
                
                if (vert_dist >= 0) safeCount++;
                
                max_vdist = max_vdist < vert_dist ? vert_dist : max_vdist;
                min_vdist = min_vdist > vert_dist ? vert_dist : min_vdist;
                
                ofs << " " << x;
                ofs << " " << y;
                ofs << " " << z;
                ofs << " " << vert_dist;
                ofs << std::endl;
                
            } else std::cerr << "point outside raster boundaries" << std::endl;
            ++lineCount;
        }
        ofs.close();
        ifs.close();
        
        std::cout << "Safe points: " << safeCount << " out of " << pointCount << " ( " << lineCount << " )" << std::endl;
        std::cout << "Vertical RMS error: " << sqrt(squared_sum/pointCount) << std::endl;
        std::cout << "Max error: " << max_vdist << ",  Min error " << min_vdist << std::endl;
        
        CPLFree(pafImage);
    }
    return 0;
}

