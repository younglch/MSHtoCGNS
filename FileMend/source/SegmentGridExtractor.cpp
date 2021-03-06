#include <FileMend/SegmentGridExtractor.hpp>
#include <cgnslib.h>

SegmentGridExtractor::SegmentGridExtractor(boost::shared_ptr<GridData> gridData) : gridData(gridData) {
    this->checkGridData();
    this->createSegmentGrid();
    this->defineQuantities();
    this->copyVertices();
    this->copyElements();
    this->copyFacets();
    this->copyLine();
    this->fixRegion();
    this->fixBoundaries();
    this->fixWell();
}

void SegmentGridExtractor::checkGridData() {
    if (this->gridData->dimension != 3)
        throw std::runtime_error(std::string(__PRETTY_FUNCTION__) + " - gridData dimension must be 3 and not " + std::to_string(this->gridData->dimension));

    if (this->gridData->tetrahedronConnectivity.size() != 0u)
        throw std::runtime_error(std::string(__PRETTY_FUNCTION__) + " - gridData tetrahedronConnectivity size must be 0 and not " + std::to_string(this->gridData->tetrahedronConnectivity.size()));

    if (this->gridData->hexahedronConnectivity.size() == 0u)
        throw std::runtime_error(std::string(__PRETTY_FUNCTION__) + " - gridData hexahedronConnectivity size must not be 0");

    if (this->gridData->prismConnectivity.size() == 0u)
        throw std::runtime_error(std::string(__PRETTY_FUNCTION__) + " - gridData prismConnectivity size must not be 0");

    if (this->gridData->pyramidConnectivity.size() != 0u)
        throw std::runtime_error(std::string(__PRETTY_FUNCTION__) + " - gridData pyramidConnectivity size must be 0 and not " + std::to_string(this->gridData->pyramidConnectivity.size()));

    if (this->gridData->triangleConnectivity.size() == 0u)
        throw std::runtime_error(std::string(__PRETTY_FUNCTION__) + " - gridData triangleConnectivity size must not be 0");

    if (this->gridData->quadrangleConnectivity.size() == 0u)
        throw std::runtime_error(std::string(__PRETTY_FUNCTION__) + " - gridData quadrangleConnectivity size must not be 0");

    if (this->gridData->lineConnectivity.size() == 0u)
        throw std::runtime_error(std::string(__PRETTY_FUNCTION__) + " - gridData lineConnectivity size must not be 0");

    if (this->gridData->boundaries.size() != 3u)
        throw std::runtime_error(std::string(__PRETTY_FUNCTION__) + " - gridData boundaries size must be 1 and not " + std::to_string(this->gridData->boundaries.size()));

    if (this->gridData->regions.size() != 1u)
        throw std::runtime_error(std::string(__PRETTY_FUNCTION__) + " - gridData regions size must be 1 and not " + std::to_string(this->gridData->regions.size()));

    if (this->gridData->wells.size() != 1u)
        throw std::runtime_error(std::string(__PRETTY_FUNCTION__) + " - gridData wells size must be 1 and not " + std::to_string(this->gridData->wells.size()));
}

void SegmentGridExtractor::defineQuantities() {
    this->numberOfSegments = this->gridData->lineConnectivity.size();
    this->numberOfPrismsPerSegment = this->gridData->prismConnectivity.size() / this->numberOfSegments;
    this->numberOfHexahedronsPerSegment = this->gridData->hexahedronConnectivity.size() / this->numberOfSegments;
    this->numberOfHexahedronsPerRadius = this->numberOfHexahedronsPerSegment / this->numberOfPrismsPerSegment;
    this->numberOfVerticesPerSection = this->gridData->coordinates.size() / (this->gridData->lineConnectivity.size() + 1);
}

void SegmentGridExtractor::createSegmentGrid() {
    this->segmentGrid = boost::make_shared<GridData>();
    this->segmentGrid->dimension = this->gridData->dimension;

    this->segmentGrid->boundaries = this->gridData->boundaries;

    this->segmentGrid->regions = this->gridData->regions;

    this->segmentGrid->wells = this->gridData->wells;
}

void SegmentGridExtractor::copyVertices() {
    for (int vertex = 0; vertex < 2 * this->numberOfVerticesPerSection; vertex++)
        this->segmentGrid->coordinates.emplace_back(this->gridData->coordinates[vertex]);
}

void SegmentGridExtractor::copyElements() {
    for (int prism = 0; prism < this->numberOfPrismsPerSegment; prism++) {
        this->segmentGrid->prismConnectivity.emplace_back(this->gridData->prismConnectivity[prism]);
        this->segmentGrid->prismConnectivity[prism].back() = this->elementShift++;
    }

    for (int hexahedron = 0; hexahedron < this->numberOfHexahedronsPerSegment; hexahedron++) {
        this->segmentGrid->hexahedronConnectivity.emplace_back(this->gridData->hexahedronConnectivity[hexahedron]);
        this->segmentGrid->hexahedronConnectivity[hexahedron].back() = this->elementShift++;
    }
}

