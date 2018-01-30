#include <IO/CgnsReader2D.hpp>
 
CgnsReader2D::CgnsReader2D(const std::string& filePath) :
	CgnsReader(filePath) {
	this->gridData.dimension = this->cellDimension;
	this->gridData.thickness = 1.0;
	this->readNodes();
	this->readElements();
}

void CgnsReader2D::readNodes() {
	this->numberOfNodes = this->zoneSizes[0];
	cgsize_t one = 1;
	double coordinatesX[this->numberOfNodes];
	double coordinatesY[this->numberOfNodes];
	if (cg_coord_read(this->cgnsFile, this->cgnsBase, this->cgnsZone, "CoordinateX", RealDouble, &one, &this->numberOfNodes, coordinatesX)) {
		std::cerr << "\nError reading CoordinateX" << std::endl;	
		cg_error_exit();
	}
	if (cg_coord_read(this->cgnsFile, this->cgnsBase, this->cgnsZone, "CoordinateY", RealDouble, &one, &this->numberOfNodes, coordinatesY)) {
		std::cerr << "\nError reading CoordinateY" << std::endl;	
		cg_error_exit();
	}

	this->gridData.coordinates.resize(this->numberOfNodes, std::vector<double>(3));
	for (int i = 0; i < this->numberOfNodes; i++) {
		this->gridData.coordinates[i][0] = coordinatesX[i]; 
		this->gridData.coordinates[i][1] = coordinatesY[i]; 
		this->gridData.coordinates[i][2] = 0.0; 
	}
}

void CgnsReader2D::readElements() {
	for (int section = 1; section <= this->numberOfSections; section++) {
		ElementType_t type;
		cgsize_t elementStart, elementEnd; 
		int nBdry, parentFlag;
		std::vector<cgsize_t> connectivities;
		if (cg_section_read(this->cgnsFile, this->cgnsBase, this->cgnsZone, section, buffer, &type, &elementStart, &elementEnd, &nBdry, &parentFlag) != CG_OK) {
			std::cerr << "\nThere was a problem when reading section" << section << std::endl;	
			cg_error_exit();
		}
		int numberOfElements = elementEnd - elementStart + 1;
		
		switch (type) {
			case TRI_3: {
				int numberOfVertices = 3;	
				connectivities.resize(numberOfVertices*numberOfElements);
				cg_elements_read(this->cgnsFile, this->cgnsBase, this->cgnsZone, section, &connectivities[0], 0);
				for (int e = 0; e < numberOfElements; e++) {
					std::vector<int> triangle(numberOfVertices);
					for (int k = 0; k < numberOfVertices; k++) triangle[k] = connectivities[e*numberOfVertices+k] - 1;
					this->gridData.triangleConnectivity.emplace_back(std::move(triangle));	
				}
				break; 
			}
			case QUAD_4: {
				int numberOfVertices = 4;	
				connectivities.resize(numberOfVertices*numberOfElements);
				cg_elements_read(this->cgnsFile, this->cgnsBase, this->cgnsZone, section, &connectivities[0], 0);
				for (int e = 0; e < numberOfElements; e++) {
					std::vector<int> quadrilateral(numberOfVertices);
					for (int k = 0; k < numberOfVertices; k++) quadrilateral[k] = connectivities[e*numberOfVertices+k]-1;
					this->gridData.triangleConnectivity.emplace_back(std::move(quadrilateral));	
				}
				break; 
			}
			case BAR_2: {
				int numberOfVertices = 2;	
				connectivities.resize(numberOfVertices*numberOfElements);
				cg_elements_read(this->cgnsFile, this->cgnsBase, this->cgnsZone, section, &connectivities[0], 0);
				std::vector<std::vector<int>> lineConnectivity;
				for (int e = 0; e < numberOfElements; e++) {
					std::vector<int> line(numberOfVertices);
					for (int k = 0; k < numberOfVertices; k++) line[k] = connectivities[e*numberOfVertices+k]-1;
					lineConnectivity.emplace_back(std::move(line));
				}
				
				BoundaryData boundaryData;
				boundaryData.name = buffer;
				boundaryData.lineConnectivity = std::move(lineConnectivity);
				this->gridData.boundaries.emplace_back(std::move(boundaryData));
				break; 
			}
			case NODE: {
				int numberOfVertices = 2;	
				connectivities.resize(numberOfVertices*numberOfElements);
				WellData wellData;
				cg_elements_read(this->cgnsFile, this->cgnsBase, this->cgnsZone, section, &connectivities[0], 0);
				wellData.wellNode = connectivities[0];
				wellData.name = buffer;
				this->gridData.wells.emplace_back(std::move(wellData));
				break; 
			}
			default:
				std::cout << "\nUnsupported element encountered" << std::endl;	cg_error_exit();
				break;
		}
	}
}