#include "ComplianceDashboard.hpp"
#include "dataset.hpp"
#include "WaterSample.hpp"
#include "PollutantSample.hpp"
#include "csv.hpp"
#include <iostream>
#include <string>

ComplianceDashboard::ComplianceDashboard(QWidget *parent) : QMainWindow(parent) {
    setupUI();
    populateTable("Y-2024-M.csv");
}

ComplianceDashboard::~ComplianceDashboard() {}

void ComplianceDashboard::setupUI() {
    // Central Widget
    centralWidget = new QWidget(this);
    mainLayout = new QVBoxLayout();

    // Header
    header = new QLabel("Compliance Dashboard");
    header->setAlignment(Qt::AlignCenter);
    header->setStyleSheet("font-size: 18px; font-weight: bold;");
    mainLayout->addWidget(header);

    // Filters Section
    filtersLayout = new QHBoxLayout();

    yearFilter = new QComboBox();
    yearFilter->addItems({"All Years", "2020", "2021", "2022", "2023", "2024"});
    yearFilter->setCurrentIndex(5);

    csv::CSVReader reader1("Locations.csv");
    locationFilter = new QComboBox();
    locationFilter->addItems({"All Locations"});

    for (const auto& row : reader1) {
        locationFilter->addItems({QString::fromStdString(row["Location"].get<>())});
    }

    pollutantFilter = new QComboBox();
    pollutantFilter->addItem("All Pollutants");

    WaterDataset dataset;
    std::vector<PollutantSample> pollutantSamples = dataset.loadPollutantSamples("pollutants.csv", 10);

    for (const auto& sample : pollutantSamples) {
        pollutantFilter->addItem(QString::fromStdString(sample.getName()));
    }

    statusFilter = new QComboBox();
    statusFilter->addItems({"All Statuses", "good", "medium", "bad"});

    filterButton = new QPushButton("Filter");

    filtersLayout->addWidget(yearFilter);
    filtersLayout->addWidget(locationFilter);
    filtersLayout->addWidget(pollutantFilter);
    filtersLayout->addWidget(statusFilter);
    filtersLayout->addWidget(filterButton);

    mainLayout->addLayout(filtersLayout);

    // Main Content Area
    contentLayout = new QHBoxLayout();

    // Detailed Table
    detailedTable = new QTableWidget(0, 6); // Start with 0 rows, 5 columns
    detailedTable->setHorizontalHeaderLabels({"Location", "Pollutant", "Level", "Unit", "Compliance", "Date"});
    detailedTable->setMinimumSize(600, 300); // Keep table smaller
    contentLayout->addWidget(detailedTable, 2);

    // Summary of Important Info
    importantInfo = new QTextEdit();
    importantInfo->setPlaceholderText("Summary of important information...");
    importantInfo->setReadOnly(true);
    importantInfo->setMinimumSize(200, 300);
    contentLayout->addWidget(importantInfo, 1);

    mainLayout->addLayout(contentLayout);

    // Move Summary Cards to Bottom
    cardsLayout = new QHBoxLayout();

    for (int i = 0; i < 4; ++i) { // Only first 4 rows
        summaryCards[i] = new QFrame();
        summaryCards[i]->setFrameShape(QFrame::StyledPanel);
        summaryCards[i]->setStyleSheet("background-color: #f2f2f2; border: 1px solid #d9d9d9; padding: 10px;");
        summaryCards[i]->setMinimumHeight(200);
        summaryCards[i]->setMinimumWidth(400);

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

        summaryCards[i]->setLayout(cardLayout);
        cardsLayout->addWidget(summaryCards[i]);
    }
    mainLayout->addLayout(cardsLayout); // Add summary cards layout to the bottom

    // Footer
    footer = new QLabel("Data provided by UK Environmental Agency.");
    footer->setAlignment(Qt::AlignCenter);
    footer->setStyleSheet("font-size: 12px; color: gray;");
    mainLayout->addWidget(footer);

    // Set Layout
    centralWidget->setLayout(mainLayout);
    setCentralWidget(centralWidget);
    resize(1200, 800); // Adjust window size to fit larger cards

    // Connect filter button to applyFilters
    connect(filterButton, &QPushButton::clicked, this, &ComplianceDashboard::applyFilters);
}


