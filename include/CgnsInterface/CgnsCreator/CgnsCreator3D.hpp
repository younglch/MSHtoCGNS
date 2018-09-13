#ifndef CGNS_INTERFACE_CGNS_CREATOR_3D_HPP
#define CGNS_INTERFACE_CGNS_CREATOR_3D_HPP

#include <CgnsInterface/CgnsCreator.hpp>

class CgnsCreator3D : public CgnsCreator {
	public:
		CgnsCreator3D(GridDataShared gridData, std::string folderPath);

	private:
		void checkDimension() override;
		void setDimensions() override;
		void writeCoordinates() override;
		void writeSections() override;
		void buildElementConnectivities() override;
		void writeRegions() override;
		void buildFacetConnectivities() override;
		void writeBoundaries() override;
		void buildWellConnectivities();
		void writeWells();

		int numberOfElements;
		int numberOfFacets;
		std::vector<std::vector<int>> wellConnectivities;
};

#endif