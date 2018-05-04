#include <CgnsInterface/CgnsWriter.hpp>
#include <cgnslib.h>

CgnsWriter::CgnsWriter(const std::string& filePath) : filePath(filePath), isFinalized(false) {
	this->checkFile();
	this->readBase();
	this->readZone();
}

void CgnsWriter::checkFile() {
    boost::filesystem::path input(this->filePath);
	if (!boost::filesystem::exists(input.parent_path()))
		throw std::runtime_error("CgnsWriter: The parent path does not exist");

	if (!boost::filesystem::exists(this->filePath))
		throw std::runtime_error("CgnsWriter: There is no file in the given path");

	if (input.extension() != ".cgns")
		throw std::runtime_error("CgnsWriter: The file extension is not .cgns");

	if (cg_open(this->filePath.c_str(), CG_MODE_MODIFY, &this->fileIndex))
		throw std::runtime_error("CgnsWriter: Could not open the file " + this->filePath);
}

void CgnsWriter::readBase() {
	if (cg_nbases(this->fileIndex, &this->baseIndex))
		throw std::runtime_error("CgnsWriter: Could not read number of bases");

	if (this->baseIndex != 1)
		throw std::runtime_error("CgnsWriter: The CGNS file has more than one base");
}

void CgnsWriter::readZone() {
	if (cg_nzones(this->fileIndex, this->baseIndex, &this->zoneIndex))
		throw std::runtime_error("CgnsWriter: Could not read number of zones");

	if (this->zoneIndex != 1)
		throw std::runtime_error("CgnsWriter: The CGNS file has more than one zone");
}

void CgnsWriter::writePermanentSolution() {
	cg_sol_write(this->fileIndex, this->baseIndex, this->zoneIndex, "Solution", Vertex, &this->solutionIndex);
}

void CgnsWriter::writePermanentField(const std::vector<double>& fieldValues, const std::string& fieldName) {
	cg_field_write(this->fileIndex, this->baseIndex, this->zoneIndex, this->solutionIndex, RealDouble, fieldName.c_str(), &fieldValues[0], &this->fieldIndex);
}

void CgnsWriter::writeTimeStep(const double& timeInstant) {
	this->timeInstants.push_back(timeInstant);
	this->solutionIndices.emplace_back(0);
	std::string solutionName = std::string("TimeStep") + std::to_string(timeInstants.size());
	cg_sol_write(this->fileIndex, this->baseIndex, this->zoneIndex, solutionName.c_str(), Vertex, &this->solutionIndices.back());
}

void CgnsWriter::writeTransientField(const std::vector<double>& fieldValues, const std::string& fieldName) {
	this->fieldsIndices.emplace_back(0);
	cg_field_write(this->fileIndex, this->baseIndex, this->zoneIndex, this->solutionIndices.back(), RealDouble, fieldName.c_str(), &fieldValues[0], &this->fieldsIndices.back());
}

void CgnsWriter::finalizeTransient() {
	if (!this->isFinalized) {
		this->isFinalized = true;
		if (this->timeInstants.size() > 0) {
 			int numberOfTimeSteps = this->timeInstants.size();
 		    cg_biter_write(this->fileIndex, this->baseIndex, "TimeIterativeValues", this->timeInstants.size());
 		    cg_goto(this->fileIndex, this->baseIndex, "BaseIterativeData_t", 1, nullptr);
			cg_array_write("TimeValues", RealDouble, 1, &numberOfTimeSteps, &this->timeInstants[0]);
   	 		cg_simulation_type_write(this->fileIndex, this->baseIndex, TimeAccurate);
		}
		cg_close(this->fileIndex);
	}
}

CgnsWriter::~CgnsWriter() {
	this->finalizeTransient();
}