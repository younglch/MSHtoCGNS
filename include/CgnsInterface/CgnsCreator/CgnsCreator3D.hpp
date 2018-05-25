#ifndef CGNS_INTERFACE_CGNS_FILE_3D_HPP
#define CGNS_INTERFACE_CGNS_FILE_3D_HPP

#include <CgnsInterface/CgnsCreator.hpp>

class CgnsCreator3D : public CgnsCreator {
	public:
		CgnsCreator3D(GridDataShared gridData, const std::string& folderPath);

		~CgnsCreator3D() = default;

	private:
		void checkDimension() override;
		void defineGeometryType();
		void writeCoordinates() override;
		void buildElementConnectivities() override;
		void writeRegions() override;
		void buildFacetConnectivities() override;
		void writeBoundaries() override;

		int geometry, boundary;
};

#endif