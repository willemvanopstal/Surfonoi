#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Delaunay_triangulation_3.h>
#include <CGAL/Alpha_shape_3.h>

// for stream output of alpha_shape_3
#include <CGAL/Unique_hash_map.h>
#include <CGAL/Triangulation_utils_3.h>

// envelope
#include <CGAL/Env_triangle_traits_3.h>
#include <CGAL/Env_surface_data_traits_3.h>
#include <CGAL/envelope_3.h>

#include <fstream>
#include <list>
#include <cassert>

#define CGAL_CFG_ARRAY_MEMBER_INITIALIZATION_BUG

typedef CGAL::Exact_predicates_inexact_constructions_kernel Gt;

typedef CGAL::Alpha_shape_vertex_base_3<Gt>          Vb;
typedef CGAL::Alpha_shape_cell_base_3<Gt>            Fb;
typedef CGAL::Triangulation_data_structure_3<Vb,Fb>  Tds;
typedef CGAL::Delaunay_triangulation_3<Gt,Tds>       Triangulation_3;
typedef CGAL::Alpha_shape_3<Triangulation_3>         Alpha_shape_3;

//typedef CGAL::Gmpq                                       Number_type;
//typedef CGAL::Cartesian<Number_type>                     Kernel;
typedef CGAL::Env_triangle_traits_3<Gt>              Traits_3;
//typedef Kernel::Point_3                                  Point_3;
typedef Traits_3::Surface_3                              Triangle_3;
//typedef CGAL::Env_surface_data_traits_3<Traits_3, char>  Data_traits_3;
//typedef Data_traits_3::Surface_3                         Data_triangle_3;
typedef CGAL::Envelope_diagram_2<Traits_3>          Envelope_diagram_2;

typedef Gt::Point_3                                  Point;
typedef Alpha_shape_3::Alpha_iterator               Alpha_iterator;

int main()
{
    std::list<Point> lp;
    
    //read input
    std::ifstream is("/Volumes/macData/mScpp/testdata/sloehaven_50mgrid.xyz");

    double minx, maxx, miny, maxy, minz, maxz;
    is >> minx >> maxx >> miny >> maxy >> minz >> maxz;
    double x_offset = (maxx-minx)/2 + minx;
    double y_offset = (maxy-miny)/2 + miny;
    double z_offset = (maxz-minz)/2 + minz;
    
    std::istream_iterator<Point> begin(is), end;
    
    // compute alpha shape
    Alpha_shape_3 as(begin,end,5000,Alpha_shape_3::GENERAL);
    std::cout << "Alpha shape computed in REGULARIZED mode by default"
    << std::endl;
    
    
    
    //std::cout << as;
    // find optimal alpha value
//    Alpha_iterator opt = as.find_optimal_alpha(1);
//    std::cout << "Optimal alpha value to get one connected component is "
//    <<  *opt    << std::endl;
//    as.set_alpha(*opt);
//    assert(as.number_of_solid_components() == 1);
    std::ofstream os("/Volumes/macData/mScpp/testdata/alphashape/sloehaven50mAlpha5K_general.obj");
    std::list<Triangle_3>   triangles;
    
    typedef  Alpha_shape_3::size_type             size_type;
    typedef  Alpha_shape_3::Vertex_handle         Vertex_handle;
    typedef  Alpha_shape_3::Cell_handle           Cell_handle;
    typedef  Alpha_shape_3::Alpha_shape_vertices_iterator 
    Alpha_shape_vertices_iterator;
    typedef  Alpha_shape_3::Alpha_shape_facets_iterator
    Alpha_shape_facets_iterator;
    
    CGAL::Unique_hash_map< Vertex_handle, size_type > V;
    size_type number_of_vertices = 0;
    
    Alpha_shape_vertices_iterator vit;
    for( vit = as.alpha_shape_vertices_begin();
        vit != as.alpha_shape_vertices_end();
        ++vit) {
        V[*vit] = ++number_of_vertices;
        os << "v " << (*vit)->point().x()-x_offset << " " << (*vit)->point().y()-y_offset << " " << (*vit)->point().z()-z_offset << std::endl;
    }
    
    Cell_handle c;
    int i;
    Alpha_shape_facets_iterator fit;
    for( fit = as.alpha_shape_facets_begin();
        fit != as.alpha_shape_facets_end();
        ++fit) {
        c = fit->first;
        i = fit->second;
        // the following ensures that regular facets are output
        // in ccw order
        if (as.classify(*fit) == Alpha_shape_3::REGULAR && (as.classify(c) == Alpha_shape_3::INTERIOR)){
            c = c->neighbor(i);
            i = c->index(fit->first);
        }
        
        int i0 = CGAL::Triangulation_utils_3::vertex_triple_index(i,0);
        int i1 = CGAL::Triangulation_utils_3::vertex_triple_index(i,1);
        int i2 = CGAL::Triangulation_utils_3::vertex_triple_index(i,2);
        os << "f " << V[c->vertex(i0)] << ' ' 
        << V[c->vertex(i1)] << ' ' 
        << V[c->vertex(i2)] << std::endl;
        
        triangles.push_back ( Triangle_3 (c->vertex(i0)->point(), c->vertex(i1)->point(), c->vertex(i2)->point()) );
    }
    os.close();
    
    
    Envelope_diagram_2      min_diag;
    
    CGAL::upper_envelope_3 (triangles.begin(), triangles.end(),
                            min_diag);
    
    
    Envelope_diagram_2::Face_const_iterator            fcit;
    Envelope_diagram_2::Ccb_halfedge_const_circulator  ccb;
    Envelope_diagram_2::Surface_const_iterator         sit;
    
    for (fcit = min_diag.faces_begin(); fcit != min_diag.faces_end(); ++fcit)
    {
        // Print the face boundary.
        if (fcit->is_unbounded())
        {
            std::cout << "[Unbounded face]";
        }
        else
        {
            // Print the vertices along the outer boundary of the face.
            ccb = fcit->outer_ccb();
            std::cout << "[Face]  ";
            do
            {
                std::cout << '(' << ccb->target()->point() << ")  ";
                ++ccb;
            } while (ccb != fcit->outer_ccb());
        }
    }
    
    return 0;
}