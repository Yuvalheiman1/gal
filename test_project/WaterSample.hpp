#ifndef WATERSAMPLE_HPP
#define WATERSAMPLE_HPP

#include <string>

class WaterSample {
public:
    // Constructor
    WaterSample(const std::string& location, const std::string& pollutant, double level, 
                const std::string& unit, const std::string& complianceStatus, const std::string& sampleDate);

    // Getters
    int getYear() const;
    const std::string& getLocation() const;
    const std::string& getPollutant() const;
    double getLevel() const;
    const std::string& getUnit() const;
    const std::string& getComplianceStatus() const;
    const std::string& getSampleDate() const;

    // Setters (optional, if modification is needed)
    void setLocation(const std::string& location);
    void setPollutant(const std::string& pollutant);
    void setLevel(double level);
    void setUnit(const std::string& unit);
    void setComplianceStatus(const std::string& complianceStatus); // Use const reference
    void setSampleDate(const std::string& sampleDate);

private:
    // Member variables
    std::string location;
    std::string pollutant;
    double level;
    std::string unit;
    std::string complianceStatus;
    std::string sampleDate;
};

#endif // WATERSAMPLE_HPP
