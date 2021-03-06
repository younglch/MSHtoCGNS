#include <FileMend/GridDataExtractor.hpp>

GridDataExtractor::GridDataExtractor(boost::shared_ptr<GridData> original, std::string gridDataExtractorScript) : original(original) {
    this->checkGridData();
    boost::property_tree::read_json(gridDataExtractorScript, this->propertyTree);
    this->readScript();
    this->buildElementConnectivities();
    this->extract = boost::make_shared<GridData>();
    this->extract->dimension = 3;
    this->extractRegions();
    this->extractBoundaries();
    this->extractWells();
    this->extractVertices();
    this->fixIndices();
}

GridDataExtractor::GridDataExtractor(boost::shared_ptr<GridData> original, boost::property_tree::ptree propertyTree) : original(original), propertyTree(propertyTree) {
    this->checkGridData();
    this->readScript();
    this->buildElementConnectivities();
    this->extract = boost::make_shared<GridData>();
    this->extract->dimension = 3;
    this->extractRegions();
    this->extractBoundaries();
    this->extractWells();
    this->extractVertices();
    this->fixIndices();
}

void GridDataExtractor::checkGridData() {
    if (this->original->dimension != 3)
        throw std::runtime_error(std::string(__PRETTY_FUNCTION__) + " - original dimension must be 3 and not " + std::to_string(this->original->dimension));
}

void GridDataExtractor::readScript() {
    this->gridDataExtractorDatum.emplace_back();

    for (auto region : this->propertyTree.get_child("regions"))
        this->gridDataExtractorDatum.back().regions.emplace_back(region.second.get_value<std::string>());

    for (auto boundary : this->propertyTree.get_child("boundaries"))
        this->gridDataExtractorDatum.back().boundaries.emplace_back(boundary.second.get_value<std::string>());

    for (auto wells : this->propertyTree.get_child("wells"))
        this->gridDataExtractorDatum.back().wells.emplace_back(wells.second.get_value<std::string>());
}

void GridDataExtractor::buildElementConnectivities() {
    for (auto i = this->original->tetrahedronConnectivity.cbegin(); i != this->original->tetrahedronConnectivity.cend(); i++)
        this->elementConnectivities.emplace_back(i->cbegin(), i->cend());

    for (auto i = this->original->hexahedronConnectivity.cbegin(); i != this->original->hexahedronConnectivity.cend(); i++)
        this->elementConnectivities.emplace_back(i->cbegin(), i->cend());

    for (auto i = this->original->prismConnectivity.cbegin(); i != this->original->prismConnectivity.cend(); i++)
        this->elementConnectivities.emplace_back(i->cbegin(), i->cend());

    for (auto i = this->original->pyramidConnectivity.cbegin(); i != this->original->pyramidConnectivity.cend(); i++)
        this->elementConnectivities.emplace_back(i->cbegin(), i->cend());

    for (auto i = this->original->triangleConnectivity.cbegin(); i != this->original->triangleConnectivity.cend(); i++)
        this->elementConnectivities.emplace_back(i->cbegin(), i->cend());

    for (auto i = this->original->quadrangleConnectivity.cbegin(); i != this->original->quadrangleConnectivity.cend(); i++)
        this->elementConnectivities.emplace_back(i->cbegin(), i->cend());

    for (auto i = this->original->lineConnectivity.cbegin(); i != this->original->lineConnectivity.cend(); i++)
        this->elementConnectivities.emplace_back(i->cbegin(), i->cend());

    std::stable_sort(this->elementConnectivities.begin(), this->elementConnectivities.end(), [](const auto& a, const auto& b) {return a.back() < b.back();});

    for (unsigned i = 0; i < this->elementConnectivities.size(); i++)
        this->elementConnectivities[i].pop_back();
}

void GridDataExtractor::extractRegions() {
    for (auto name : this->gridDataExtractorDatum.back().regions) {

        auto iterator(std::find_if(this->original->regions.cbegin(),this->original->regions.cend(), [=](auto r){return r.name == name;}));
        if (iterator == this->original->regions.cend())
            throw std::runtime_error(std::string(__PRETTY_FUNCTION__) + " - There is no region " + name + " in gridData");
        auto region(*iterator);

        auto regionBegin = this->elementConnectivities.begin() + region.elementBegin;
        auto regionEnd = this->elementConnectivities.begin() + region.elementEnd;

        for (auto element = regionBegin; element != regionEnd; element++) {

            for (auto vertex  : *element)
                this->vertices.insert(vertex);

            element->push_back(this->localIndex++);

            switch (element->size()) {
                case 5: {
                    std::array<int, 5> tetrahedron;
                    std::copy_n(element->cbegin(), 5, std::begin(tetrahedron));
                    this->extract->tetrahedronConnectivity.emplace_back(std::move(tetrahedron));
                    break;
                }
                case 9: {
                    std::array<int, 9> hexahedron;
                    std::copy_n(element->cbegin(), 9, std::begin(hexahedron));
                    this->extract->hexahedronConnectivity.emplace_back(std::move(hexahedron));
                    break;
                }
                case 7: {
                    std::array<int, 7> prism;
                    std::copy_n(element->cbegin(), 7, std::begin(prism));
                    this->extract->prismConnectivity.emplace_back(std::move(prism));
                    break;
                }
                case 6: {
                    std::array<int, 6> pyramid;
                    std::copy_n(element->cbegin(), 6, std::begin(pyramid));
                    this->extract->pyramidConnectivity.emplace_back(std::move(pyramid));
                    break;
                }
            }

        }

        region.elementBegin = regionBegin->back();
        region.elementEnd = (regionEnd - 1)->back() + 1;

        this->extract->regions.emplace_back(region);
    }
}

