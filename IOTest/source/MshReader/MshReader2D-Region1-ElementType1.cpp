#include <BoostInterface/Test.hpp>
#include <Grid/GridData.hpp>
#include <IO/MshReader2D.hpp>

#define TOLERANCE 1e-12

struct Region1_ElementType1_2D {
	Region1_ElementType1_2D() {
		MshReader2D mshReader2D("/home/felipe/Felipe/cpp/MSHtoCGNS/Zeta/TestFiles/2D/5n_4e.msh");
		this->gridData = mshReader2D.getGridData();
	}

	~Region1_ElementType1_2D() = default;

	GridDataShared gridData;
};

FixtureTestSuite(ReadMsh2D_Region1_ElementType1, Region1_ElementType1_2D)

TestCase(Coordinates) {
	auto coordinates = this->gridData->coordinates;

	checkEqual(static_cast<int>(coordinates.size()), 5);
	checkClose(coordinates[0][0], 0.0, TOLERANCE); checkClose(coordinates[0][1], 0.0, TOLERANCE); checkClose(coordinates[0][2], 0.0, TOLERANCE);
	checkClose(coordinates[1][0], 1.0, TOLERANCE); checkClose(coordinates[1][1], 0.0, TOLERANCE); checkClose(coordinates[1][2], 0.0, TOLERANCE);
	checkClose(coordinates[2][0], 1.0, TOLERANCE); checkClose(coordinates[2][1], 1.0, TOLERANCE); checkClose(coordinates[2][2], 0.0, TOLERANCE);
	checkClose(coordinates[3][0], 0.0, TOLERANCE); checkClose(coordinates[3][1], 1.0, TOLERANCE); checkClose(coordinates[3][2], 0.0, TOLERANCE);
	checkClose(coordinates[4][0], 0.5, TOLERANCE); checkClose(coordinates[4][1], 0.5, TOLERANCE); checkClose(coordinates[4][2], 0.0, TOLERANCE);
}

TestCase(Elements) {
	auto triangles = this->gridData->triangleConnectivity;

	checkEqual(static_cast<int>(triangles.size()), 4);
	checkEqual(triangles[0][0], 0); checkEqual(triangles[0][1], 1); checkEqual(triangles[0][2], 4);
	checkEqual(triangles[1][0], 0); checkEqual(triangles[1][1], 4); checkEqual(triangles[1][2], 3);
	checkEqual(triangles[2][0], 1); checkEqual(triangles[2][1], 2); checkEqual(triangles[2][2], 4);
	checkEqual(triangles[3][0], 2); checkEqual(triangles[3][1], 3); checkEqual(triangles[3][2], 4);
}

TestCase(Boundaries) {
	checkEqual(static_cast<int>(this->gridData->boundaries.size()), 4);
}

TestCase(West) {
	BoundaryData west = this->gridData->boundaries[0];

	check(west.name == std::string("West"));

	auto lines = west.lineConnectivity;
	checkEqual(static_cast<int>(lines.size()), 1);
	checkEqual(lines[0][0], 3); checkEqual(lines[0][1], 0);

	auto vertices = west.vertices;
	checkEqual(static_cast<int>(vertices.size()), 2);
	checkEqual(vertices[0], 0);
	checkEqual(vertices[1], 3); 
}

TestCase(East) {
	BoundaryData east = this->gridData->boundaries[1];

	check(east.name == std::string("East"));

	auto lines = east.lineConnectivity;
	checkEqual(static_cast<int>(lines.size()), 1);
	checkEqual(lines[0][0], 1); checkEqual(lines[0][1], 2);

	auto vertices = east.vertices;
	checkEqual(static_cast<int>(vertices.size()), 2);
	checkEqual(vertices[0], 1);
	checkEqual(vertices[1], 2); 
}

TestCase(South) {
	BoundaryData south = this->gridData->boundaries[2];

	check(south.name == std::string("South"));

	auto lines = south.lineConnectivity;
	checkEqual(static_cast<int>(lines.size()), 1);
	checkEqual(lines[0][0], 0); checkEqual(lines[0][1], 1);

	auto vertices = south.vertices;
	checkEqual(static_cast<int>(vertices.size()), 2);
	checkEqual(vertices[0], 0);
	checkEqual(vertices[1], 1); 
}

TestCase(North) {
	BoundaryData north = this->gridData->boundaries[3];

	check(north.name == std::string("North"));

	auto lines = north.lineConnectivity;
	checkEqual(static_cast<int>(lines.size()), 1);
	checkEqual(lines[0][0], 2); checkEqual(lines[0][1], 3);

	auto vertices = north.vertices;
	checkEqual(static_cast<int>(vertices.size()), 2);
	checkEqual(vertices[0], 2);
	checkEqual(vertices[1], 3); 
}

TestCase(Regions) {
	checkEqual(static_cast<int>(this->gridData->regions.size()), 1);

	auto elementsOnRegion = this->gridData->regions[0].elementsOnRegion;
	checkEqual(static_cast<int>(elementsOnRegion.size()), 4);
	checkEqual(elementsOnRegion[0], 0);
	checkEqual(elementsOnRegion[1], 1);
	checkEqual(elementsOnRegion[2], 2);
	checkEqual(elementsOnRegion[3], 3);
}

TestSuiteEnd()