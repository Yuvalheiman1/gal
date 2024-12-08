#include "ComplianceDashboard.hpp"
#include "dataset.hpp"
#include "WaterSample.hpp"
#include "PollutantSample.hpp"
#include "csv.hpp"
#include <iostream>
#include <string>

ComplianceDashboard::ComplianceDashboard(QWidget *parent) : QMainWindow(parent) {
    initializeUI();
    loadDataToTable("Y-2024-M.csv");
}

ComplianceDashboard::~ComplianceDashboard() {}

void ComplianceDashboard::initializeUI() {
    // Root Widget and Layout
    rootWidget = new QWidget(this);
    mainLayout = new QVBoxLayout();

    // Header Section
    pageTitle = new QLabel("Environmental Compliance Monitor");
    pageTitle->setAlignment(Qt::AlignCenter);
    pageTitle->setStyleSheet("font-size: 20px; font-weight: bold;");
    mainLayout->addWidget(pageTitle);

    // Filters Section
    filterBar = new QHBoxLayout();

    // Dropdown for Years
    yearDropdown = new QComboBox();
    yearDropdown->addItems({"All Timeframes", "2019", "2020", "2021", "2022", "2023", "2024"});
    yearDropdown->setCurrentIndex(6); // Default selection to the latest year

    // Dropdown for Locations
    csv::CSVReader locationData("Locations.csv");
    locationDropdown = new QComboBox();
    locationDropdown->addItems({"All Regions"});

    for (const auto& row : locationData) {
        locationDropdown->addItem(QString::fromStdString(row["Region"].get<>()));
    }

    // Dropdown for Pollutants
    pollutantDropdown = new QComboBox();
    pollutantDropdown->addItems({"All Substances"});

    WaterDataset dataset;
    std::vector<PollutantSample> contaminantList = dataset.loadPollutantSamples("pollutants.csv", 10);
    for (const auto& contaminant : contaminantList) {
        pollutantDropdown->addItem(QString::fromStdString(contaminant.getName()));
    }

    // Dropdown for Compliance Levels
    complianceDropdown = new QComboBox();
    complianceDropdown->addItems({"All Compliance Levels", "Fully Compliant", "Moderately Compliant", "Non-Compliant"});

    // Filter Button
    applyButton = new QPushButton("Apply Filters");

    // Add Filters to Filter Bar
    filterBar->addWidget(yearDropdown);
    filterBar->addWidget(locationDropdown);
    filterBar->addWidget(pollutantDropdown);
    filterBar->addWidget(complianceDropdown);
    filterBar->addWidget(applyButton);

    // Add Filter Bar to Main Layout
    mainLayout->addLayout(filterBar);

    // Table Section
    contentArea = new QHBoxLayout();

    // Compliance Table
    complianceTable = new QTableWidget(0, 6);
    complianceTable->setHorizontalHeaderLabels({"Region", "Substance", "Measurement", "Units", "Compliance", "Date"});
    complianceTable->setMinimumSize(650, 300);
    contentArea->addWidget(complianceTable, 2);

    // Stats Box
    statsBox = new QTextEdit();
    statsBox->setPlaceholderText("Key Statistics...");
    statsBox->setReadOnly(true);
    statsBox->setMinimumSize(250, 300);
    contentArea->addWidget(statsBox, 1);

    // Add Table and Stats to Main Layout
    mainLayout->addLayout(contentArea);

    // Footer Section
    footerLabel = new QLabel("Data courtesy of UK Environmental Agency.");
    footerLabel->setAlignment(Qt::AlignCenter);
    footerLabel->setStyleSheet("font-size: 12px; color: gray;");
    mainLayout->addWidget(footerLabel);

    // Apply Main Layout to Root Widget
    rootWidget->setLayout(mainLayout);
    setCentralWidget(rootWidget);

    resize(1300, 850); // Resize window for updated design

    // Connect filter button signal to applyFilter function
    connect(applyButton, &QPushButton::clicked, this, &ComplianceDashboard::applyFilters);
}

