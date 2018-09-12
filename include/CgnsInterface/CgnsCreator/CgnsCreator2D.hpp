#ifndef CGNS_INTERFACE_CGNS_CREATOR_2D_HPP
#define CGNS_INTERFACE_CGNS_CREATOR_2D_HPP

#include <CgnsInterface/CgnsCreator.hpp>

class CgnsCreator2D : public CgnsCreator {
	public:
		CgnsCreator2D(GridDataShared gridData, std::string folderPath);

	private:
		void checkDimension() override;
		void setDimensions() override;
		void writeCoordinates() override;
		void buildElementConnectivities() override;
		void writeRegions() override;
		void buildFacetConnectivities() override;
		void writeBoundaries() override;
};

#endif