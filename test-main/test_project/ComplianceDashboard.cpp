#include "ComplianceDashboard.hpp"
#include "dataset.hpp"
#include "WaterSample.hpp"
#include "PollutantSample.hpp"
#include "csv.hpp"
#include <iostream>
#include <string>

ComplianceDashboard::ComplianceDashboard(QWidget *parent) : QMainWindow(parent) {
    initializeUI();
    loadTableData("Y-2024-M.csv");
}

ComplianceDashboard::~ComplianceDashboard() {}

void ComplianceDashboard::initializeUI() {
    // Central Widget
    mainWidget = new QWidget(this);
    layoutMain = new QVBoxLayout();

    // Header
    headerText = new QLabel("Compliance Dashboard");
    headerText->setAlignment(Qt::AlignCenter);
    headerText->setStyleSheet("font-size: 18px; font-weight: bold;");
    layoutMain->addWidget(headerText);

    // Filters Section
    layoutFilters = new QHBoxLayout();

    filterYear = new QComboBox();
    filterYear->addItems({"All Years", "2020", "2021", "2022", "2023", "2024"});
    filterYear->setCurrentIndex(5);

    csv::CSVReader reader1("Locations.csv");
    filterLocation = new QComboBox();
    filterLocation->addItems({"All Locations"});

    for (const auto& row : reader1) {
        filterLocation->addItems({QString::fromStdString(row["Location"].get<>())});
    }

    filterPollutant = new QComboBox();
    filterPollutant->addItem("All Pollutants");

    WaterDataset dataset;
    std::vector<PollutantSample> pollutantSamples = dataset.loadPollutantSamples("pollutants.csv", 10);

    for (const auto& sample : pollutantSamples) {
        filterPollutant->addItem(QString::fromStdString(sample.getName()));
    }

    filterStatus = new QComboBox();
    filterStatus->addItems({"All Statuses", "good", "medium", "bad"});

    applyFilterButton = new QPushButton("Filter");

    layoutFilters->addWidget(filterYear);
    layoutFilters->addWidget(filterLocation);
    layoutFilters->addWidget(filterPollutant);
    layoutFilters->addWidget(filterStatus);
    layoutFilters->addWidget(applyFilterButton);

    layoutMain->addLayout(layoutFilters);

    // Main Content Area
    layoutContent = new QHBoxLayout();

    // Detailed Table
    dataTable = new QTableWidget(0, 6); // Start with 0 rows, 6 columns
    dataTable->setHorizontalHeaderLabels({"Location", "Pollutant", "Level", "Unit", "Compliance", "Date"});
    dataTable->setMinimumSize(600, 300);
    layoutContent->addWidget(dataTable, 2);

    // Summary of Important Info
    infoBox = new QTextEdit();
    infoBox->setPlaceholderText("Summary of important information...");
    infoBox->setReadOnly(true);
    infoBox->setMinimumSize(200, 300);
    layoutContent->addWidget(infoBox, 1);

    layoutMain->addLayout(layoutContent);

    // Summary Cards
    layoutCards = new QHBoxLayout();

    for (int i = 0; i < 4; ++i) {
        summaryFrames[i] = new QFrame();
        summaryFrames[i]->setFrameShape(QFrame::StyledPanel);
        summaryFrames[i]->setStyleSheet("background-color: #f2f2f2; border: 1px solid #d9d9d9; padding: 10px;");
        summaryFrames[i]->setMinimumHeight(200);
        summaryFrames[i]->setMinimumWidth(400);

        QVBoxLayout *cardLayout = new QVBoxLayout();

        QLabel *cardTitle = new QLabel();
        QLabel *cardDetails = new QLabel();

        if (i < pollutantSamples.size()) {
            const PollutantSample& sample = pollutantSamples[i];
            cardTitle->setText(QString::fromStdString(sample.getName()));
            QString details = QString("Unit: %1\nMin Threshold: %2\nMax Threshold: %3\nInfo: %4")
                                    .arg(QString::fromStdString(sample.getUnit()))
                                    .arg(QString::fromStdString(sample.getMinThreshold()))
                                    .arg(QString::fromStdString(sample.getMaxThreshold()))
                                    .arg(QString::fromStdString(sample.getInfo()));
            cardDetails->setText(details);
        } else {
            cardTitle->setText("No Data");
            cardDetails->setText("No additional information available.");
        }

        cardTitle->setAlignment(Qt::AlignCenter);
        cardDetails->setAlignment(Qt::AlignLeft);
        cardLayout->addWidget(cardTitle);
        cardLayout->addWidget(cardDetails);

        summaryFrames[i]->setLayout(cardLayout);
        layoutCards->addWidget(summaryFrames[i]);
    }
    layoutMain->addLayout(layoutCards);

    // Footer
    footerText = new QLabel("Data provided by UK Environmental Agency.");
    footerText->setAlignment(Qt::AlignCenter);
    footerText->setStyleSheet("font-size: 12px; color: gray;");
    layoutMain->addWidget(footerText);

    // Set Layout
    mainWidget->setLayout(layoutMain);
    setCentralWidget(mainWidget);
    resize(1200, 800);

    // Connect filter button to applySearchFilters
    connect(applyFilterButton, &QPushButton::clicked, this, &ComplianceDashboard::applySearchFilters);
}

