#include "dataset.hpp"
#include "WaterSample.hpp"
#include "PollutantSample.hpp"
#include "csv.hpp"
#include <stdexcept>
#include <iostream>

void WaterDataset::loadData(const std::string& filename) {
    csv::CSVReader reader(filename);

    data.clear();

    for (const auto& row : reader) {
        try {
            double level = 0.0;
            if (!row["result"].is_null()) {
                level = row["result"].get<double>();
            }

            WaterSample sample(
            row["sample.samplingPoint.label"].get<>(),
            row["determinand.label"].get<>(),
            level,
            row["determinand.unit.label"].get<>(),
            row["sample.isComplianceSample"].get(),
            row["sample.sampleDateTime"].get<std::string>()
            
             );
            std::cout << "Created WaterSample: "
            << sample.getLocation() << ", "
            << sample.getPollutant() << ", "
            << sample.getLevel() << ", "
            << sample.getUnit() << ", "
            << sample.getComplianceStatus() << std::endl
            << sample.getSampleDate() << std::endl;

            data.push_back(sample);
        } catch (const std::exception& e) {
            std::cerr << "Error processing row: " << e.what() << std::endl;
            continue;
        }
    }
}

std::vector<WaterSample>& WaterDataset::getData() {
    checkDataExists();
    return data;
}

void WaterDataset::checkDataExists() const {
    if (data.empty()) {
        throw std::runtime_error("Dataset is empty or could not be loaded!");
    }
}

void WaterDataset::appendData(const std::vector<WaterSample>& newSamples) {
    data.insert(data.end(), newSamples.begin(), newSamples.end());
}

std::vector<PollutantSample> WaterDataset::loadPollutantSamples(const std::string& filename, int rowCount) {
    csv::CSVReader reader(filename);
    std::vector<PollutantSample> pollutantSamples;

    int counter = 0;
    for (const auto& row : reader) {
        if (counter >= rowCount) break;
            PollutantSample sample(
                row["Pollutant"].get<std::string>(),             // Assuming 'name' column exists
                row["Unit"].get<std::string>(),             // Assuming 'unit' column exists
                row["Min.Threshold"].get<std::string>(),     // Assuming 'minThreshold' column exists
                row["Max.Threshold"].get<std::string>(),     // Assuming 'maxThreshold' column exists
                row["Info"].get<std::string>()              // Assuming 'info' column exists
            );
            pollutantSamples.push_back(sample);
            counter++;
    }

    return pollutantSamples;
}