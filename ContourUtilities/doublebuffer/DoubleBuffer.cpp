//
//  DoubleBuffer.cpp
//  doublebuffer
//
//  Created by Ravi on 15/5/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "DoubleBuffer.h"

DoubleBuffer::DoubleBuffer(const char* inputFile, const char* outputFile, const char* depthAttributeName_):depthAttributeName(depthAttributeName_)  {
    OGRRegisterAll();
    precisionModel = new geos::geom::PrecisionModel(10000);
    readShapefile(inputFile);
    prepareOutputShapefile(outputFile);
}

DoubleBuffer::~DoubleBuffer() {

    OGRDataSource::DestroyDataSource( poDstDS );
}

void DoubleBuffer::perform(simplificationMethod method, double tolerance) {
    if (method == GEOSBIL){
        #ifdef WITHGEOSBIL
            performBIL(tolerance);
        #else
            std::cerr << "GEOSBIL is not enabled in this build" << std::endl;
        #endif
    } else if (method == DOUBLEBUFF) {
        performDB(tolerance);
    }
}

void DoubleBuffer::readShapefile(const char* inputFile)
{
    OGRDataSource       *poDS;
    
    poDS = OGRSFDriverRegistrar::Open( inputFile, FALSE );
    if( poDS == NULL )
    {
        printf( "Open failed.\n" );
        exit( 1 );
    }
    
    OGRLayer  *poSrcLayer;
    
    poSrcLayer = poDS->GetLayer( 0 );
    
    if ( poSrcLayer->GetSpatialRef() ) {
        spatialReference = poSrcLayer->GetSpatialRef()->Clone();
    } else spatialReference=NULL;


    OGRFeature *poFeature;
    
    poSrcLayer->ResetReading();
    while( (poFeature = poSrcLayer->GetNextFeature()) != NULL )
    {
        OGRFeatureDefn *poFdDefn = poSrcLayer->GetLayerDefn();
        double depth;
        int iField;
        
        iField = poFdDefn->GetFieldIndex(depthAttributeName);
        OGRFieldDefn* poFieldDefn = poFdDefn->GetFieldDefn(iField);
        
        if( poFieldDefn->GetType() == OFTReal ) {
            depth = poFeature->GetFieldAsDouble(iField);
        } else {
            std::cerr << "Error: Reading level attribute";
            exit(1);
        }
            
        OGRGeometry *poGeometry;
        
        poGeometry = poFeature->GetGeometryRef();
        if( poGeometry != NULL 
           && wkbFlatten(poGeometry->getGeometryType()) == wkbLineString )
        {
            OGRLineString* poLineString = (OGRLineString *) poGeometry;
            geos::geom::CoordinateArraySequence* lsseq = OGRLineString2GEOSCoordinateSequence(poLineString);
            
            inputContours[depth].push_back(lsseq);
        }
        else
        {
            printf( "no point geometry\n" );
        }       
        OGRFeature::DestroyFeature( poFeature );
    }
    
    OGRDataSource::DestroyDataSource( poDS );  
}

void DoubleBuffer::prepareOutputShapefile(const char* outputFile) {
    const char *pszDriverName = "ESRI Shapefile";
    OGRSFDriver *poDriver;
    
    poDriver = OGRSFDriverRegistrar::GetRegistrar()->GetDriverByName(pszDriverName );
    if( poDriver == NULL ) {
        printf( "%s driver not available.\n", pszDriverName );
        exit( 1 );
    }
    
    poDstDS = poDriver->CreateDataSource( outputFile, NULL );
    if( poDstDS == NULL ) {
        printf( "Creation of output file failed.\n" );
        exit( 1 );
    }
}

void DoubleBuffer::writeShapefileLayer(ContourMap contourData, const char* layerName) {
    
    OGRLayer *poLayer;
    
    poLayer = poDstDS->CreateLayer( layerName, spatialReference, wkbLineString, NULL );
    if( poLayer == NULL ) {
        printf( "Layer creation failed.\n" );
        exit( 1 );
    }
    
    OGRFieldDefn oField( depthAttributeName, OFTReal );
    
    if( poLayer->CreateField( &oField ) != OGRERR_NONE ) {
        printf( "Creating oftreal field failed.\n" );
        exit( 1 );
    }
    
    for (ContourMap::iterator it = contourData.begin(); it != contourData.end(); ++it) {
        
        for (coordVec::iterator jt = it->second.begin(); jt != it->second.end(); ++jt) {
            
            OGRLineString *ls = new OGRLineString();
            
            OGRFeature *poFeature;
            poFeature = OGRFeature::CreateFeature( poLayer->GetLayerDefn() );
            poFeature->SetField( depthAttributeName, it->first );
            
            std::vector<geos::geom::Coordinate> coordinates;
            (*jt)->toVector(coordinates);
            for (std::vector<geos::geom::Coordinate>::iterator cit = coordinates.begin(); cit != coordinates.end(); ++cit) {
                ls->addPoint(cit->x, cit->y, cit->z);
            }
            
            poFeature->SetGeometry( ls );
            
            if( poLayer->CreateFeature( poFeature ) != OGRERR_NONE )
            {
                printf( "Failed to create feature in shapefile.\n" );
                exit( 1 );
            }
            
            OGRFeature::DestroyFeature( poFeature );
        }
    }
}

