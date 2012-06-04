//
//  DoubleBuffer.h
//  doublebuffer
//
//  Created by Ravi on 15/5/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

// (see http://trac.osgeo.org/geos/ticket/548 ) Patch is also in GEOS trunk now
//#define WITHGEOSBIL

#ifndef doublebuffer_DoubleBuffer_h
#define doublebuffer_DoubleBuffer_h

#include <map>
#include <vector>

//OGR
#include <GDAL/ogrsf_frmts.h>

//GEOS
#include <GEOS/geom/Coordinate.h>
#include <GEOS/geom/LineString.h>
#include <GEOS/geom/PrecisionModel.h>
#include <GEOS/geom/GeometryFactory.h>
#include <GEOS/geom/CoordinateSequence.h>
#include <GEOS/geom/CoordinateArraySequence.h>
#include <GEOS/operation/buffer/BufferParameters.h>
#include <GEOS/operation/buffer/BufferBuilder.h>

#ifdef WITHGEOSBIL
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
