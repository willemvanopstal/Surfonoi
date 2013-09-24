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

// (see http://trac.osgeo.org/geos/ticket/548 ) Patch is in GEOS trunk now and planned to be in GEOS 3.4
//#define WITHGEOSBIL

#ifndef doublebuffer_DoubleBuffer_h
#define doublebuffer_DoubleBuffer_h

#include <map>
#include <vector>

//OGR
#include <ogrsf_frmts.h>

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
