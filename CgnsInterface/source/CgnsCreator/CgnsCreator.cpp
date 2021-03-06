#include <CgnsInterface/CgnsCreator.hpp>
#include <cgnslib.h>

CgnsCreator::CgnsCreator(boost::shared_ptr<GridData> gridData, std::string folderPath) : gridData(gridData), folderPath(folderPath), elementStart(1), elementEnd(0) {
    this->baseName = "Base";
    this->zoneName = "Zone";
}

void CgnsCreator::setupFile() {
    boost::filesystem::path input(this->folderPath);
    if (input.extension() == std::string(".cgns")) {
        if (boost::filesystem::exists(this->folderPath))
            boost::filesystem::remove_all(this->folderPath);

        this->fileName = this->folderPath;
    }
    else {
        std::string folderName = this->folderPath + std::string("/") + std::to_string(this->sizes[0]) + std::string("v_") + std::to_string(this->sizes[1]) + "e/";
        createDirectory(folderName);
        this->fileName = folderName + std::string("Grid.cgns");
    }
    cg_open(this->fileName.c_str(), CG_MODE_WRITE, &this->fileIndex);
}

void CgnsCreator::initialize() {
    this->writeBase();
    this->writeZone();
    this->writeCoordinates();
    this->buildGlobalConnectivities();
    this->writeSections();
    this->writeBoundaryConditions();
}

void CgnsCreator::writeBase() {
    if (cg_base_write(this->fileIndex, this->baseName.c_str(), this->cellDimension, this->physicalDimension, &this->baseIndex))
        throw std::runtime_error(std::string(__PRETTY_FUNCTION__) + " - Could not write base");
}

void CgnsCreator::writeZone() {
    if (cg_zone_write(this->fileIndex, this->baseIndex, this->zoneName.c_str(), this->sizes, Unstructured, &this->zoneIndex))
        throw std::runtime_error(std::string(__PRETTY_FUNCTION__) + " - Could not write zone");
}

void CgnsCreator::writeSections() {
    this->writeRegions();
    this->writeBoundaries();
}

void CgnsCreator::writeBoundaryConditions() {
    for (auto boundary = this->gridData->boundaries.cbegin(); boundary != this->gridData->boundaries.cend(); boundary++) {
        std::vector<int> indices;
        std::transform(boundary->vertices.cbegin(), boundary->vertices.cend(), std::back_inserter(indices), [](auto x){return x + 1;});

        if (cg_boco_write(this->fileIndex, this->baseIndex, this->zoneIndex, boundary->name.c_str(), BCWall, PointList, indices.size(), &indices[0], &this->boundaryIndex))
            throw std::runtime_error(std::string(__PRETTY_FUNCTION__) + " - Could not write boundary condition " + std::to_string(this->boundaryIndex));

        if (cg_goto(this->fileIndex, this->baseIndex, "Zone_t", this->zoneIndex, "ZoneBC_t", 1, "BC_t", this->boundaryIndex, nullptr))
            throw std::runtime_error(std::string(__PRETTY_FUNCTION__) + " - Could go to boundary condition " + std::to_string(this->boundaryIndex));

        if (cg_famname_write(boundary->name.c_str()))
            throw std::runtime_error(std::string(__PRETTY_FUNCTION__) + " - Could not write boundary condition " + std::to_string(this->boundaryIndex) + " family name");
    }
}

std::string CgnsCreator::getFileName() const {
    return this->fileName;
}

CgnsCreator::~CgnsCreator() {
    cg_close(this->fileIndex);
}