void ComplianceDashboard::loadTableData(const std::string& filePath) {
    WaterDataset dataset;
    dataset.loadData(filePath);

    std::vector<PollutantSample> pollutantSamples = dataset.loadPollutantSamples("pollutants.csv", 10);
    auto& samples = dataset.getData();

    if (samples.empty()) {
        QMessageBox::warning(this, "No Data Found",
                             "There is no data available in the file. Please download it from - <a href='https://environment.data.gov.uk/water-quality/view/download'>this link</a>");
        return;
    }

    dataTable->setRowCount(samples.size());
    dataTable->setColumnCount(6); // Number of columns
    dataTable->setHorizontalHeaderLabels({"Location", "Pollutant", "Level", "Unit", "Date", "Compliance"});

    int row = 0;
    for (const auto& sample : samples) {
        std::string complianceStatus = assessPerformanceStatus(sample, pollutantSamples);

        auto *itemLocation = new QTableWidgetItem(QString::fromStdString(sample.getLocation()));
        auto *itemPollutant = new QTableWidgetItem(QString::fromStdString(sample.getPollutant()));
        auto *itemLevel = new QTableWidgetItem(QString::number(sample.getLevel()));
        auto *itemUnit = new QTableWidgetItem(QString::fromStdString(sample.getUnit()));
        auto *itemDate = new QTableWidgetItem(QString::fromStdString(sample.getSampleDate()));
        auto *itemCompliance = new QTableWidgetItem(QString::fromStdString(complianceStatus));

        // Set background color based on compliance status
        QColor rowColor;
        if (complianceStatus == "good") {
            rowColor = QColor(0, 255, 0); // Green
        } else if (complianceStatus == "medium") {
            rowColor = QColor(255, 165, 0); // Orange
        } else if (complianceStatus == "bad") {
            rowColor = QColor(255, 0, 0); // Red
        } else {
            rowColor = QColor(255, 255, 255); // White for missing data
        }

        // Apply background color to each item
        itemLocation->setBackground(rowColor);
        itemPollutant->setBackground(rowColor);
        itemLevel->setBackground(rowColor);
        itemUnit->setBackground(rowColor);
        itemDate->setBackground(rowColor);
        itemCompliance->setBackground(rowColor);

        // Add items to the table
        dataTable->setItem(row, 0, itemLocation);
        dataTable->setItem(row, 1, itemPollutant);
        dataTable->setItem(row, 2, itemLevel);
        dataTable->setItem(row, 3, itemUnit);
        dataTable->setItem(row, 4, itemDate);
        dataTable->setItem(row, 5, itemCompliance);

        row++;
    }
}


