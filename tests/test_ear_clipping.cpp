#include <gtest/gtest.h>
#include <utility>
#include <list>
#include <vector>
#include "../include/ear_clipping.h"

using namespace ear_clipping;

class TestingWrapper : public EarClipping {
 public:
  bool isEarTest(
      const std::list<Point_2>& poly_vertices,
      std::list<Point_2>::const_iterator vertex_iter,
      CGAL::Orientation poly_orientation) {
    return EarClipping::isEar(poly_vertices, vertex_iter, poly_orientation);
  }

  std::vector<Triangle_2> triangulateTest(const Polygon_2& simple_poly) {
    return EarClipping::triangulate(simple_poly);
  }
};

std::list<Point_2> getTriangle() {
  std::list<Point_2> poly_vertices;
  poly_vertices.push_back(Point_2(0, 0));
  poly_vertices.push_back(Point_2(2, 0));
  poly_vertices.push_back(Point_2(3, 1));
  return std::move(poly_vertices);
}

std::list<Point_2> getSquare() {
  std::list<Point_2> poly_vertices;
  poly_vertices.push_back(Point_2(0, 0));
  poly_vertices.push_back(Point_2(5, 0));
  poly_vertices.push_back(Point_2(5, 5));
  poly_vertices.push_back(Point_2(0, 5));
  return std::move(poly_vertices);
}

std::list<Point_2> getPolygonSingleConcavity() {
  std::list<Point_2> poly_vertices;
  poly_vertices.push_back(Point_2(0, 0));
  poly_vertices.push_back(Point_2(1, 1));
  poly_vertices.push_back(Point_2(2, 0));
  poly_vertices.push_back(Point_2(1, 4));
  return std::move(poly_vertices);
}

std::list<Point_2> getPolygonMultipleConcavities() {
  std::list<Point_2> poly_vertices;
  poly_vertices.push_back(Point_2(0, 0));
  poly_vertices.push_back(Point_2(2, 0));
  poly_vertices.push_back(Point_2(2, 1));
  poly_vertices.push_back(Point_2(1, 1));
  poly_vertices.push_back(Point_2(1, 3));
  poly_vertices.push_back(Point_2(2, 3));
  poly_vertices.push_back(Point_2(2, 4));
  poly_vertices.push_back(Point_2(0, 4));
  return std::move(poly_vertices);
}

TEST(EarClippingTest, EarsTest) {
  TestingWrapper test;

  // Triangle
  std::list<Point_2> triangle_vertices = getTriangle();
  auto triangle_iter = triangle_vertices.begin();
  EXPECT_EQ(test.isEarTest(triangle_vertices, triangle_iter++,
                           CGAL::COUNTERCLOCKWISE),
            true);
  EXPECT_EQ(test.isEarTest(triangle_vertices, triangle_iter++,
                           CGAL::COUNTERCLOCKWISE),
            true);
  EXPECT_EQ(test.isEarTest(triangle_vertices, triangle_iter++,
                           CGAL::COUNTERCLOCKWISE),
            true);

  // Square
  std::list<Point_2> square_vertices = getSquare();
  auto square_iter = square_vertices.begin();
  EXPECT_EQ(test.isEarTest(square_vertices, square_iter++,
                           CGAL::COUNTERCLOCKWISE),
            true);
  EXPECT_EQ(test.isEarTest(square_vertices, square_iter++,
                           CGAL::COUNTERCLOCKWISE),
            true);
  EXPECT_EQ(test.isEarTest(square_vertices, square_iter++,
                           CGAL::COUNTERCLOCKWISE),
            true);
  EXPECT_EQ(test.isEarTest(square_vertices, square_iter++,
                           CGAL::COUNTERCLOCKWISE),
            true);

  // Polygon with single concavity
  std::list<Point_2> concave_poly1_vertices = getPolygonSingleConcavity();
  auto concave1_iter = concave_poly1_vertices.begin();
  EXPECT_EQ(test.isEarTest(concave_poly1_vertices, concave1_iter++,
                           CGAL::COUNTERCLOCKWISE),
            true);
  // This should fail due to concave edge
  EXPECT_EQ(test.isEarTest(concave_poly1_vertices, concave1_iter++,
                           CGAL::COUNTERCLOCKWISE),
            false);
  EXPECT_EQ(test.isEarTest(concave_poly1_vertices, concave1_iter++,
                           CGAL::COUNTERCLOCKWISE),
            true);
  // This should fail because the triangle formed by joining line segment
  // contains (1, 1)
  EXPECT_EQ(test.isEarTest(concave_poly1_vertices, concave1_iter++,
                           CGAL::COUNTERCLOCKWISE),
            false);

  // Polygon with multiple concavities
  std::list<Point_2> concave_poly2_vertices = getPolygonMultipleConcavities();
  auto concave2_iter = concave_poly2_vertices.begin();
  EXPECT_EQ(test.isEarTest(concave_poly2_vertices, concave2_iter++,
                           CGAL::COUNTERCLOCKWISE),
            false);
  EXPECT_EQ(test.isEarTest(concave_poly2_vertices, concave2_iter++,
                           CGAL::COUNTERCLOCKWISE),
            true);
  EXPECT_EQ(test.isEarTest(concave_poly2_vertices, concave2_iter++,
                           CGAL::COUNTERCLOCKWISE),
            true);
  EXPECT_EQ(test.isEarTest(concave_poly2_vertices, concave2_iter++,
                           CGAL::COUNTERCLOCKWISE),
            false);
  EXPECT_EQ(test.isEarTest(concave_poly2_vertices, concave2_iter++,
                           CGAL::COUNTERCLOCKWISE),
            false);
  EXPECT_EQ(test.isEarTest(concave_poly2_vertices, concave2_iter++,
                           CGAL::COUNTERCLOCKWISE),
            true);
  EXPECT_EQ(test.isEarTest(concave_poly2_vertices, concave2_iter++,
                           CGAL::COUNTERCLOCKWISE),
            true);
  EXPECT_EQ(test.isEarTest(concave_poly2_vertices, concave2_iter++,
                           CGAL::COUNTERCLOCKWISE),
            false);
}