void GridDataExtractor::extractBoundaries() {
    for (auto name : this->gridDataExtractorDatum.back().boundaries) {

        auto iterator(std::find_if(this->original->boundaries.cbegin(),this->original->boundaries.cend(), [=](auto b){return b.name == name;}));
        if (iterator == this->original->boundaries.cend())
            throw std::runtime_error(std::string(__PRETTY_FUNCTION__) + " - There is no boundary " + name + " in gridData");
        auto boundary(*iterator);

        std::vector<int> deleteIndices;
        for (auto i = this->original->triangleConnectivity.cbegin(); i != this->original->triangleConnectivity.cend(); i++)
            if (i->back() >= boundary.facetBegin && i->back() <= boundary.facetEnd)
                deleteIndices.emplace_back(i -  this->original->triangleConnectivity.cbegin());

        for (auto rit = deleteIndices.crbegin(); rit != deleteIndices.crend(); rit++)
            this->original->triangleConnectivity.erase(this->original->triangleConnectivity.begin() + *rit);

        deleteIndices.clear();
        for (auto i = this->original->quadrangleConnectivity.cbegin(); i != this->original->quadrangleConnectivity.cend(); i++)
            if (i->back() >= boundary.facetBegin && i->back() <= boundary.facetEnd)
                deleteIndices.emplace_back(i -  this->original->quadrangleConnectivity.cbegin());

        for (auto rit = deleteIndices.crbegin(); rit != deleteIndices.crend(); rit++)
            this->original->quadrangleConnectivity.erase(this->original->quadrangleConnectivity.begin() + *rit);

        this->original->boundaries.erase(iterator);

        auto boundaryBegin = this->elementConnectivities.begin() + boundary.facetBegin;
        auto boundaryEnd = this->elementConnectivities.begin() + boundary.facetEnd;

        for (auto facet = boundaryBegin; facet != boundaryEnd; facet++) {

            facet->push_back(this->localIndex++);

            switch (facet->size()) {
                case 4: {
                    std::array<int, 4> triangle;
                    std::copy_n(facet->cbegin(), 4, std::begin(triangle));
                    this->extract->triangleConnectivity.emplace_back(std::move(triangle));
                    break;
                }
                case 5: {
                    std::array<int, 5> quadrangle;
                    std::copy_n(facet->cbegin(), 5, std::begin(quadrangle));
                    this->extract->quadrangleConnectivity.emplace_back(std::move(quadrangle));
                    break;
                }
            }

        }

        boundary.facetBegin = boundaryBegin->back();
        boundary.facetEnd = (boundaryEnd - 1)->back() + 1;

        this->extract->boundaries.emplace_back(boundary);
    }
}

void GridDataExtractor::extractWells() {
    for (auto name : this->gridDataExtractorDatum.back().wells) {

        auto iterator(std::find_if(this->original->wells.cbegin(),this->original->wells.cend(), [=](auto w){return w.name == name;}));
        if (iterator == this->original->wells.cend())
            throw std::runtime_error(std::string(__PRETTY_FUNCTION__) + " - There is no well " + name + " in gridData");
        auto well(*iterator);

        auto wellBegin = this->elementConnectivities.begin() + well.lineBegin;
        auto wellEnd = this->elementConnectivities.begin() + well.lineEnd;

        for (auto element = wellBegin; element != wellEnd; element++) {

            element->push_back(this->localIndex++);

            std::array<int, 3> line;
            std::copy_n(element->cbegin(), 3, std::begin(line));
            this->extract->lineConnectivity.emplace_back(std::move(line));
        }

        well.lineBegin = wellBegin->back();
        well.lineEnd = (wellEnd - 1)->back() + 1;

        this->extract->wells.emplace_back(well);
    }
}

void GridDataExtractor::extractVertices() {
    for (auto vertex : this->vertices)
        this->extract->coordinates.push_back(this->original->coordinates[vertex]);
}

void GridDataExtractor::fixIndices() {
    std::unordered_map<int, int> originalToExtract;
    int index = 0;
    for (auto vertex : vertices)
        originalToExtract[vertex] = index++;

    for (auto& tetrahedron : this->extract->tetrahedronConnectivity)
        for (auto vertex = tetrahedron.begin(); vertex != tetrahedron.end() - 1; vertex++)
            *vertex = originalToExtract[*vertex];

    for (auto& hexahedron : this->extract->hexahedronConnectivity)
        for (auto vertex = hexahedron.begin(); vertex != hexahedron.end() - 1; vertex++)
            *vertex = originalToExtract[*vertex];

    for (auto& prism : this->extract->prismConnectivity)
        for (auto vertex = prism.begin(); vertex != prism.end() - 1; vertex++)
            *vertex = originalToExtract[*vertex];

    for (auto& pyramid : this->extract->pyramidConnectivity)
        for (auto vertex = pyramid.begin(); vertex != pyramid.end() - 1; vertex++)
            *vertex = originalToExtract[*vertex];

    for (auto& triangle : this->extract->triangleConnectivity)
        for (auto vertex = triangle.begin(); vertex != triangle.end() - 1; vertex++)
            *vertex = originalToExtract[*vertex];

    for (auto& quadrangle : this->extract->quadrangleConnectivity)
        for (auto vertex = quadrangle.begin(); vertex != quadrangle.end() - 1; vertex++)
            *vertex = originalToExtract[*vertex];

    for (auto& line : this->extract->lineConnectivity)
        for (auto vertex = line.begin(); vertex != line.end() - 1; vertex++)
            *vertex = originalToExtract[*vertex];

    for (auto& well : this->extract->wells)
        for (auto& vertex : well.vertices)
            vertex = originalToExtract[vertex];

    for (auto& boundary : this->extract->boundaries)
        for (auto& vertex : boundary.vertices)
            vertex = originalToExtract[vertex];
}