void ComplianceDashboard::applySearchFilters() {
    QString selectedYear = filterYear->currentText();
    QString selectedLocation = filterLocation->currentText();
    QString selectedPollutant = filterPollutant->currentText();
    QString selectedStatus = filterStatus->currentText();

    WaterDataset dataset;
    std::vector<PollutantSample> pollutantSamples = dataset.loadPollutantSamples("pollutants.csv", 10);

    if (selectedYear == "All Years") {
        for (int year = 2020; year <= 2024; ++year) {
            try {
                WaterDataset tempDataset;
                tempDataset.loadData("Y-" + std::to_string(year) + "-M.csv");
                dataset.appendData(tempDataset.getData());
            } catch (...) {
                continue; // Ignore missing files
            }
        }
    } else {
        dataset.loadData("Y-" + selectedYear.toStdString() + "-M.csv");
    }

    const auto& samples = dataset.getData();
    dataTable->setRowCount(0);

    for (const auto& sample : samples) {
        if (selectedLocation != "All Locations" && sample.getLocation() != selectedLocation.toStdString())
            continue;
        if (selectedPollutant != "All Pollutants" && sample.getPollutant() != selectedPollutant.toStdString())
            continue;
        std::string complianceStatus = assessPerformanceStatus(sample, pollutantSamples);
        if (selectedStatus != "All Statuses" && complianceStatus != selectedStatus.toStdString())
            continue;

        int row = dataTable->rowCount();
        dataTable->insertRow(row);

        auto *itemLocation = new QTableWidgetItem(QString::fromStdString(sample.getLocation()));
        auto *itemPollutant = new QTableWidgetItem(QString::fromStdString(sample.getPollutant()));
        auto *itemLevel = new QTableWidgetItem(QString::number(sample.getLevel()));
        auto *itemUnit = new QTableWidgetItem(QString::fromStdString(sample.getUnit()));
        auto *itemDate = new QTableWidgetItem(QString::fromStdString(sample.getSampleDate()));
        auto *itemCompliance = new QTableWidgetItem(QString::fromStdString(complianceStatus));

        // Set background color based on compliance status
        QColor rowColor;
        if (complianceStatus == "good") {
            rowColor = QColor(0, 255, 0); // Green
        } else if (complianceStatus == "medium") {
            rowColor = QColor(255, 165, 0); // Orange
        } else if (complianceStatus == "bad") {
            rowColor = QColor(255, 0, 0); // Red
        } else {
            rowColor = QColor(255, 255, 255); // White for missing data
        }

        itemLocation->setBackground(rowColor);
        itemPollutant->setBackground(rowColor);
        itemLevel->setBackground(rowColor);
        itemUnit->setBackground(rowColor);
        itemDate->setBackground(rowColor);
        itemCompliance->setBackground(rowColor);

        // Add items to the table
        dataTable->setItem(row, 0, itemLocation);
        dataTable->setItem(row, 1, itemPollutant);
        dataTable->setItem(row, 2, itemLevel);
        dataTable->setItem(row, 3, itemUnit);
        dataTable->setItem(row, 4, itemDate);
        dataTable->setItem(row, 5, itemCompliance);
    }
}


std::string ComplianceDashboard::assessPerformanceStatus(const WaterSample& sample, const std::vector<PollutantSample>& pollutantSamples) {
    for (const auto& pollutant : pollutantSamples) {
        if (sample.getPollutant() == pollutant.getName()) {
            double sampleLevel = sample.getLevel();
            double minThreshold = std::stod(pollutant.getMinThreshold());
            double maxThreshold = std::stod(pollutant.getMaxThreshold());
            double range = maxThreshold - minThreshold;

            if (sampleLevel >= minThreshold && sampleLevel <= maxThreshold)
                return "good";
            else if (sampleLevel >= minThreshold - 0.2 * range && sampleLevel <= maxThreshold + 0.2 * range)
                return "medium";
            else
                return "bad";
        }
    }
    return "-"; // Unknown pollutant
}


void ComplianceDashboard::displayStats(const std::string& topLocation, const std::string& bottomLocation,
                                       const std::string& topYear, const std::string& bottomYear,
                                       const std::string& topPollutant, const std::string& bottomPollutant,
                                       int totalEntries, int missingEntryCount, int compliantEntries,
                                       int averageEntries, int nonCompliantEntries) {
    // Implementation for displaying statistics
}