inline geos::geom::CoordinateArraySequence* DoubleBuffer::OGRLineString2GEOSCoordinateSequence(OGRLineString* ogrLineString) {
    geos::geom::CoordinateArraySequence* cseq = new geos::geom::CoordinateArraySequence;

    for (int i=0; i != ogrLineString->getNumPoints(); ++i){
        geos::geom::Coordinate coord(ogrLineString->getX(i), ogrLineString->getY(i), ogrLineString->getZ(i));
        precisionModel->makePrecise(coord);
        cseq->add(coord);
    }
    
    return cseq;    
}

#ifdef WITHGEOSBIL
CoordinateArraySequence* DoubleBuffer::doSingleBIL(CoordinateSequence* inputLine, double bufferTolerance) {
    std::auto_ptr<geos::geom::CoordinateSequence> coords_ = geos::operation::buffer::BufferInputLineSimplifier::simplify(*inputLine, bufferTolerance);
    geos::geom::CoordinateSequence& coords = *coords_;
    geos::geom::CoordinateSequence* coords_copy = coords.clone();
    geos::geom::CoordinateArraySequence* coords_copy_ = dynamic_cast<geos::geom::CoordinateArraySequence *>(coords_copy);

    return coords_copy_;
}

ContourMap DoubleBuffer::performBIL(double bufferTolerance) {
    
    ContourMap contourOut;
    
    for (ContourMap::iterator it = inputContours.begin(); it != inputContours.end(); ++it) {
        
        for (coordVec::iterator jt = it->second.begin(); jt != it->second.end(); ++jt) {
            contourOut[it->first].push_back( doSingleBIL(*jt, bufferTolerance) );
        }
    }
    
    writeShapefileLayer(contourOut, "bil");
    
    return contourOut;
}
#endif

ContourMap DoubleBuffer::performDB(double bufferTolerance) {
    
    ContourMap contourOut, intermediateOut;
    
    geos::geom::GeometryFactory geomFactory(precisionModel);
    geos::operation::buffer::BufferParameters bufferParam;
    bufferParam.setSingleSided(true);
    geos::operation::buffer::BufferBuilder bufferBuild(bufferParam);
    bufferBuild.setWorkingPrecisionModel(precisionModel);
    

    for (ContourMap::iterator it = inputContours.begin(); it != inputContours.end(); ++it) {
        
        for (coordVec::iterator jt = it->second.begin(); jt != it->second.end(); ++jt) {
            
            const geos::geom::Geometry* contourBufferUp;
            
            const geos::geom::LineString* geomLinestring = geomFactory.createLineString(*jt);
            contourBufferUp = bufferBuild.bufferLineSingleSided(geomLinestring, bufferTolerance, true);

            if(!contourBufferUp->isEmpty()) {
                for (size_t i = 0; i<contourBufferUp->getNumGeometries(); ++i) {
                    const geos::geom::LineString* geom = dynamic_cast<const geos::geom::LineString*>(contourBufferUp->getGeometryN(i));
//                    std::cerr << geom->toString() << std::endl;
                    if(!geom->isEmpty()) {
                        const geos::geom::Geometry* contourBufferUpDown = bufferBuild.bufferLineSingleSided(geom->reverse(), bufferTolerance, true);
                        intermediateOut[it->first].push_back(geom->getCoordinates());
                        for (size_t j = 0; j<contourBufferUpDown->getNumGeometries(); ++j) {
                            if ( !contourBufferUpDown->isEmpty() )
                                contourOut[it->first].push_back(contourBufferUpDown->getGeometryN(j)->getCoordinates());
                        }
                    }
                }
            }
        }
    }

//    writeShapefileLayer(intermediateOut, "singlebuffered");
    writeShapefileLayer(contourOut, "doublebuffered");
    
    return contourOut;
}
