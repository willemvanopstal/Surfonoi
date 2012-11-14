#ifndef PhDVis_CgalProcessor_h
#define PhDVis_CgalProcessor_h

//#define REAL double
//#define ANSI_DECLARATORS
//#define VOID void

#include "CgalType.h"

//OGR
#include <gdal/ogrsf_frmts.h>
//GDAL
#include <gdal/gdal_priv.h>
#include <gdal/cpl_string.h>
#include <gdal/cpl_conv.h>
#include <gdal/ogr_spatialref.h>
//GEOS
#include <geos/geom/Coordinate.h>
#include <geos/geom/Point.h>
#include <geos/geom/LineString.h>
#include <geos/geom/PrecisionModel.h>
#include <geos/geom/GeometryFactory.h>
#include <geos/geom/CoordinateArraySequence.h>
#include <geos/operation/linemerge/LineMerger.h>

#include <map>
#include <vector>
#include <set>
#include <numeric>
#include <fstream>
#include <sstream>
#include <string>

enum smoothAlg {LP, NN, LIN};
typedef std::vector< CGAL::Segment_3<K> > segmentVec;
typedef std::map<double, segmentVec > contourSegmentVec;

// Simplfication
typedef std::map<Vertex_handle, double> vdMap; // a vertex and an error
typedef std::pair<Vertex_handle, double> vdPair;

struct SimplificationCache {
    vdMap verts;
    vdPair minError;
};

class OutsideConvexHullException: public std::exception
{
public:
    virtual const char* what() const throw()
    {
        return "Vertex lays outside triangulation convex hull";
    }
};

class CgalProcessor {
private:
    Dt dt;
    double minx, maxx, miny, maxy, minz, maxz;
    double minL2error, maxL2error;
//    const char* inputFile;
    
    inline double LaplaceWeight(double tsx, double tsy, double ttx, double tty, double vsx, double vsy, double vtx, double vty);

    //contouring
    inline int cntrEvalVertex(Vertex_handle v, double depth);
    inline PointDt cntrIntersectEdge(Vertex_handle v1, Vertex_handle v2, double depth);
    void extractContour(contourSegmentVec& segmentVec, double isoDepth);
    
    double estimateZ(smoothAlg, PointDt) throw(OutsideConvexHullException);
    double estimateZ_LP(Vertex_handle) throw(OutsideConvexHullException);
    double estimateZ_NN(Vertex_handle) throw(OutsideConvexHullException);
    double estimateZ_LIN(Vertex_handle) throw(OutsideConvexHullException);
    double estimateZ(smoothAlg, Vertex_handle) throw(OutsideConvexHullException);
    double vertError(smoothAlg, Vertex_handle) throw(OutsideConvexHullException);
    double verticalError(PointDt q);
    double verticalError(Vertex_handle);
    void updateCache(SimplificationCache &cache, smoothAlg algorithm, Vertex_handle v, bool upOnly);
    
public:
    CgalProcessor(const char *inputFile);
    CgalProcessor(CgalProcessor &);
    // Load data
//    void load(const char* inputFile);
    void clear();

    // export data
    void toRaster(const char * outFile, double cellSize, smoothAlg alg);
    void toRaster(const char * outFile, double cellSize, smoothAlg alg, double xmin, double xmax, double ymin, double ymax);
    void dumpOBJ(const char * outfile, double zExageration=1);
    void dumpXYZ(const char * outFile);
    void dumpDiffs(CgalProcessor &otherSurface, const char * outFile);
    contourSegmentVec extractContours(std::vector<double> isoDepths);
    contourSegmentVec extractContoursCgalInt(std::vector<double> isoDepths);
    void saveContourShp(std::vector<double> isoDepths, const char * fileOut);

    double MaxX();
    double MinX();
    double MaxY();
    double MinY();
    double MaxZ();
    double MinZ();
    Dt& t();
    
    void markBigTriangles_mel(double maxEdgeLength);
    void markBigTriangles_mta(double maxDoubleArea);
    void printTags();

    void smooth(smoothAlg, bool upOnly=true);
    void smoothRegion(smoothAlg, bool upOnly=true);
    void densify(smoothAlg, bool tooBigOnly=false);
    void simplify(smoothAlg, double treshold, bool upOnly=true, bool dumpIntermediateStepsToOBJ=false);
        
    void metricL2(CgalProcessor &otherSurface);
    void metricSafety(CgalProcessor &processedSurface);
    
    friend class visVoronoi;
    friend class visContour;
    friend class visTriangulation;
    
//    const Dt& getDt();
//    Vd getVd();
};

#endif
