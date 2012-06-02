#ifndef PhDVis_CgalProcessor_h
#define PhDVis_CgalProcessor_h

//#define REAL double
//#define ANSI_DECLARATORS
//#define VOID void

#include "CgalType.h"

//OGR
#include <GDAL/ogrsf_frmts.h>
//GEOS
#include <GEOS/GEOS.h>
#include <GEOS/operation/linemerge/LineMerger.h>

#include <map>
#include <vector>
#include <numeric>
#include <fstream>
#include <sstream>
#include <string>

enum smoothAlg {LP, NN, LIN};
typedef std::map<double, std::vector<CGAL::Segment_3<K> > > contourSegmentVec;

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
    void dumpOBJ(const char * outfile);
    void dumpXYZ(const char * outFile);
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
    
    void markBigTriangles(double maxEdgeLength);
    void printTags();
    
    void smooth(smoothAlg);
    void densify(smoothAlg);
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
