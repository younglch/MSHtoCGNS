#include <BoostInterface/Test.hpp>
#include <CgnsInterface/CgnsReader/CgnsReader2D.hpp>

#define TOLERANCE 1e-12

struct Region1_ElementType2_2D_Cgns {
    Region1_ElementType2_2D_Cgns() {
        CgnsReader2D cgnsReader2D(std::string(TEST_INPUT_DIRECTORY) + "CgnsInterface/2D-Region1-ElementType2/11v_10e.cgns");
        this->gridData = cgnsReader2D.gridData;
    }

    ~Region1_ElementType2_2D_Cgns() = default;

    boost::shared_ptr<GridData> gridData;
};

FixtureTestSuite(ReadCgns_Region1_ElementType2_2D, Region1_ElementType2_2D_Cgns)

TestCase(Coordinates) {
    auto coordinates = this->gridData->coordinates;

    checkEqual(coordinates.size(), 11u);
    checkClose(coordinates[ 0][0], 0.00, TOLERANCE); checkClose(coordinates[ 0][1], 0.00, TOLERANCE); checkClose(coordinates[ 0][2], 0.00, TOLERANCE);
    checkClose(coordinates[ 1][0], 0.50, TOLERANCE); checkClose(coordinates[ 1][1], 0.00, TOLERANCE); checkClose(coordinates[ 1][2], 0.00, TOLERANCE);
    checkClose(coordinates[ 2][0], 1.00, TOLERANCE); checkClose(coordinates[ 2][1], 0.00, TOLERANCE); checkClose(coordinates[ 2][2], 0.00, TOLERANCE);
    checkClose(coordinates[ 3][0], 0.00, TOLERANCE); checkClose(coordinates[ 3][1], 0.50, TOLERANCE); checkClose(coordinates[ 3][2], 0.00, TOLERANCE);
    checkClose(coordinates[ 4][0], 0.50, TOLERANCE); checkClose(coordinates[ 4][1], 0.50, TOLERANCE); checkClose(coordinates[ 4][2], 0.00, TOLERANCE);
    checkClose(coordinates[ 5][0], 1.00, TOLERANCE); checkClose(coordinates[ 5][1], 0.50, TOLERANCE); checkClose(coordinates[ 5][2], 0.00, TOLERANCE);
    checkClose(coordinates[ 6][0], 0.00, TOLERANCE); checkClose(coordinates[ 6][1], 1.00, TOLERANCE); checkClose(coordinates[ 6][2], 0.00, TOLERANCE);
    checkClose(coordinates[ 7][0], 0.50, TOLERANCE); checkClose(coordinates[ 7][1], 1.00, TOLERANCE); checkClose(coordinates[ 7][2], 0.00, TOLERANCE);
    checkClose(coordinates[ 8][0], 1.00, TOLERANCE); checkClose(coordinates[ 8][1], 1.00, TOLERANCE); checkClose(coordinates[ 8][2], 0.00, TOLERANCE);
    checkClose(coordinates[ 9][0], 0.25, TOLERANCE); checkClose(coordinates[ 9][1], 0.75, TOLERANCE); checkClose(coordinates[ 9][2], 0.00, TOLERANCE);
    checkClose(coordinates[10][0], 0.75, TOLERANCE); checkClose(coordinates[10][1], 0.25, TOLERANCE); checkClose(coordinates[10][2], 0.00, TOLERANCE);
}

