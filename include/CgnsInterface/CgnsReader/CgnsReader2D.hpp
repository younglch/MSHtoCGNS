#ifndef CGNS_READER_2D_HPP
#define CGNS_READER_2D_HPP

#include <CgnsInterface/CgnsReader/CgnsReader.hpp>

class CgnsReader2D : public CgnsReader {
	public:
		CgnsReader2D() = default;
		CgnsReader2D(const std::string&);

	private:
		void readCoordinates() override;
		void readSections() override;
};

#endif