std::string ComplianceDashboard::calculateComplianceStatus(const WaterSample& sample, const std::vector<PollutantSample>& pollutantSamples) {
    for (int j = 0; j < pollutantSamples.size(); ++j) {
        if (sample.getPollutant() == pollutantSamples[j].getName()) {
            double sampleLevel = sample.getLevel();
            double minThreshold = std::stod(pollutantSamples[j].getMinThreshold());
            double maxThreshold = std::stod(pollutantSamples[j].getMaxThreshold());
            double thresholdRange = maxThreshold - minThreshold;

            if (sampleLevel >= minThreshold && sampleLevel <= maxThreshold) {
                return "good";
            }
            else if (sampleLevel >= minThreshold - 0.2 * thresholdRange && sampleLevel <= maxThreshold + 0.2 * thresholdRange) {
                return "medium";
            }
            else {
                return "bad";
            }
        }
    }
    return "-"; // Return "-" if pollutant is not found in the list
}

void ComplianceDashboard::populateStats(const std::string& bestLocation, const std::string& worstLocation,
                                         const std::string& bestYear, const std::string& worstYear,
                                         const std::string& bestPollutant, const std::string& worstPollutant,
                                         int totalSamples, int missingDataCount, int compliantSamples,
                                         int mediumSamples, int nonCompliantSamples) {

    // Only show these stats if the selected filter is "All Categories"
    QString selectedLocation = locationFilter->currentText();
    QString selectedYear = yearFilter->currentText();
    QString selectedPollutant = pollutantFilter->currentText();

    // Show location stats only if "All Locations" is selected
    if (selectedLocation == "All Locations") {
        importantInfo->append(QString("<font color='green'>Highest-performing location: %1</font>")
                               .arg(QString::fromStdString(bestLocation)));
        importantInfo->append(QString("<font color='red'>Lowest-performing location: %1</font>")
                               .arg(QString::fromStdString(worstLocation)));
    }

    // Show year stats only if "All Years" is selected
    if (selectedYear == "All Years") {
        importantInfo->append(QString("<font color='green'>Highest-performing year: %1</font>")
                               .arg(QString::fromStdString(bestYear)));
        importantInfo->append(QString("<font color='red'>Lowest-performing year: %1</font>")
                               .arg(QString::fromStdString(worstYear)));
    }

    // Show pollutant stats only if "All Pollutants" is selected
    if (selectedPollutant == "All Pollutants") {
        importantInfo->append(QString("<font color='green'>Most compliant pollutant: %1</font>")
                               .arg(QString::fromStdString(bestPollutant)));
        importantInfo->append(QString("<font color='red'>Least compliant pollutant: %1</font>")
                               .arg(QString::fromStdString(worstPollutant)));
    }

    // Show general sample statistics
    importantInfo->append(QString("<font color='black'>\n\nSample Statistics:</font>"));
    importantInfo->append(QString("<font color='black'>\n- Total number of samples: %1</font>").arg(totalSamples));
    importantInfo->append(QString("<font color='red'>\n- Samples with missing data: %1</font>").arg(missingDataCount));
    importantInfo->append(QString("<font color='green'>\n- Compliant samples: %1</font>").arg(compliantSamples));
    importantInfo->append(QString("<font color='orange'>\n- Samples with medium compliance: %1</font>").arg(mediumSamples));
    importantInfo->append(QString("<font color='red'>\n- Non-compliant samples: %1</font>").arg(nonCompliantSamples));
}


