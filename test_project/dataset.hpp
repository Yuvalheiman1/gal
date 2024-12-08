#ifndef WATERDATASET_HPP
#define WATERDATASET_HPP

#include <vector>
#include <string>
#include "WaterSample.hpp"
#include "PollutantSample.hpp"

class WaterDataset {
public:
    void loadData(const std::string& filename);
    std::vector<WaterSample>& getData();
    void appendData(const std::vector<WaterSample>& newSamples);
    std::vector<PollutantSample> loadPollutantSamples(const std::string& filename, int rowCount = 10);

private:
    std::vector<WaterSample> data;
    std::vector<PollutantSample> PollutantData;
    void checkDataExists() const;
};

#endif // WATERDATASET_HPP
