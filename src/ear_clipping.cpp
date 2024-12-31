#include "../include/ear_clipping.h"

// Copyright 2024 <...>

namespace ear_clipping {

Point_2 EarClipping::getNextPointInPolygon(
    const std::list<Point_2>& poly_vertices,
    std::list<Point_2>::const_iterator vertex_iter) const {
  auto iter_next = std::next(vertex_iter);
  return (iter_next == poly_vertices.end()) ? *poly_vertices.begin() : *iter_next;
}

Point_2 EarClipping::getPreviousPointInPolygon(
    const std::list<Point_2>& poly_vertices,
    std::list<Point_2>::const_iterator vertex_iter) const {
  return (vertex_iter == poly_vertices.begin())
             ? *poly_vertices.rbegin()
             : *std::prev(vertex_iter);
}

std::vector<Triangle_2> EarClipping::triangulate(
    const Polygon_2& simple_poly) const {
  if (!simple_poly.is_simple()) {
    throw(std::invalid_argument("Input polygon has to be a simple polygon"));
  }

  std::list<Point_2> poly_vertices(
      simple_poly.vertices_begin(), simple_poly.vertices_end());
  std::vector<Triangle_2> triangles;

  int maxSteps = poly_vertices.size() - 3;

  // At every iteration of the loop below we expect at least one ear to be
  // detected. An upper bound for the loop is max_steps, prefer to use a for
  // loop in place of while loop since numerical issues can cause isEar()
  // to fail and lead to infinite looping.
  for (int step = 0; step < maxSteps; ++step) {
    for (auto iter = poly_vertices.begin(); iter != poly_vertices.end();) {
      if (isEar(poly_vertices, iter, simple_poly.orientation())) {
        // Create the triangle and add it to the result
        Triangle_2 ear_triangle(
            getPreviousPointInPolygon(poly_vertices, iter), *iter,
            getNextPointInPolygon(poly_vertices, iter));
        triangles.push_back(ear_triangle);
        iter = poly_vertices.erase(iter);
        if (poly_vertices.size() == 3) break;
      } else {
        ++iter;
      }
    }
    if (poly_vertices.size() == 3) break;
  }

  if (poly_vertices.size() != 3) {
    triangles.clear();
    throw(std::runtime_error(
        "Failed to properly triangulate due to numerical issues in Ear "
        "detection."));
  }

  // Add the last triangle
  auto iter = poly_vertices.begin();
  triangles.push_back(Triangle_2(*iter, *std::next(iter), *std::next(iter, 2)));
  return triangles;
}

bool EarClipping::isEar(
    const std::list<Point_2>& poly_vertices,
    std::list<Point_2>::const_iterator vertex_iter,
    CGAL::Orientation poly_orientation) const {
  auto prev_vertex = getPreviousPointInPolygon(poly_vertices, vertex_iter);
  auto curr_vertex = *vertex_iter;
  auto next_vertex = getNextPointInPolygon(poly_vertices, vertex_iter);

  if (!isConvex(prev_vertex, curr_vertex, next_vertex, poly_orientation)) {
    return false;
  }

  // Check that no other vertices are inside the triangle
  Triangle_2 triangle(prev_vertex, curr_vertex, next_vertex);

  // Check for points in the list that are after next_vertex
  auto iter_adv1 = std::next(vertex_iter);
  if (poly_vertices.end() != iter_adv1) {
    auto iter_adv2 = std::next(iter_adv1);
    if (poly_vertices.end() != iter_adv2) {
      for (auto iter = iter_adv2; iter != poly_vertices.end(); iter++) {
        if (isPointInTriangle(*iter, triangle)) {
          return false;
        }
      }
    }
  }

  // Check for points that are before previous to previous current vertex
  if (poly_vertices.begin() != vertex_iter) {
    auto iter_ret1 = std::prev(vertex_iter);
    if (poly_vertices.begin() != iter_ret1) {
      for (auto iter = poly_vertices.begin(); iter != iter_ret1; iter++) {
        if (isPointInTriangle(*iter, triangle)) {
          return false;
        }
      }
    }
  }

  return true;
}

// Check if a point is inside a given triangle
bool EarClipping::isPointInTriangle(
    const Point_2& point, const Triangle_2& triangle) const {
  return triangle.has_on_bounded_side(point);
}

// Checks whether the vertex curr is locally convex.
bool EarClipping::isConvex(
    const Point_2& prev, const Point_2& curr, const Point_2& next,
    CGAL::Orientation poly_orientation) const {
  Vector_2 v1 = next - curr;
  Vector_2 v2 = curr - prev;

  if (CGAL::COUNTERCLOCKWISE == poly_orientation) {
    return (v1.x() * v2.y()) - (v1.y() * v2.x()) < 0;
  } else {
    return (v1.x() * v2.y()) - (v1.y() * v2.x()) > 0;
  }
}

}  // namespace ear_clipping