void ComplianceDashboard::populateTable(const std::string& filename) {
    WaterDataset dataset;
    dataset.loadData(filename);

    std::vector<PollutantSample> pollutantSamples = dataset.loadPollutantSamples("pollutants.csv", 10);
    auto& samples = dataset.getData();
    if (samples.empty()) {
        // Display a pop-up if the dataset is empty
        QMessageBox::warning(this, "No Data Found", "There is no data available in the file. Please download it from - <a href='https://environment.data.gov.uk/water-quality/view/download'>this link</a>");
        return;
    }

    detailedTable->setRowCount(samples.size());
    detailedTable->setColumnCount(6); // Number of columns
    detailedTable->setHorizontalHeaderLabels({"Location", "Pollutant", "Level", "Unit", "Date", "Compliance"});

    int totalSamples = 0, missingDataCount = 0, compliantSamples = 0, mediumSamples = 0, nonCompliantSamples = 0;
    std::string bestLocation, worstLocation, bestYear, worstYear, bestPollutant, worstPollutant;
    double bestLocationPerformance = -1, worstLocationPerformance = std::numeric_limits<double>::max();
    double bestYearPerformance = -1, worstYearPerformance = std::numeric_limits<double>::max();
    double bestPollutantPerformance = -1, worstPollutantPerformance = std::numeric_limits<double>::max();

    int row = 0;
    for (const auto& sample : samples) {
        std::string complianceStatus = calculateComplianceStatus(sample, pollutantSamples);

        // Update sample statistics
        totalSamples++;
        if (complianceStatus == "good") {
            compliantSamples++;
        } else if (complianceStatus == "medium") {
            mediumSamples++;
        } else if (complianceStatus == "bad") {
            nonCompliantSamples++;
        }

        if (complianceStatus == "-") {
            missingDataCount++;
        }

        // Track the best and worst locations, years, and pollutants
        double samplePerformance = sample.getLevel();  // Assuming performance is based on level for simplicity

        // Update Location Stats
        if (complianceStatus != "-") {
            if (samplePerformance > bestLocationPerformance) {
                bestLocation = sample.getLocation();
                bestLocationPerformance = samplePerformance;
            }
            if (samplePerformance < worstLocationPerformance) {
                worstLocation = sample.getLocation();
                worstLocationPerformance = samplePerformance;
            }
        }

        // Update Year Stats (Assuming year is a part of sample's metadata)
        std::string sampleYear = std::to_string(sample.getYear());
        if (complianceStatus != "-") {
            if (samplePerformance > bestYearPerformance) {
                bestYear = sampleYear;
                bestYearPerformance = samplePerformance;
            }
            if (samplePerformance < worstYearPerformance) {
                worstYear = sampleYear;
                worstYearPerformance = samplePerformance;
            }
        }

        // Update Pollutant Stats
        for (int j = 0; j < pollutantSamples.size(); ++j) {
            if (sample.getPollutant() == pollutantSamples[j].getName()) {
                double pollutantPerformance = samplePerformance; // Using same logic for pollutant as location
                if (complianceStatus != "-") {
                    if (pollutantPerformance > bestPollutantPerformance) {
                        bestPollutant = sample.getPollutant();
                        bestPollutantPerformance = pollutantPerformance;
                    }
                    if (pollutantPerformance < worstPollutantPerformance) {
                        worstPollutant = sample.getPollutant();
                        worstPollutantPerformance = pollutantPerformance;
                    }
                }
            }
        }

        // Add the sample data to the table
        detailedTable->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(sample.getLocation())));
        detailedTable->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(sample.getPollutant())));
        detailedTable->setItem(row, 2, new QTableWidgetItem(QString::number(sample.getLevel())));
        detailedTable->setItem(row, 3, new QTableWidgetItem(QString::fromStdString(sample.getUnit())));
        detailedTable->setItem(row, 4, new QTableWidgetItem(QString::fromStdString(sample.getSampleDate())));
        detailedTable->setItem(row, 5, new QTableWidgetItem(QString::fromStdString(complianceStatus)));

        // Apply background color based on compliance status, except if it's "-"
        if (complianceStatus != "-") {
            QColor rowColor;
            if (complianceStatus == "good") {
                rowColor = QColor(0, 255, 0); // Green for "good"
            } else if (complianceStatus == "medium") {
                rowColor = QColor(255, 165, 0); // Orange for "medium"
            } else {
                rowColor = QColor(255, 0, 0); // Red for "bad"
            }

            for (int column = 0; column < detailedTable->columnCount(); ++column) {
                detailedTable->item(row, column)->setBackground(rowColor);
            }
        }

        row++;
    }

    // Call populateStats to display the calculated statistics
    populateStats(bestLocation, worstLocation, bestYear, worstYear, bestPollutant, worstPollutant,
                  totalSamples, missingDataCount, compliantSamples, mediumSamples, nonCompliantSamples);
}


