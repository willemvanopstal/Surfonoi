//
//  DoubleBuffer.h
//  doublebuffer
//
//  Created by Ravi on 15/5/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

//#define WITHGEOSBIL

#ifndef doublebuffer_DoubleBuffer_h
#define doublebuffer_DoubleBuffer_h

#include <map>
#include <vector>

//OGR
#include <GDAL/ogrsf_frmts.h>

//GEOS
#include <GEOS/GEOS.h>
#include <GEOS/operation/buffer/BufferParameters.h>
#include <GEOS/operation/buffer/BufferBuilder.h>

#ifdef WITHGEOSBIL
//this requires a modified geos...:
#include <GEOS/operation/buffer/BufferInputLineSimplifier.h>
#endif

typedef std::vector< geos::geom::CoordinateSequence* > coordVec;
typedef std::map< double, coordVec > ContourMap;

enum simplificationMethod  {GEOSBIL, DOUBLEBUFF};

class DoubleBuffer {
public:
    DoubleBuffer(const char* inputFile, const char* outputFile, const char* depthAttributeName);
    ~DoubleBuffer();
    
    void perform(simplificationMethod, double tolerance);
    
private:
    inline geos::geom::CoordinateArraySequence* OGRLineString2GEOSCoordinateSequence(OGRLineString* ogrLineString);
    const geos::geom::Geometry* cleanSingleBufferOutput(const geos::geom::Geometry* inputGeom);
    
    void readShapefile(const char* inputFile);
    void prepareOutputShapefile(const char* outputFile);
    void writeShapefileLayer(ContourMap contourData, const char* layerName);

    ContourMap performDB(double bufferTolerance);
    
#ifdef WITHGEOSBIL
    CoordinateArraySequence* doSingleBIL(CoordinateSequence* inputLine, double bufferTolerance);
    ContourMap performBIL(double bufferTolerance);
#endif
    
    const char* depthAttributeName;
    ContourMap inputContours;
    OGRSpatialReference * spatialReference;
    OGRDataSource *poDstDS; //output datasource
    geos::geom::PrecisionModel* precisionModel;
};
#endif