TestCase(Elements) {
    auto triangles = this->gridData->triangleConnectivity;

    checkEqual(triangles.size(), 8u);
    checkEqual(triangles[0][0], 3); checkEqual(triangles[0][1],  4); checkEqual(triangles[0][2],  9); checkEqual(triangles[0][3], 0);
    checkEqual(triangles[1][0], 3); checkEqual(triangles[1][1],  9); checkEqual(triangles[1][2],  6); checkEqual(triangles[1][3], 1);
    checkEqual(triangles[2][0], 4); checkEqual(triangles[2][1],  7); checkEqual(triangles[2][2],  9); checkEqual(triangles[2][3], 2);
    checkEqual(triangles[3][0], 6); checkEqual(triangles[3][1],  9); checkEqual(triangles[3][2],  7); checkEqual(triangles[3][3], 3);
    checkEqual(triangles[4][0], 1); checkEqual(triangles[4][1],  2); checkEqual(triangles[4][2], 10); checkEqual(triangles[4][3], 4);
    checkEqual(triangles[5][0], 1); checkEqual(triangles[5][1], 10); checkEqual(triangles[5][2],  4); checkEqual(triangles[5][3], 5);
    checkEqual(triangles[6][0], 2); checkEqual(triangles[6][1],  5); checkEqual(triangles[6][2], 10); checkEqual(triangles[6][3], 6);
    checkEqual(triangles[7][0], 4); checkEqual(triangles[7][1], 10); checkEqual(triangles[7][2],  5); checkEqual(triangles[7][3], 7);

    auto quadrangles = this->gridData->quadrangleConnectivity;

    checkEqual(quadrangles.size(), 2u);
    checkEqual(quadrangles[0][0], 4); checkEqual(quadrangles[0][1], 5); checkEqual(quadrangles[0][2], 8); checkEqual(quadrangles[0][3], 7); checkEqual(quadrangles[0][4], 8);
    checkEqual(quadrangles[1][0], 0); checkEqual(quadrangles[1][1], 1); checkEqual(quadrangles[1][2], 4); checkEqual(quadrangles[1][3], 3); checkEqual(quadrangles[1][4], 9);
}

TestCase(Facets) {
    auto lines = this->gridData->lineConnectivity;

    checkEqual(lines.size(), 8u);
    checkEqual(lines[0][0], 6); checkEqual(lines[0][1], 3); checkEqual(lines[0][2], 10);
    checkEqual(lines[1][0], 3); checkEqual(lines[1][1], 0); checkEqual(lines[1][2], 11);
    checkEqual(lines[2][0], 2); checkEqual(lines[2][1], 5); checkEqual(lines[2][2], 12);
    checkEqual(lines[3][0], 5); checkEqual(lines[3][1], 8); checkEqual(lines[3][2], 13);
    checkEqual(lines[4][0], 0); checkEqual(lines[4][1], 1); checkEqual(lines[4][2], 14);
    checkEqual(lines[5][0], 1); checkEqual(lines[5][1], 2); checkEqual(lines[5][2], 15);
    checkEqual(lines[6][0], 8); checkEqual(lines[6][1], 7); checkEqual(lines[6][2], 16);
    checkEqual(lines[7][0], 7); checkEqual(lines[7][1], 6); checkEqual(lines[7][2], 17);
}

TestCase(Regions) {
    checkEqual(this->gridData->regions.size(), 1u);
}

TestCase(Geometry) {
    auto region = this->gridData->regions[0];

    check(region.name == std::string("Geometry"));

    checkEqual(region.elementBegin, 0);
    checkEqual(region.elementEnd, 10);
}

TestCase(Boundaries) {
    checkEqual(this->gridData->boundaries.size(), 4u);
}

TestCase(West) {
    auto boundary = this->gridData->boundaries[0];

    check(boundary.name == std::string("West"));

    checkEqual(boundary.facetBegin, 10);
    checkEqual(boundary.facetEnd, 12);

    auto vertices = boundary.vertices;
    checkEqual(vertices.size(), 3u);
    checkEqual(vertices[0], 0);
    checkEqual(vertices[1], 3);
    checkEqual(vertices[2], 6);
}

TestCase(East) {
    auto boundary = this->gridData->boundaries[1];

    check(boundary.name == std::string("East"));

    checkEqual(boundary.facetBegin, 12);
    checkEqual(boundary.facetEnd, 14);

    auto vertices = boundary.vertices;
    checkEqual(vertices.size(), 3u);
    checkEqual(vertices[0], 2);
    checkEqual(vertices[1], 5);
    checkEqual(vertices[2], 8);
}

TestCase(South) {
    auto boundary = this->gridData->boundaries[2];

    check(boundary.name == std::string("South"));

    checkEqual(boundary.facetBegin, 14);
    checkEqual(boundary.facetEnd, 16);

    auto vertices = boundary.vertices;
    checkEqual(vertices.size(), 3u);
    checkEqual(vertices[0], 0);
    checkEqual(vertices[1], 1);
    checkEqual(vertices[2], 2);
}

TestCase(North) {
    auto boundary = this->gridData->boundaries[3];

    check(boundary.name == std::string("North"));

    checkEqual(boundary.facetBegin, 16);
    checkEqual(boundary.facetEnd, 18);

    auto vertices = boundary.vertices;
    checkEqual(vertices.size(), 3u);
    checkEqual(vertices[0], 6);
    checkEqual(vertices[1], 7);
    checkEqual(vertices[2], 8);
}

TestSuiteEnd()
