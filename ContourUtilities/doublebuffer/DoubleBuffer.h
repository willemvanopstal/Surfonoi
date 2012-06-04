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
#include <gdal/ogrsf_frmts.h>

//GEOS
#include <geos/geom/Coordinate.h>
#include <geos/geom/LineString.h>
#include <geos/geom/PrecisionModel.h>
#include <geos/geom/GeometryFactory.h>
#include <geos/geom/CoordinateSequence.h>
#include <geos/geom/CoordinateArraySequence.h>
#include <geos/operation/buffer/BufferParameters.h>
#include <geos/operation/buffer/BufferBuilder.h>

#ifdef WITHGEOSBIL
#include <geos/operation/buffer/BufferInputLineSimplifier.h>
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
