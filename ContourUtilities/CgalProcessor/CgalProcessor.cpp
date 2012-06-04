#include "CgalProcessor.h"

CgalProcessor::CgalProcessor(const char *inputFile){
    if (dt.number_of_vertices() != 0)
        clear();
    
    std::string inputFile_bounds = inputFile;
    inputFile_bounds.append(".bounds");
    
    std::ifstream in_bounds(inputFile_bounds.c_str());
    in_bounds >> minx >> maxx >> miny >> maxy >> minz >> maxz;
    in_bounds.close();

    std::ifstream in(inputFile);    
    std::istream_iterator<Point3D> begin(in), end;
    dt.insert(begin, end);
    
    std::cout << "Succesfully opened " << inputFile << std::endl;
    std::cout << "  X range: " << minx << " to " << maxx << " ("<<maxx-minx<<" m)" << std::endl;
    std::cout << "  Y range: " << miny << " to " << maxy << " ("<<maxy-miny<<" m)" << std::endl;
    std::cout << "  Z range: " << minz << " to " << maxz << " ("<<maxz-minz<<" m)" << std::endl;
    std::cout << "  # Points: " << dt.number_of_vertices() << std::endl;
    std::cout << "  # Triangles: " << dt.number_of_faces() << std::endl;
}

CgalProcessor::CgalProcessor(CgalProcessor& otherCp){
    minx = otherCp.minx;
    maxx = otherCp.maxx;
    miny = otherCp.miny;
    maxy = otherCp.maxy;
    minz = otherCp.minz;
    maxz = otherCp.maxz;
    
    dt = otherCp.dt;
    
}

void CgalProcessor::printTags(){
    for( Face_iterator ib = dt.faces_begin();
        ib != dt.faces_end(); ++ib) {
        std::cout << "this one is: " << ib->info().tooBig << std::endl;
    }
    std::cout << "end" << std::endl;
}

void CgalProcessor::markBigTriangles(double maxEdgeLength){

    for (Dt::All_edges_iterator ei = dt.all_edges_begin(); ei != dt.all_edges_end(); ++ei) {
        Point3D p1 = ei->first->vertex(ei->first->cw(ei->second))->point();
        Point3D p2 = ei->first->vertex(ei->first->ccw(ei->second))->point();
        double edgeLength = CGAL::squared_distance(p1, p2);
//        std::cout << edgeLength << std::endl;
        if (edgeLength > maxEdgeLength){
            ei->first->info().tooBig = true;
            ei->first->neighbor(ei->second)->info().tooBig = true;
        }
    }
    
}

