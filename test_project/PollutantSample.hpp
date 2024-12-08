#ifndef POLLUTANTSAMPLE_HPP
#define POLLUTANTSAMPLE_HPP

#include <string>

class PollutantSample {
public:
    // Constructor
    PollutantSample(const std::string& name, const std::string& unit, const std::string& minThreshold, 
                     const std::string& maxThreshold, const std::string& info);

    // Getters
    const std::string& getName() const;
    const std::string& getUnit() const;
    const std::string& getMinThreshold() const;
    const std::string& getMaxThreshold() const;
    const std::string& getInfo() const;

private:
    // Member variables
    std::string Name;
    std::string Unit;
    std::string MinThreshold;
    std::string MaxThreshold;
    std::string Info; // Added field for the sample date
};

#endif 