void ComplianceDashboard::applyFilters() {
    // Clear the important information box before applying new filters
    importantInfo->clear();

    // Retrieve filter criteria
    QString selectedYear = yearFilter->currentText();
    QString selectedLocation = locationFilter->currentText();
    QString selectedPollutant = pollutantFilter->currentText();
    QString selectedStatus = statusFilter->currentText();

    // Load the dataset
    WaterDataset dataset;
    std::vector<PollutantSample> pollutantSamples = dataset.loadPollutantSamples("pollutants.csv", 10);
    if (selectedYear == "All Years") {
        // Load data for all years
        for (int year = 2020; year <= 2024; ++year) {
            std::string yearFile = "Y-" + std::to_string(year) + "-M.csv";
            try {
                WaterDataset tempDataset;
                tempDataset.loadData(yearFile);
                dataset.appendData(tempDataset.getData());
            } catch (const std::exception& e) {
                continue;  // Skip year file if loading fails
            }
        }
    } else {
        std::string newDate = "Y-" + selectedYear.toStdString() + "-M.csv";
        try {
            dataset.loadData(newDate);  // Load data for the selected year
        } catch (const std::exception& e) {
            QMessageBox::warning(this, "Data Error", "There is no data available for the selected year. Please try again.");
            return;
        }
    }

    // Filter samples based on the filters applied by the user
    const auto& samples = dataset.getData();
    detailedTable->setRowCount(0);  // Clear the table for new filtered results

    if (samples.empty()) {
        QMessageBox::information(this, "No Results", "No data matches the selected filters.");
        return;
    }

    // Initialize statistics counters
    int totalSamples = 0, missingDataCount = 0, compliantSamples = 0, mediumSamples = 0, nonCompliantSamples = 0;
    std::string bestLocation, worstLocation, bestYear, worstYear, bestPollutant, worstPollutant;
    double bestLocationPerformance = -1, worstLocationPerformance = std::numeric_limits<double>::max();
    double bestYearPerformance = -1, worstYearPerformance = std::numeric_limits<double>::max();
    double bestPollutantPerformance = -1, worstPollutantPerformance = std::numeric_limits<double>::max();

    int row = 0;
    for (const auto& sample : samples) {
        // Apply Year Filter
        if (selectedYear != "All Years" && sample.getYear() != std::stoi(selectedYear.toStdString())) continue;
        // Apply Location Filter
        if (selectedLocation != "All Locations" && sample.getLocation() != selectedLocation.toStdString()) continue;
        // Apply Pollutant Filter
        if (selectedPollutant != "All Pollutants" && sample.getPollutant() != selectedPollutant.toStdString()) continue;
        // Apply Status Filter
        std::string complianceStatus = calculateComplianceStatus(sample, pollutantSamples);
        if (selectedStatus != "All Statuses" && complianceStatus != selectedStatus.toStdString()) continue;

        // Update sample statistics for the filtered samples
        totalSamples++;
        if (complianceStatus == "good") {
            compliantSamples++;
        } else if (complianceStatus == "medium") {
            mediumSamples++;
        } else if (complianceStatus == "bad") {
            nonCompliantSamples++;
        }

        if (complianceStatus == "-") {
            missingDataCount++;
        }

        // Track the best and worst locations, years, and pollutants
        double samplePerformance = sample.getLevel();  // Assuming performance is based on level for simplicity

        // Update Location Stats
        if (complianceStatus != "-") {
            if (samplePerformance > bestLocationPerformance) {
                bestLocation = sample.getLocation();
                bestLocationPerformance = samplePerformance;
            }
            if (samplePerformance < worstLocationPerformance) {
                worstLocation = sample.getLocation();
                worstLocationPerformance = samplePerformance;
            }
        }

        // Update Year Stats
        std::string sampleYear = std::to_string(sample.getYear());
        if (complianceStatus != "-") {
            if (samplePerformance > bestYearPerformance) {
                bestYear = sampleYear;
                bestYearPerformance = samplePerformance;
            }
            if (samplePerformance < worstYearPerformance) {
                worstYear = sampleYear;
                worstYearPerformance = samplePerformance;
            }
        }

        // Update Pollutant Stats
        for (int j = 0; j < pollutantSamples.size(); ++j) {
            if (sample.getPollutant() == pollutantSamples[j].getName()) {
                double pollutantPerformance = samplePerformance;
                if (complianceStatus != "-") {
                    if (pollutantPerformance > bestPollutantPerformance) {
                        bestPollutant = sample.getPollutant();
                        bestPollutantPerformance = pollutantPerformance;
                    }
                    if (pollutantPerformance < worstPollutantPerformance) {
                        worstPollutant = sample.getPollutant();
                        worstPollutantPerformance = pollutantPerformance;
                    }
                }
            }
        }

        // Insert the row in the table if the sample passed the filters
        detailedTable->insertRow(row);
        detailedTable->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(sample.getLocation())));
        detailedTable->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(sample.getPollutant())));
        detailedTable->setItem(row, 2, new QTableWidgetItem(QString::number(sample.getLevel())));
        detailedTable->setItem(row, 3, new QTableWidgetItem(QString::fromStdString(sample.getUnit())));
        detailedTable->setItem(row, 4, new QTableWidgetItem(QString::fromStdString(sample.getSampleDate())));
        detailedTable->setItem(row, 5, new QTableWidgetItem(QString::fromStdString(complianceStatus)));

        // Apply background color based on the compliance status
        if (complianceStatus != "-") {
            QColor rowColor;
            if (complianceStatus == "good") {
                rowColor = QColor(0, 255, 0); // Green for "good"
            } else if (complianceStatus == "medium") {
                rowColor = QColor(255, 165, 0); // Orange for "medium"
            } else {
                rowColor = QColor(255, 0, 0); // Red for "bad"
            }

            // Set the row background color
            for (int column = 0; column < detailedTable->columnCount(); ++column) {
                detailedTable->item(row, column)->setBackground(rowColor);
            }
        }

        row++;
    }

    // Call populateStats to display the calculated statistics
    populateStats(bestLocation, worstLocation, bestYear, worstYear, bestPollutant, worstPollutant,
                  totalSamples, missingDataCount, compliantSamples, mediumSamples, nonCompliantSamples);
}