void SegmentGridExtractor::copyFacets() {
    for (int quadrangle = 0; quadrangle < this->numberOfPrismsPerSegment; quadrangle++) {
        this->segmentGrid->quadrangleConnectivity.emplace_back(this->gridData->quadrangleConnectivity[quadrangle]);
        this->segmentGrid->quadrangleConnectivity[quadrangle].back() = this->elementShift++;
    }

    for (int triangle = 0; triangle < this->numberOfPrismsPerSegment; triangle++) {
        this->segmentGrid->triangleConnectivity.emplace_back(this->gridData->triangleConnectivity[triangle]);
        this->segmentGrid->triangleConnectivity[triangle].back() = this->elementShift++;
    }

    for (int quadrangle = this->numberOfPrismsPerSegment * this->numberOfSegments; quadrangle < this->numberOfPrismsPerSegment * this->numberOfSegments + this->numberOfHexahedronsPerSegment; quadrangle++) {
        this->segmentGrid->quadrangleConnectivity.emplace_back(this->gridData->quadrangleConnectivity[quadrangle]);
        this->segmentGrid->quadrangleConnectivity.back().back() = this->elementShift++;
    }

    for (int triangle = this->numberOfPrismsPerSegment; triangle < 2 * this->numberOfPrismsPerSegment; triangle++) {
        this->segmentGrid->triangleConnectivity.emplace_back(this->gridData->triangleConnectivity[triangle]);
        this->segmentGrid->triangleConnectivity[triangle].back() = this->elementShift++;
    }

    for (int quadrangle = this->numberOfPrismsPerSegment * this->numberOfSegments + this->numberOfHexahedronsPerSegment; quadrangle < int(this->gridData->quadrangleConnectivity.size()); quadrangle++) {
        this->segmentGrid->quadrangleConnectivity.emplace_back(this->gridData->quadrangleConnectivity[quadrangle]);
        this->segmentGrid->quadrangleConnectivity.back().back() = this->elementShift++;
    }
}

void SegmentGridExtractor::copyLine() {
    this->segmentGrid->lineConnectivity.emplace_back(this->gridData->lineConnectivity[0]);
    this->segmentGrid->lineConnectivity[0].back() = this->elementShift++;
}

void SegmentGridExtractor::fixRegion() {
    this->elementShift = this->numberOfPrismsPerSegment + this->numberOfHexahedronsPerSegment;
    this->segmentGrid->regions[0].elementBegin = 0;
    this->segmentGrid->regions[0].elementEnd = this->elementShift;
}

void SegmentGridExtractor::fixBoundaries() {
    // cylinder surface
    auto boundary = this->segmentGrid->boundaries.begin();
    boundary->facetBegin = this->elementShift;
    boundary->facetEnd = boundary->facetBegin + this->numberOfPrismsPerSegment;
    boundary->vertices.erase(std::remove_if(boundary->vertices.begin(), boundary->vertices.end(), [=](auto v){return v >= 2 * this->numberOfVerticesPerSection;}), boundary->vertices.end());

    // first lid
    boundary = this->segmentGrid->boundaries.begin() + 1;
    boundary->facetBegin = this->elementShift + this->numberOfPrismsPerSegment;
    boundary->facetEnd = boundary->facetBegin + this->numberOfPrismsPerSegment + this->numberOfHexahedronsPerSegment;
    std::stable_sort(boundary->vertices.begin(), boundary->vertices.end());

    // last lid
    boundary = this->segmentGrid->boundaries.begin() + 2;
    boundary->facetBegin = this->elementShift + this->numberOfPrismsPerSegment + this->numberOfPrismsPerSegment + this->numberOfHexahedronsPerSegment;
    boundary->facetEnd = boundary->facetBegin + this->numberOfPrismsPerSegment + this->numberOfHexahedronsPerSegment;
    std::stable_sort(boundary->vertices.begin(), boundary->vertices.end());

    std::unordered_map<int, int> lastToSecond;
    int shift = 0;
    for (auto& vertex : boundary->vertices) {
        lastToSecond[vertex] = this->numberOfVerticesPerSection + shift++;
        vertex = lastToSecond[vertex];
    }

    for (auto triangle = this->segmentGrid->triangleConnectivity.begin(); triangle != this->segmentGrid->triangleConnectivity.end(); triangle++)
        if (triangle->back() >= boundary->facetBegin && triangle->back() < boundary->facetEnd)
            for (auto vertex = triangle->begin(); vertex != triangle->end() - 1; vertex++)
                *vertex = lastToSecond[*vertex];

    for (auto quadrangle = this->segmentGrid->quadrangleConnectivity.begin(); quadrangle != this->segmentGrid->quadrangleConnectivity.end(); quadrangle++)
        if (quadrangle->back() >= boundary->facetBegin && quadrangle->back() < boundary->facetEnd)
            for (auto vertex = quadrangle->begin(); vertex != quadrangle->end() - 1; vertex++)
                *vertex = lastToSecond[*vertex];
}

void SegmentGridExtractor::fixWell() {
    this->segmentGrid->wells[0].lineBegin = this->segmentGrid->boundaries[2].facetEnd;
    this->segmentGrid->wells[0].lineEnd = this->segmentGrid->wells[0].lineBegin + 1;
    this->segmentGrid->wells[0].vertices = std::vector<int>{this->segmentGrid->lineConnectivity[0][0], this->segmentGrid->lineConnectivity[0][1]};
}
