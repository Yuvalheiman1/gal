#include "PollutantSample.hpp"

// Constructor
PollutantSample::PollutantSample(const std::string& name, const std::string& unit, const std::string& minThreshold, 
                const std::string& maxThreshold, const std::string& info)
    : Name(name), Unit(unit), MinThreshold(minThreshold), MaxThreshold(maxThreshold), Info(info) {}

// Getters
const std::string& PollutantSample::getName() const {
    return Name;
}

const std::string& PollutantSample::getUnit() const {
    return Unit;
}

const std::string& PollutantSample::getMinThreshold() const {
    return MinThreshold;
}

const std::string& PollutantSample::getMaxThreshold() const {
    return MaxThreshold;
}

const std::string& PollutantSample::getInfo() const {
    return Info;
}
