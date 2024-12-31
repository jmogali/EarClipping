#ifndef EAR_CLIPPING_H_
#define EAR_CLIPPING_H_

// Copyright 2024 <...>

#include <CGAL/Simple_cartesian.h>
#include <CGAL/Polygon_2.h>
#include <CGAL/Triangle_2.h>
#include <CGAL/Vector_2.h>
#include <list>
#include <vector>
namespace ear_clipping {
typedef CGAL::Simple_cartesian<double> Kernel;
typedef Kernel::Point_2 Point_2;
typedef CGAL::Polygon_2<Kernel> Polygon_2;
typedef CGAL::Triangle_2<Kernel> Triangle_2;
typedef Kernel::Vector_2 Vector_2;

class EarClipping {
 public:
  std::vector<Triangle_2> triangulate(const Polygon_2& simple_poly) const;

 protected:
  bool isEar(const std::list<Point_2>& poly_vertices,
             std::list<Point_2>::const_iterator vertex_iter,
             CGAL::Orientation poly_orientation) const;
  bool isConvex(const Point_2& prev, const Point_2& curr, const Point_2& next,
                CGAL::Orientation poly_orientation) const;
  bool isPointInTriangle(const Point_2& point,
                         const Triangle_2& triangle) const;
  Point_2 getNextPointInPolygon(const std::list<Point_2>& poly_vertices,
                    std::list<Point_2>::const_iterator vertex_iter) const;
  Point_2 getPreviousPointInPolygon(const std::list<Point_2>& poly_vertices,
                    std::list<Point_2>::const_iterator vertex_iter) const;
};
}  // namespace ear_clipping

#endif  // EAR_CLIPPING_H_
