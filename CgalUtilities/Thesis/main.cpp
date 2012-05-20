
#include "CgalProcessor.h"
#include <ogrsf_frmts.h>

using namespace std;


// file for testing and debugging

int main( )
{
    CgalProcessor cp("/Volumes/macData/mScpp/testdata/sloehaven_50mgrid.xyz");
//    CgalProcessor cp2(cp);
    
//    cp2.simplify(1, true, true);
    
//    cp.metricL2(cp2);
//    cp.metricSafety(cp2);
    
//    cp.printTags();
//    cp.markBigTriangles(100*100);
//    cp.printTags();
    
    std::vector<double> isoDepths;
    isoDepths.push_back(0);
    isoDepths.push_back(-2);
    isoDepths.push_back(-5);
    isoDepths.push_back(-10);
    isoDepths.push_back(-15);
    cp.saveContourShp(isoDepths, "/Volumes/macData/mScpp/testdata/validate_sloehaven/50m_.shp");
//    cp.simplify(LIN, 0.25);
//    cp.saveContourShp(isoDepths, "/Volumes/macData/mScpp/testdata/validate_london/10m_simpLIN025.shp");
    cp.smooth(NN);
    cp.smooth(NN);
    cp.saveContourShp(isoDepths, "/Volumes/macData/mScpp/testdata/validate_sloehaven/50m_smth2x.shp");
    cp.densify(NN);
    cp.saveContourShp(isoDepths, "/Volumes/macData/mScpp/testdata/validate_sloehaven/50m_smth2x_dens1x.shp");
    
    return 0;
}

//    ifstream in("/Volumes/macData/dataset/atlis/Antillen/HY1027_lm2-1972.xyz");
//    istream_iterator<Point> begin(in);
//    istream_iterator<Point> end;
//    
//    Dt dt;
    //Tds &tds();
    //tds = dt.tds();
//    dt.insert(begin, end);
//    for (Dt::Finite_faces_iterator fi = dt.finite_faces_begin (); fi != dt.finite_faces_end(); ++fi)
//    {
//        cout << fi->vertex(0)->point().x() << " ";
//        cout << fi->vertex(0)->point().y() << " ";
//        cout << fi->vertex(0)->point().z() << endl;
//    }
//    
//    for (Dt::Finite_edges_iterator ei = dt.finite_edges_begin (); ei != dt.finite_edges_end(); ++ei)
//    {
//        cout << dt.segment(*ei) << endl;
//    }
    
//    cout << dt << endl;
    
//    CgalProcessor cp;
//    cp.load("/Volumes/macData/mScpp/testdata/sloehaven_200mgrid.xyz");
//
//    cout << cp.MinX() << " " << cp.MaxX() << endl;
    
//    cp.create1DVisualisation_Contour(-10.0);
    
    
//    CgalProcessor::DisplayEdges &dv = cp.create1DVisualisation_Voronoi() ;    
//    std::cout << "nVertices: " << dv.nVertices <<std::endl;
//    for(int i=0; i<dv.nVertices; i+=1)
//    {
//        cout << i << " " << dv.vertices[i] << endl;
//    }
    
//    CgalProcessor::DisplayEdges &de = cp.create1DVisualisation_Contour(-10.0);    
//    std::cout << "nEdges: " << de.nEdges << " " << de.nVertices << std::endl;
//    for(int i=0; i<de.nEdges*2; i+=2)
//    {
//        cout << "Edge #: " << i/2 << endl << de.indices[i] << " " << de.vertices[de.indices[i]*3] << " " << de.vertices[de.indices[i]*3+1] << " " << de.vertices[de.indices[i]*3+2] <<
//        endl << de.indices[i+1] << " " << de.vertices[de.indices[i+1]*3] << " " << de.vertices[de.indices[i+1]*3+1] << " " << de.vertices[de.indices[i+1]*3+2] << endl;
//    }


//    CgalProcessor::DisplayFaces &df = cp.create2DVisualisation() ;
//    std::cout << "nFaces: " << df.nFaces <<std::endl;  
//    for(int i=0; i<df.nFaces*3; i+=3)
//    {
//        cout << i << " " << df.indices[i] << endl << df.indices[i+1] << endl << df.indices[i+2] << endl;
//    }
    
//    std::ifstream ifs("");

