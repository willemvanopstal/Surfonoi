//
//  CgalType.h
//  PhDVis
//
//  Created by Ravi on 23/3/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef PhDVis_CgalType_h
#define PhDVis_CgalType_h

/* stupid OS X macro names */
#ifdef check
#undef check
#endif
#ifdef nil
#undef nil
#endif
#ifdef Nil
#undef Nil
#endif

#include <cgal/Plane_3.h>
#include <cgal/intersections.h>

#include <cgal/Exact_predicates_inexact_constructions_kernel.h>
#include <cgal/Triangulation_vertex_base_with_info_2.h>
#include <cgal/Triangulation_face_base_with_info_2.h>
#include <cgal/Projection_traits_xy_3.h>
#include <cgal/Delaunay_triangulation_2.h>

#include <cgal/Triangulation_hierarchy_2.h>

//#include <CGAL/Voronoi_diagram_2.h>
//#include <CGAL/Delaunay_triangulation_adaptation_traits_2.h>
//#include <CGAL/Delaunay_triangulation_adaptation_policies_2.h>

#include <cgal/natural_neighbor_coordinates_2.h>

//#include <CGAL/Simple_cartesian.h>
#include <cgal/AABB_tree.h>
#include <cgal/AABB_traits.h>
#include <cgal/AABB_triangle_primitive.h>
#include <list>

struct vertexInfo {
    double metricL2;
    bool metricSafety;
    bool tooBig;
    vertexInfo():metricL2(0),metricSafety(0),tooBig(0){}
};

struct faceInfo {
    bool tooBig;
    faceInfo():tooBig(0){}
};

//typedef CGAL::Simple_cartesian<double> K;
typedef CGAL::Exact_predicates_inexact_constructions_kernel K;

typedef K::Triangle_3 Triangle;

typedef std::list<Triangle>::iterator AABB_Iterator;
typedef CGAL::AABB_triangle_primitive<K,AABB_Iterator> AABB_Primitive;
typedef CGAL::AABB_traits<K, AABB_Primitive> AABB_triangle_traits;
typedef CGAL::AABB_tree<AABB_triangle_traits> AABB_Tree;

typedef CGAL::Projection_traits_xy_3<K>                     Gt;

typedef CGAL::Triangulation_vertex_base_with_info_2<vertexInfo,Gt>       Vbb;
typedef CGAL::Triangulation_hierarchy_vertex_base_2<Vbb>    Vb;
typedef CGAL::Triangulation_face_base_with_info_2<faceInfo,Gt>                 Fb;
typedef CGAL::Triangulation_data_structure_2<Vb,Fb>         Tds;

typedef CGAL::Delaunay_triangulation_2<Gt,Tds>              Dtt;
typedef CGAL::Triangulation_hierarchy_2<Dtt>                Dt;

//typedef CGAL::Delaunay_triangulation_adaptation_traits_2<Dt>                 AT;
//typedef CGAL::Delaunay_triangulation_caching_degeneracy_removal_policy_2<Dt> AP;
//typedef CGAL::Voronoi_diagram_2<Dt,AT,AP>                   Vd;

typedef Dt::Vertex_handle                                   Vertex_handle;
typedef Dt::Vertex_iterator                                 Vertex_iterator;
//typedef Vd::Vertex_handle                                   Vertex_handle_vd;
//typedef Vd::Vertex_iterator                                 Vertex_iterator_vd;
typedef Dt::Face_iterator                                   Face_iterator;

typedef Dt::Point                                           PointDt;
typedef K::Point_3                                          Point3D;
typedef K::Point_2                                          Point2D;
typedef CGAL::Segment_3<K>                                  Segment3D;

#endif