std::string ComplianceDashboard::evaluateCompliance(const WaterSample& sample, const std::vector<PollutantSample>& pollutants) {
    for (const auto& pollutant : pollutants) {
        if (sample.getPollutant() == pollutant.getName()) {
            double level = sample.getLevel();
            double minLimit = std::stod(pollutant.getMinThreshold());
            double maxLimit = std::stod(pollutant.getMaxThreshold());
            double range = maxLimit - minLimit;

            if (level >= minLimit && level <= maxLimit) return "Fully Compliant";
            if (level >= minLimit - 0.2 * range && level <= maxLimit + 0.2 * range) return "Moderately Compliant";
            return "Non-Compliant";
        }
    }
    return "Unknown"; // If no match is found
}

void ComplianceDashboard::loadDataToTable(const std::string& filename) {
    WaterDataset dataset;
    dataset.loadData(filename);

    auto& samples = dataset.getData();
    std::vector<PollutantSample> pollutants = dataset.loadPollutantSamples("pollutants.csv", 10);

    if (samples.empty()) {
        QMessageBox::warning(this, "No Data", "No data available. Please check your file or download the dataset.");
        return;
    }

    complianceTable->setRowCount(samples.size());

    int total = 0, unknown = 0, fullyCompliant = 0, moderatelyCompliant = 0, nonCompliant = 0;
    std::string bestRegion, worstRegion;
    int row = 0;

    for (const auto& sample : samples) {
        std::string compliance = evaluateCompliance(sample, pollutants);

        complianceTable->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(sample.getLocation())));
        complianceTable->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(sample.getPollutant())));
        complianceTable->setItem(row, 2, new QTableWidgetItem(QString::number(sample.getLevel())));
        complianceTable->setItem(row, 3, new QTableWidgetItem(QString::fromStdString(sample.getUnit())));
        complianceTable->setItem(row, 4, new QTableWidgetItem(QString::fromStdString(compliance)));
        complianceTable->setItem(row, 5, new QTableWidgetItem(QString::fromStdString(sample.getSampleDate())));

        QColor rowColor;
        if (compliance == "Fully Compliant") {
            rowColor = QColor(0, 255, 0); // Green
            fullyCompliant++;
        } else if (compliance == "Moderately Compliant") {
            rowColor = QColor(255, 165, 0); // Orange
            moderatelyCompliant++;
        } else if (compliance == "Non-Compliant") {
            rowColor = QColor(255, 0, 0); // Red
            nonCompliant++;
        } else {
            rowColor = QColor(200, 200, 200); // Gray for "Unknown"
            unknown++;
        }

        for (int col = 0; col < complianceTable->columnCount(); ++col) {
            complianceTable->item(row, col)->setBackground(rowColor);
        }
        row++;
    }

    // Update statistics
    statsBox->clear();
    statsBox->append(QString("Total Samples: %1").arg(total));
    statsBox->append(QString("Unknown Samples: %1").arg(unknown));
    statsBox->append(QString("Fully Compliant: %1").arg(fullyCompliant));
    statsBox->append(QString("Moderately Compliant: %1").arg(moderatelyCompliant));
    statsBox->append(QString("Non-Compliant: %1").arg(nonCompliant));
}

void ComplianceDashboard::applyFilters() {
    statsBox->clear();

    QString selectedYear = yearDropdown->currentText();
    QString selectedRegion = locationDropdown->currentText();
    QString selectedSubstance = pollutantDropdown->currentText();
    QString selectedCompliance = complianceDropdown->currentText();

    WaterDataset dataset;
    auto& samples = dataset.getData();
    detailedTable->setRowCount(0);

    int total = 0, fullyCompliant = 0, moderatelyCompliant = 0, nonCompliant = 0;
    for (const auto& sample : samples) {
        // Apply filtering logic here based on dropdown selections
        // Update table and statistics accordingly
    }
    statsBox->append("Filters applied.");
}
