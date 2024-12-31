#include "../include/ear_clipping.h"

namespace ear_clipping {
    Point_2 EarClipping::getNextPointInPolygon(const std::list<Point_2>& poly_vertices, std::list<Point_2>::const_iterator vertex_iterator) const {
        auto it_next = std::next(vertex_iterator);
        return (poly_vertices.end() == it_next) ? *poly_vertices.begin() : *it_next;
    }

    Point_2 EarClipping::getPreviousPointInPolygon(const std::list<Point_2>& poly_vertices, std::list<Point_2>::const_iterator vertex_iterator) const {
        return (poly_vertices.begin() == vertex_iterator) ? *poly_vertices.rbegin() : *std::prev(vertex_iterator);
    }

    std::vector<Triangle_2> EarClipping::triangulateSimplePolygon(const Polygon_2& simple_polygon) {
        if (!simple_polygon.is_simple()) {
            throw(std::invalid_argument("Input polygon has to be a simple polygon"));
        }

        std::list<Point_2> poly_vertices(simple_polygon.vertices_begin(), simple_polygon.vertices_end());
        std::vector<Triangle_2> triangles;
        
        int max_steps = poly_vertices.size() - 3;
        
        // At every iteration of the loop below we expect at least one ear to be detected.
        // An upper bound for the loop is max_steps, prefer to use this for loop
        // since numerical issues can cause isEar to fail and create infinte loops.
        for (int step = 0; step < max_steps; step++) {
            for (auto it = poly_vertices.begin(); it != poly_vertices.end();) {
                if (isEar(poly_vertices, it, simple_polygon.orientation())) {
                    // Create the triangle and add to the result
                    Triangle_2 ear_triangle(getPreviousPointInPolygon(poly_vertices, it), *it, getNextPointInPolygon(poly_vertices, it));
                    triangles.push_back(ear_triangle);
                    it = poly_vertices.erase(it);
                    if (3 == poly_vertices.size()) break;
                }
                else {
                    ++it;
                }
            }
            if (3 == poly_vertices.size()) break;
        }

        if (3 != poly_vertices.size()) {
            triangles.clear();
            throw(std::runtime_error("Failed to properly triangulate due to numerical issues in Ear detection."));
        }

        // Add the last triangle
        auto it = poly_vertices.begin();
        triangles.push_back(Triangle_2(*it, *std::next(it), *std::next(it, 2)));
        return triangles;        
    }

    bool EarClipping::isEar(const std::list<Point_2>& poly_vertices, std::list<Point_2>::const_iterator vertex_iterator, CGAL::Orientation polygon_orientation) const {
        auto prev_vertex = getPreviousPointInPolygon(poly_vertices, vertex_iterator);
        auto curr_vertex = *vertex_iterator;
        auto next_vertex = getNextPointInPolygon(poly_vertices, vertex_iterator);
        
        if (!isConvex(prev_vertex, curr_vertex, next_vertex, polygon_orientation)) return false;
        
        // Check that no other vertices are inside the triangle
        Triangle_2 triangle(prev_vertex, curr_vertex, next_vertex);
        
        // Check for points in the list that are after next_vertex 
        auto it_adv_1 = std::next(vertex_iterator);
        if (poly_vertices.end() != it_adv_1) {
            auto it_adv_2 = std::next(it_adv_1);
            if (poly_vertices.end() != it_adv_2) {
                for (auto it = it_adv_2; it != poly_vertices.end(); it++) {
                    if (isPointInTriangle(*it, triangle)) {
                        return false;
                    }
                }
            }
        }

        // Check for points that are before previous to previous current vertex
        if (poly_vertices.begin() != vertex_iterator){
            auto it_ret_1 = std::prev(vertex_iterator);
            if (poly_vertices.begin() != it_ret_1) {
                for (auto it = poly_vertices.begin(); it != it_ret_1; it++) {
                    if (isPointInTriangle(*it, triangle)) {
                        return false;
                    }
                }
            }
        }
        
        return true;
    }

    // Check if a point is inside a given triangle
    bool EarClipping::isPointInTriangle(const Point_2& point, const Triangle_2& triangle) const {
        return triangle.has_on_bounded_side(point);
    }

    // Checks whether the vertex curr is locally convex. 
    bool EarClipping::isConvex(const Point_2& prev, const Point_2& curr, const Point_2& next, CGAL::Orientation polygon_orientation) const {
        Vector_2 v1 = next - curr;
        Vector_2 v2 = curr - prev;

        if (CGAL::COUNTERCLOCKWISE == polygon_orientation) {
            return (v1.x() * v2.y()) - (v1.y() * v2.x()) < 0;
        }
        else {
            return (v1.x() * v2.y()) - (v1.y() * v2.x()) > 0;
        }
    }
}