void CgalProcessor::saveContourShp(std::vector<double> isoDepths, const char * fileOut){
    
    contourSegmentVec contours = extractContoursCgalInt(isoDepths);
    
    const char *pszDriverName = "ESRI Shapefile";
    OGRSFDriver *poDriver;
    
    OGRRegisterAll();
    
    poDriver = OGRSFDriverRegistrar::GetRegistrar()->GetDriverByName( pszDriverName );
    if( poDriver == NULL ) {
        printf( "%s driver not available.\n", pszDriverName );
        exit( 1 );
    }
    
    OGRDataSource *poDS;
    
    poDS = poDriver->CreateDataSource( fileOut, NULL );
    if( poDS == NULL ) {
        printf( "Creation of output file failed.\n" );
        exit( 1 );
    }
    
    OGRLayer *poLayer;
    
    poLayer = poDS->CreateLayer( "contour", NULL, wkbLineString, NULL );
    if( poLayer == NULL ) {
        printf( "Layer creation failed.\n" );
        exit( 1 );
    }
    
    OGRFieldDefn oField( "depth", OFTReal );
    
    //    oField.SetWidth(32);
    
    if( poLayer->CreateField( &oField ) != OGRERR_NONE ) {
        printf( "Creating oftreal field failed.\n" );
        exit( 1 );
    }
    
    for (contourSegmentVec::iterator it = contours.begin(); it != contours.end(); ++it) {

        geos::operation::linemerge::LineMerger merger;
        geos::geom::PrecisionModel::PrecisionModel* precisionModel = new geos::geom::PrecisionModel(10000);
        geos::geom::GeometryFactory geomFactory(precisionModel);
        
        // Convert CGAL segments to GEOS geometry and add them to the linemerger
        for (std::vector<CGAL::Segment_3<K> >::iterator sit = it->second.begin(); sit != it->second.end(); ++sit) {
            
            geos::geom::CoordinateArraySequence* pseq = new geos::geom::CoordinateArraySequence();

            geos::geom::Coordinate source(sit->source().x(), sit->source().y(), sit->source().z());
            precisionModel->makePrecise(source);
            
            geos::geom::Coordinate target(sit->target().x(), sit->target().y(), sit->target().z());
            precisionModel->makePrecise(target);
            
            pseq->add(source);
            pseq->add(target);
            const geos::geom::Geometry* geos_segment = geomFactory.createLineString(pseq);
            merger.add(geos_segment);
        }

        // Get merged linestrings
        std::vector<geos::geom::LineString*> * mergedSegments = merger.getMergedLineStrings();

        // Orient properly, convert OGR geometry and save to shapefile layer
        for (std::vector<geos::geom::LineString*>::iterator sit = mergedSegments->begin(); sit != mergedSegments->end(); ++sit){
            geos::geom::LineString* geos_ls;
            OGRLineString *ls = new OGRLineString();
            
            OGRFeature *poFeature;
            poFeature = OGRFeature::CreateFeature( poLayer->GetLayerDefn() );
            poFeature->SetField( "depth", it->first );
            
            // fix orientation of contours to have the shallower region on the right side
            size_t index = (*sit)->getNumPoints()/2;
            geos::geom::Point* p1_ = (*sit)->getPointN(index);
            geos::geom::Point* p2_ = (*sit)->getPointN(index+1);
            PointDt p1(p1_->getX(), p1_->getY(), it->first);
            PointDt p2(p2_->getX(), p2_->getY(), it->first);
            PointDt p3;
            
            Dt::Face_handle triangle = dt.locate(p1);
            
            int triangle_index = -1;
            CGAL::Orientation orientation = CGAL::COLLINEAR;
            do {
                triangle_index++;
                p3 = triangle->vertex(triangle_index)->point();
                CGAL::Point_2<K> p1_2(p1.x(), p1.y());
                CGAL::Point_2<K> p2_2(p2.x(), p2.y());
                CGAL::Point_2<K> p3_2(p3.x(), p3.y());
                orientation = CGAL::orientation(p1_2,p2_2,p3_2);

            } while (orientation==CGAL::COLLINEAR && triangle_index<2);

            if (orientation==CGAL::COLLINEAR) {
                geos_ls = *sit;
                std::cerr << "!!! failed to orient a contour line" << std::endl;
            } else if ( !(((orientation == CGAL::NEGATIVE) and (p3.z() > it->first)) or 
                   ((orientation == CGAL::POSITIVE) and (p3.z() < it->first)) ) ) 
            {
                geos::geom::Geometry* g = (*sit)->reverse();
                geos_ls = dynamic_cast<geos::geom::LineString*>( g );
            } else {
                geos_ls = *sit;
            }
            
            // save to ogr
            std::vector<geos::geom::Coordinate> coordinates;
            geos_ls->getCoordinates()->toVector(coordinates);
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
        
    OGRDataSource::DestroyDataSource( poDS );    
}

// following method does not work properly for degenerate cases!
contourSegmentVec CgalProcessor::extractContours(std::vector<double> isoDepths) {
    
    contourSegmentVec segmentVec;
    

    for( Face_iterator ib = dt.faces_begin();
        ib != dt.faces_end(); ++ib) {
//        if(!ib->info().tooBig) {
            std::map<double, std::vector<PointDt> > intersectionVec;
            
            int a=0,b=0,c=0;
            for (int i=0; i<3; ++i) {
                Vertex_handle v1 = ib->vertex(i);
                PointDt p1 = v1->point();
                Vertex_handle v2 = ib->vertex(ib->cw(i));
                PointDt p2 = v2->point();
                
                for(std::vector<double>::iterator iD = isoDepths.begin(); iD != isoDepths.end(); ++iD) {

//                    if(p1.z() == *iD and p2.z() == *iD) {//intersection is exactly along an edge of this triangle face
//                        intersectionVec[*iD].push_back( p1 );
//                        intersectionVec[*iD].push_back( p2 );
//                    }
                    if(p1.z() != *iD and p2.z() != *iD) {
                        if((p1.z() < *iD and p2.z() > *iD) or ((p1.z() > *iD and p2.z() < *iD) and intersectionVec.size() < 2)) {
                                c++;
                                double lamda = (*iD - p1.z()) / (p2.z() - p1.z());
                                double x = (1-lamda) * p1.x() + lamda * p2.x();
                                double y = (1-lamda) * p1.y() + lamda * p2.y();
                                intersectionVec[*iD].push_back( PointDt(x,y,*iD) );
    //                            std::cout << "pushing " << *(intersectionVec[*iD].end()-1) << " " << *iD << std::endl;
                        } else if (p1.z() == *iD and intersectionVec.size() < 2 ) {
                            intersectionVec[*iD].push_back( p1 );
                            a++;
                        } else if (p2.z() == *iD and intersectionVec.size() < 2) {
                            intersectionVec[*iD].push_back( p2 );
                            b++;
                        }
                        
                    }
                }
            }
            
            for(std::vector<double>::iterator iD = isoDepths.begin(); iD != isoDepths.end(); ++iD) {
                if (intersectionVec[*iD].size() == 2){
                    segmentVec[*iD].push_back(CGAL::Segment_3<K>(intersectionVec[*iD][0], intersectionVec[*iD][1]));
//                    std::cout << CGAL::Segment_3<K>(intersectionVec[*iD][0], intersectionVec[*iD][1]) <<std::endl;
//                } else if (intersectionVec[*iD].size() == 6) {
//                    segmentVec[*iD].push_back(CGAL::Segment_3<K>(intersectionVec[*iD][0], intersectionVec[*iD][1]));
//                    segmentVec[*iD].push_back(CGAL::Segment_3<K>(intersectionVec[*iD][2], intersectionVec[*iD][3]));
//                    segmentVec[*iD].push_back(CGAL::Segment_3<K>(intersectionVec[*iD][4], intersectionVec[*iD][5]));
                } else if (intersectionVec[*iD].size()!=0) {
                    std::cout << "wth " << c << a << b << " " << intersectionVec[*iD].size() << " " << *iD << std::endl;
                    ib->vertex(0)->info().metricSafety = true;
                    ib->vertex(1)->info().metricSafety = true;
                    ib->vertex(2)->info().metricSafety = true;   
                }
            }
//        }
    }
    
    return segmentVec;
}

contourSegmentVec CgalProcessor::extractContoursCgalInt(std::vector<double> isoDepths) {
    
    contourSegmentVec segmentVec;
    
    for(std::vector<double>::iterator isoDepth = isoDepths.begin(); isoDepth != isoDepths.end(); ++isoDepth) {
        
        CGAL::Plane_3<K> plane(0,0,-1,*isoDepth);
        
        for( Face_iterator ib = dt.faces_begin();
            ib != dt.faces_end(); ++ib) {
            if(!ib->info().tooBig){
                CGAL::Object result = CGAL::intersection(plane, dt.triangle(ib));
                //std::cout << CGAL::do_intersect(plane, dt.triangle(ib));
                if (const CGAL::Segment_3<K> *isegment = CGAL::object_cast<CGAL::Segment_3<K> >(&result)) {
                    segmentVec[*isoDepth].push_back(*isegment);
                }
            }
        }
    }
    return segmentVec;
}

void CgalProcessor::clear(){
    dt.clear();
}

void CgalProcessor::dumpXYZ(const char * outFile){
    
    std::string outFile_bounds = outFile;
    outFile_bounds.append(".bounds");
    
    std::ofstream ofs_bounds(outFile_bounds.c_str());
    ofs_bounds << minx << std::endl << maxx << 
     std::endl << miny << std::endl << maxy << 
     std::endl << minz << std::endl << maxz << std::endl;
    
    ofs_bounds.close();
    
    std::ofstream ofs(outFile);
    
    
    for( Dt::Finite_vertices_iterator vit= dt.finite_vertices_begin(); vit != dt.finite_vertices_end() ; ++vit) {
        ofs <<std::setprecision(2)<<std::fixed;
        
        ofs << " " << (vit->point().cartesian(0));
        ofs << " " << (vit->point().cartesian(1));
        ofs << " " << (vit->point().cartesian(2));
        ofs << std::endl;
    }
    
    ofs.close();
}

void CgalProcessor::dumpOBJ(const char * outFile){
   
    std::ofstream ofs(outFile);
    
    std::map<Vertex_handle,int> V;

    int jnum = 0;
    double x_offset = (maxx-minx)/2 + minx;
    double y_offset = (maxy-miny)/2 + miny;
    double z_offset = (maxz-minz)/2 + minz;
    for( Dt::Finite_vertices_iterator vit= dt.finite_vertices_begin(); vit != dt.finite_vertices_end() ; ++vit) {
        ofs << "v"<<std::setprecision(6)<<std::showpoint;

        ofs << " " << (vit->point().cartesian(0) - x_offset) ;
        ofs << " " << (vit->point().cartesian(1) - y_offset) ;
        ofs << " " << 100*(vit->point().cartesian(2) - z_offset);

        ofs << std::endl;
        V[vit] = ++jnum;
    }
    
    // vertices of the faces
    
    for( Face_iterator ib = dt.faces_begin();
        ib != dt.faces_end(); ++ib) {
        ofs << "f";
        for(int i = 0; i < 3 ; i++) {
            ofs << " " << V[ib->vertex(i)];
        }
        ofs << std::endl;
    }
    
    ofs.close();
}

double CgalProcessor::MaxX(){
    return maxx;
}
double CgalProcessor::MinX(){
    return minx;
}
double CgalProcessor::MaxY(){
    return maxy;
}
double CgalProcessor::MinY(){
    return miny;
}
double CgalProcessor::MaxZ(){
    return maxz;
}
double CgalProcessor::MinZ(){
    return minz;
}
Dt& CgalProcessor::t(){
    Dt &dtr = dt;
    return dtr;
}

inline double CgalProcessor::LaplaceWeight(double tsx, double tsy, double ttx, double tty, double vsx, double vsy, double vtx, double vty){
    return sqrt( pow( ( vtx - vsx ),2) +
                 pow( ( vty - vsy ),2)    ) /
            sqrt( pow( ( ttx - tsx ),2) +
                 pow( ( tty - tsy ),2)    );
}

double CgalProcessor::estimateZ_LIN(Vertex_handle v) throw(OutsideConvexHullException)
{
    PointDt p1,p2,p3,q = v->point();
    
    Dt t2;
    Dt::Vertex_circulator vc = dt.incident_vertices(v), done(vc);
    do{
        if(!dt.is_infinite(vc))
            t2.insert(vc->point());
    } while(++vc!=done);
    
    // what should happen if this requested point is on the boudnary of the tin...

    Dt::Face_handle face = t2.locate(q);
    if (face==NULL) throw OutsideConvexHullException();
        
    p1 = face->vertex(0)->point();
    p2 = face->vertex(1)->point();
    p3 = face->vertex(2)->point();
    CGAL::Plane_3<K> plane(p1,p2,p3);
    
    return - plane.a()/plane.c() * q.x() - plane.b()/plane.c()*q.y() - plane.d()/plane.c();
}

double CgalProcessor::estimateZ_NN(Vertex_handle v) throw(OutsideConvexHullException)
{
    typedef std::vector< std::pair< Dt::Vertex_handle, Dt::Geom_traits::FT > > Point_coordinate_vector;
    
    Dt t2;
    Dt::Vertex_circulator vc = dt.incident_vertices(v),
    done(vc);
    do{
        if(!dt.is_infinite(vc))
            t2.insert(vc->point());
    } while(++vc!=done);
    
    Point_coordinate_vector coords;
    
    CGAL::Triple<
    std::back_insert_iterator<Point_coordinate_vector>,
    K::FT, bool> result =
        CGAL::natural_neighbor_coordinates_vertex_2(t2, v->point(), std::back_inserter(coords));
    
    double newZ = 0;

    if(result.third)
        for(Point_coordinate_vector::iterator it = coords.begin(); it != coords.end(); ++it)
            newZ += ( it->first->point().z() * (it->second/result.second) );

    return newZ;
}

double CgalProcessor::estimateZ_LP(Vertex_handle v) throw(OutsideConvexHullException)
{
    std::vector< double > rawWeights, zDepth;
    
    Dt::Edge_circulator iEdges = dt.incident_edges(v), done(iEdges);
    do {
        
        if( (!dt.is_infinite(iEdges->first)) &&  (!dt.is_infinite(iEdges->first->neighbor(iEdges->second))) )
        {
            PointDt vs = dt.circumcenter(iEdges->first);
            PointDt vt = dt.circumcenter(iEdges->first->neighbor(iEdges->second));
            PointDt ts = iEdges->first->vertex(Dt::ccw(iEdges->second))->point();
            PointDt tt = iEdges->first->vertex(Dt::cw(iEdges->second))->point();
            
            rawWeights.push_back(LaplaceWeight(ts.x(), ts.y(), tt.x(), tt.y(), vs.x(), vs.y(), vt.x(), vt.y()));
            zDepth.push_back(ts.z());
        }
        
    } while(++iEdges != done);
    
    double sum = std::accumulate(rawWeights.begin(), rawWeights.end(), 0.0);
    double newZ=0;

    for(size_t i=0; i<rawWeights.size(); ++i)
        newZ += (rawWeights[i]/sum)*zDepth[i];
    
    return newZ;
}

double CgalProcessor::estimateZ(smoothAlg algorithm, Vertex_handle v) throw(OutsideConvexHullException)
{
    try {
        if(algorithm == NN)
            return estimateZ_NN(v);
        else if(algorithm == LP)
            return estimateZ_LP(v);
        else if(algorithm == LIN)
            return estimateZ_LIN(v);
        else {
            std::cerr << "Undefined smoothing algorithm";
            exit(1);
        }
    }
    catch (OutsideConvexHullException& e) {
        throw e;
    }
}

void CgalProcessor::smooth(smoothAlg algorithm)
{
    typedef std::pair< Vertex_handle, double > vdPair;
    std::vector< vdPair > newDepths;
    
    for( Dt::Finite_vertices_iterator vit=dt.finite_vertices_begin() ; vit != dt.finite_vertices_end(); ++vit ) {
        
        double oldZ = vit->point().z();

        double newZ = estimateZ(algorithm, vit);
        
        if (oldZ < newZ){
            newDepths.push_back(std::make_pair(vit, newZ));
        }
    }
    
    for( std::vector<vdPair>::iterator it = newDepths.begin(); it != newDepths.end(); ++it ) {
        //dt.remove(it->first);
        it->first->set_point(PointDt(it->first->point().x(), it->first->point().y(),it->second ));
    }
    std::cout << "Smoothing success" << std::endl;
}

//void CgalProcessor::smoothDrop(smoothAlg algorithm, double treshold)
//{
//    typedef std::pair< Vertex_handle, double > vdPair;
//    std::vector< vdPair > newDepths;
//    
//    for( Dt::Finite_vertices_iterator vit=dt.finite_vertices_begin() ; vit != dt.finite_vertices_end(); ++vit ) {
//        
//        double oldZ = vit->point().z();
//        
//        double newZ = estimateZ(algorithm, vit);
//        
//        //if (oldZ < newZ){
//        if (newZ > oldZ)
//            if ((newZ-oldZ) < treshold)
//                newDepths.push_back(std::make_pair(vit, newZ));
//    }
//    
//    for( std::vector<vdPair>::iterator it = newDepths.begin(); it != newDepths.end(); ++it ) {
//        if(dropPoints)
//            dt.remove(it->first);
//        else
//            it->first->set_point(PointDt(it->first->point().x(), it->first->point().y(),it->second ));
//    }
//}

void CgalProcessor::densify(smoothAlg algorithm)
{
    typedef std::vector < std::pair< PointDt, Dt::Face_handle > > pfPair;
    pfPair newPoints;
    for( Dt::Finite_faces_iterator ib = dt.finite_faces_begin();
        ib != dt.finite_faces_end(); ++ib) {
        if (!ib->info().tooBig)
            newPoints.push_back(std::make_pair(dt.circumcenter(ib), ib) );
    }
                            
    for (pfPair::iterator p = newPoints.begin(); p != newPoints.end(); ++p){
        
        Vertex_handle v = dt.insert(p->first, p->second);
        
        double newZ = estimateZ(algorithm, v);
        
        v->set_point(PointDt(v->point().x(), v->point().y(), newZ ));
    }
}

double CgalProcessor::vertError(smoothAlg algorithm, Vertex_handle v) throw(OutsideConvexHullException)
{
    try {
        return v->point().z() - estimateZ(algorithm, v);
    } catch (OutsideConvexHullException& e) {
        throw e;
    }
    
}
    
double CgalProcessor::verticalError(PointDt q)
{
    PointDt p1,p2,p3;
    
    // what if no triangle "contains" this point?
    Dt::Face_handle face = dt.locate(q);
    
    p1 = face->vertex(0)->point();
    p2 = face->vertex(1)->point();
    p3 = face->vertex(2)->point();
    CGAL::Plane_3<K> plane(p1,p2,p3);
    
    double interpolatedZ = - plane.a()/plane.c() * q.x() - plane.b()/plane.c()*q.y() - plane.d()/plane.c();
    
    return q.z()-interpolatedZ; //std::fabs()
}

void CgalProcessor::updateCache(SimplificationCache &cache, smoothAlg algorithm, Vertex_handle v, bool upOnly)
{
    // check if point is not incident to infinite vertex
    try {
        double e = vertError(algorithm, v);
        if (upOnly){
            if (e < -1E-5){
                cache.verts[v] = std::fabs(e);
            } else if (cache.verts.find(v) == cache.verts.end()) {
                cache.verts.erase(v);
            }
        } else {
            cache.verts[v] = std::fabs(e);
        }
        
        if (cache.minError.second > e){
            cache.minError = std::make_pair(v,e);
        }
    } catch (OutsideConvexHullException& e) {
        std::cerr << "Simpflification: Skipping point on boundary";
    }
    
}

void CgalProcessor::simplify(smoothAlg algorithm, double treshold, bool upOnly, bool dumpIntermediateStepsToOBJ)
{
    SimplificationCache cache;
    
    Vertex_handle vh = dt.finite_vertices_begin();
    cache.minError = std::make_pair(vh,vertError(algorithm, vh));
    
    for( Dt::Finite_vertices_iterator vit=dt.finite_vertices_begin() ; vit != dt.finite_vertices_end(); ++vit ) {
        updateCache(cache, algorithm, vit, upOnly);
    }
    
    int numberOfDroppedPoints = 0;
    while(cache.minError.second < treshold) {
        vdMap::iterator currentPosIt = cache.verts.begin();
        cache.minError = *currentPosIt;
        
        for (vdMap::iterator it = cache.verts.begin(); it != cache.verts.end(); ++it) {
            if (cache.minError.second > it->second) {
                cache.minError = *it;
                currentPosIt = it;
            }
        }
        std::cout << cache.minError.first->point() << " | " << cache.minError.second << "\n";        

        Dt::Vertex_circulator iVertex = dt.incident_vertices(cache.minError.first), done(iVertex);
                
        std::vector<Vertex_handle> neighbours;
        do{
            if(!dt.is_infinite(iVertex)) 
                neighbours.push_back(iVertex);
        } while(++iVertex != done);
        
        dt.remove(cache.minError.first);
        cache.verts.erase(currentPosIt);
        numberOfDroppedPoints++;
        
        for(std::vector<Vertex_handle>::iterator iVertex = neighbours.begin(); iVertex != neighbours.end(); ++iVertex){
            updateCache(cache, algorithm, *iVertex, upOnly);
        }
        if(dumpIntermediateStepsToOBJ) {
            std::stringstream outPath;
            outPath << "/Volumes/macData/mScpp/testdata/intermediate_output/dump_" << numberOfDroppedPoints << ".obj";
            dumpOBJ(outPath.str().c_str());
        }
    }
    std::cout << "# points dropped for simplify: " << numberOfDroppedPoints << std::endl;
    
}

void CgalProcessor::metricL2(CgalProcessor &otherSurface)
{
    Dt & otherDt = otherSurface.t();
    AABB_Tree TriTree;

    std::list<Triangle> triangleList;
    for( Dt::Finite_faces_iterator ib = otherDt.finite_faces_begin();
        ib != otherDt.finite_faces_end(); ++ib) {
        triangleList.push_back(otherDt.triangle(ib));
    }
    
    TriTree.insert(triangleList.begin(), triangleList.end());
    if (TriTree.accelerate_distance_queries()) std::cout << "build AABB_tree succes\n"; else std::cout << "build AABB_tree fail\n";
    
    double min=1, max=0;
    for( Dt::Finite_vertices_iterator vit=dt.finite_vertices_begin() ; vit != dt.finite_vertices_end(); ++vit ) {
        double d = TriTree.squared_distance(vit->point());
        vit->info().metricL2 = d;
        if (min>d) min = d;
        if (max<d) max = d;
        std::cout << "point-distance: " << d  << std::endl;
    }
    maxL2error = max;
    minL2error = min;
    std::cout << "metricL2: max="<<max<<", min="<<min<<std::endl;
}

void CgalProcessor::metricSafety(CgalProcessor &otherSurface)
{
    double NofUnsafePoints=0;
    for( Dt::Finite_vertices_iterator vit=dt.finite_vertices_begin() ; vit != dt.finite_vertices_end(); ++vit ) {
        double d = otherSurface.verticalError(vit->point());
        
        bool notSafe = d>1E-5;
        vit->info().metricSafety = notSafe;
        if (notSafe) {
            std::cout << "Vertical distance: " << d << " : is marked unsafe!" << std::endl;

            NofUnsafePoints++;
        } 

    }
    std::cout << "# unsafe points: " << NofUnsafePoints << std::endl;
}