Polygon_2 getPolygon(const std::list<Point_2>& poly_vertices) {
  Polygon_2 poly(poly_vertices.begin(), poly_vertices.end());
  return std::move(poly);
}

void printTriangles(const std::vector<Triangle_2>& triangulation) {
  for (const auto& triangle : triangulation) {
    std::cout << "(" << triangle.vertex(0).x() << "," << triangle.vertex(0).y()
              << "), ";
    std::cout << "(" << triangle.vertex(1).x() << "," << triangle.vertex(1).y()
              << "), ";
    std::cout << "(" << triangle.vertex(2).x() << "," << triangle.vertex(2).y()
              << "), \n";
  }
}

TEST(EarClippingTest, TriangulationTest) {
  TestingWrapper test;

  auto triangle_poly = getPolygon(getTriangle());
  auto triangulated_triangle = test.triangulate(triangle_poly);
  EXPECT_EQ(triangulated_triangle.size(), 1);

  auto square_poly = getPolygon(getSquare());
  auto triangulated_square = test.triangulate(square_poly);
  EXPECT_EQ(triangulated_square.size(), 2);

  auto single_concavity_poly = getPolygon(getPolygonSingleConcavity());
  auto triangulated_single_concave = test.triangulate(single_concavity_poly);
  EXPECT_EQ(triangulated_single_concave.size(), 2);

  auto multiple_concavity_poly = getPolygon(getPolygonMultipleConcavities());
  auto triangulated_multiple_concave = test.triangulate(multiple_concavity_poly);
  EXPECT_EQ(triangulated_multiple_concave.size(), 6);

  // Check for triangulation when input polygon is oriented Clock-wise.
  auto multiple_concavity_vertex_list = getPolygonMultipleConcavities();
  multiple_concavity_vertex_list.reverse();
  auto multiple_concave_polygon_rev = getPolygon(multiple_concavity_vertex_list);
  auto triangulated_multiple_concave_rev =
      test.triangulate(multiple_concave_polygon_rev);
  EXPECT_EQ(triangulated_multiple_concave_rev.size(), 6);
}

std::list<Point_2> getNonSimplePolygon() {
  std::list<Point_2> poly_vertices;
  poly_vertices.push_back(Point_2(0, 0));
  poly_vertices.push_back(Point_2(5, 0));
  poly_vertices.push_back(Point_2(4, 1));
  poly_vertices.push_back(Point_2(3, 0));
  poly_vertices.push_back(Point_2(2, 1));
  return std::move(poly_vertices);
}

TEST(EarClippingTest, TestSimplePoly) {
  TestingWrapper test;

  auto non_simple_poly = getPolygon(getNonSimplePolygon());
  EXPECT_THROW(test.triangulate(non_simple_poly), std::invalid_argument);
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
