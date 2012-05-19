//
//  DoubleBuffer.h
//  doublebuffer
//
//  Created by Ravi on 15/5/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef doublebuffer_DoubleBuffer_h
#define doublebuffer_DoubleBuffer_h

//OGR
#include <GDAL/ogrsf_frmts.h>

//GEOS
#include <GEOS/GEOS.h>
//this requires a modified geos...:
#include <GEOS/operation/buffer/BufferInputLineSimplifier.h>
#include <GEOS/operation/buffer/BufferParameters.h>
#include <GEOS/operation/buffer/BufferBuilder.h>

#include <map>
#include <vector>

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
    CoordinateArraySequence* doSingleBIL(CoordinateSequence* inputLine, double bufferTolerance);
    
    void readShapefile(const char* inputFile);
    void prepareOutputShapefile(const char* outputFile);
    void writeShapefileLayer(ContourMap contourData, const char* layerName);
    ContourMap performBIL(double bufferTolerance);
    ContourMap performDB(double bufferTolerance);
    
    const char* depthAttributeName;
    ContourMap inputContours;
    OGRSpatialReference * spatialReference;
    OGRDataSource *poDstDS; //output datasource
    geos::geom::PrecisionModel* precisionModel;
};
#endif
