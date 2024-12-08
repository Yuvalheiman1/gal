#include "WaterSample.hpp"

// Constructor
WaterSample::WaterSample(const std::string& location, const std::string& pollutant, double level, 
                         const std::string& unit, const std::string& complianceStatus, const std::string& sampleDate)
    : location(location), pollutant(pollutant), level(level), unit(unit), complianceStatus(complianceStatus), sampleDate(sampleDate) {}

// Getters
int WaterSample::getYear() const {
    // Extract year from the sampleDate (assumed to be in "YYYY-MM-DD" format)
    return std::stoi(sampleDate.substr(0, 4));
}

const std::string& WaterSample::getLocation() const {
    return location;
}

const std::string& WaterSample::getPollutant() const {
    return pollutant;
}

double WaterSample::getLevel() const {
    return level;
}

const std::string& WaterSample::getUnit() const {
    return unit;
}

const std::string& WaterSample::getComplianceStatus() const {
    return complianceStatus;  // Return const reference
}

const std::string& WaterSample::getSampleDate() const {
    return sampleDate;
}

// Setters
void WaterSample::setLocation(const std::string& location) {
    this->location = location;
}

void WaterSample::setPollutant(const std::string& pollutant) {
    this->pollutant = pollutant;
}

void WaterSample::setLevel(double level) {
    this->level = level;
}

void WaterSample::setUnit(const std::string& unit) {
    this->unit = unit;
}

void WaterSample::setComplianceStatus(const std::string& complianceStatus) {
    this->complianceStatus = complianceStatus;  // Accept const reference
}

void WaterSample::setSampleDate(const std::string& sampleDate) {
    this->sampleDate = sampleDate;
}
