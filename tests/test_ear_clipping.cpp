#include "../include/ear_clipping.h"
#include <gtest/gtest.h>

using namespace ear_clipping;

class TestingWrapper : public EarClipping {
public:
    bool isEarTest(const std::list<Point_2>& poly_vertices, std::list<Point_2>::const_iterator vertex_iterator, CGAL::Orientation polygon_orientation) {
        return EarClipping::isEar(poly_vertices, vertex_iterator, polygon_orientation);
    }

    std::vector<Triangle_2> triangulateTest(const Polygon_2& simple_polygon) {
        return EarClipping::triangulateSimplePolygon(simple_polygon);
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
    std::list<Point_2> poly_triangle_vertices = getTriangle();
    auto it_triangle = poly_triangle_vertices.begin();
    EXPECT_EQ(test.isEarTest(poly_triangle_vertices, it_triangle++, CGAL::COUNTERCLOCKWISE), true);
    EXPECT_EQ(test.isEarTest(poly_triangle_vertices, it_triangle++, CGAL::COUNTERCLOCKWISE), true);
    EXPECT_EQ(test.isEarTest(poly_triangle_vertices, it_triangle++, CGAL::COUNTERCLOCKWISE), true);

    // Square
    std::list<Point_2> poly_square_vertices = getSquare();
    auto it_square = poly_square_vertices.begin();
    EXPECT_EQ(test.isEarTest(poly_square_vertices, it_square++, CGAL::COUNTERCLOCKWISE), true);
    EXPECT_EQ(test.isEarTest(poly_square_vertices, it_square++, CGAL::COUNTERCLOCKWISE), true);
    EXPECT_EQ(test.isEarTest(poly_square_vertices, it_square++, CGAL::COUNTERCLOCKWISE), true);
    EXPECT_EQ(test.isEarTest(poly_square_vertices, it_square++, CGAL::COUNTERCLOCKWISE), true);

    //Polygon with single concavity
    std::list<Point_2> poly_concave_vertices1 = getPolygonSingleConcavity();
    auto it_concave1 = poly_concave_vertices1.begin();
    EXPECT_EQ(test.isEarTest(poly_concave_vertices1, it_concave1++, CGAL::COUNTERCLOCKWISE), true);
    //This should fail due to concave edge
    EXPECT_EQ(test.isEarTest(poly_concave_vertices1, it_concave1++, CGAL::COUNTERCLOCKWISE), false);
    EXPECT_EQ(test.isEarTest(poly_concave_vertices1, it_concave1++, CGAL::COUNTERCLOCKWISE), true);
    //This should fail because the triangle formed by joining line segment containes (1, 1)
    EXPECT_EQ(test.isEarTest(poly_concave_vertices1, it_concave1++, CGAL::COUNTERCLOCKWISE), false);

    //Polygon with multiples concavities
    std::list<Point_2> poly_concave_vertices2 = getPolygonMultipleConcavities();
    auto it_concave2 = poly_concave_vertices2.begin();
    EXPECT_EQ(test.isEarTest(poly_concave_vertices2, it_concave2++, CGAL::COUNTERCLOCKWISE), false);
    EXPECT_EQ(test.isEarTest(poly_concave_vertices2, it_concave2++, CGAL::COUNTERCLOCKWISE), true);
    EXPECT_EQ(test.isEarTest(poly_concave_vertices2, it_concave2++, CGAL::COUNTERCLOCKWISE), true);
    EXPECT_EQ(test.isEarTest(poly_concave_vertices2, it_concave2++, CGAL::COUNTERCLOCKWISE), false);
    EXPECT_EQ(test.isEarTest(poly_concave_vertices2, it_concave2++, CGAL::COUNTERCLOCKWISE), false);
    EXPECT_EQ(test.isEarTest(poly_concave_vertices2, it_concave2++, CGAL::COUNTERCLOCKWISE), true);
    EXPECT_EQ(test.isEarTest(poly_concave_vertices2, it_concave2++, CGAL::COUNTERCLOCKWISE), true);
    EXPECT_EQ(test.isEarTest(poly_concave_vertices2, it_concave2++, CGAL::COUNTERCLOCKWISE), false);
}

Polygon_2 getPolygon(const std::list<Point_2>& poly_vertices) {
    Polygon_2 polygon(poly_vertices.begin(), poly_vertices.end());
    return std::move(polygon);
}

void printTriangles(const std::vector<Triangle_2>& triangulation) {
    for (const auto& triangle: triangulation) {
        std::cout << "(" << triangle.vertex(0).x() << "," << triangle.vertex(0).y() << "), "; 
        std::cout << "(" << triangle.vertex(1).x() << "," << triangle.vertex(1).y() << "), "; 
        std::cout << "(" << triangle.vertex(2).x() << "," << triangle.vertex(2).y() << "), \n"; 
    }
}

TEST(EarClippingTest, TriangulationTest) {
    TestingWrapper test;
    
    auto triangle_polygon = getPolygon(getTriangle());
    auto triangulated_triangle = test.triangulateSimplePolygon(triangle_polygon);
    EXPECT_EQ(triangulated_triangle.size(), 1);

    auto square_polygon = getPolygon(getSquare());
    auto triangulated_square = test.triangulateSimplePolygon(square_polygon);
    EXPECT_EQ(triangulated_square.size(), 2);

    auto single_concavity_polygon = getPolygon(getPolygonSingleConcavity());
    auto triangulated_single_concave = test.triangulateSimplePolygon(single_concavity_polygon);
    EXPECT_EQ(triangulated_single_concave.size(), 2);

    auto multiple_concavity_polygon = getPolygon(getPolygonMultipleConcavities());
    auto triangulated_multiple_concave = test.triangulateSimplePolygon(multiple_concavity_polygon);
    EXPECT_EQ(triangulated_multiple_concave.size(), 6);

    //Check for triangulation when input polygon is oriented Clock-wise. 
    auto multiple_concavity_vertex_list = getPolygonMultipleConcavities();
    multiple_concavity_vertex_list.reverse();
    auto multiple_concave_polygon_rev = getPolygon(multiple_concavity_vertex_list);
    auto triangulated_multiple_concave_rev = test.triangulateSimplePolygon(multiple_concave_polygon_rev);
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
    
    auto non_simple_polygon = getPolygon(getNonSimplePolygon());
    EXPECT_THROW(test.triangulateSimplePolygon(non_simple_polygon), std::invalid_argument);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}