#ifndef COMPLIANCEDASHBOARD_HPP
#define COMPLIANCEDASHBOARD_HPP

#include <QMainWindow>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTableWidget>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include <QTextEdit>

#include <string>
#include <vector>
#include "WaterSample.hpp"
#include "PollutantSample.hpp"

class ComplianceDashboard : public QMainWindow {
    Q_OBJECT

public:
    explicit ComplianceDashboard(QWidget *parent = nullptr);
    ~ComplianceDashboard();

private:
    // UI Elements
    QWidget *rootWidget; // Root widget for the main window
    QVBoxLayout *mainLayout; // Main vertical layout for the dashboard
    QHBoxLayout *filterBar; // Layout for filter section
    QHBoxLayout *contentArea; // Layout for the table and stats area

    QLabel *pageTitle; // Header for the page
    QTableWidget *complianceTable; // Table widget for compliance data
    QTextEdit *statsBox; // Text area for displaying statistics

    // Filters
    QComboBox *yearDropdown; // Dropdown for selecting year
    QComboBox *locationDropdown; // Dropdown for selecting location
    QComboBox *pollutantDropdown; // Dropdown for selecting pollutant
    QComboBox *complianceDropdown; // Dropdown for selecting compliance status
    QPushButton *applyButton; // Button to apply filters

    QLabel *footerLabel; // Footer text at the bottom of the dashboard

    // Helper Functions
    std::string evaluateCompliance(const WaterSample& sample, const std::vector<PollutantSample>& pollutants);

    // Core Functionalities
    void initializeUI(); // Function to set up the UI
    void loadDataToTable(const std::string& filename); // Function to populate the table with data
    void updateStatistics(int totalSamples, int unknownSamples, int compliant, int moderate, int nonCompliant); // Update stats box

private slots:
    void applyFilters(); // Function to handle the filter application logic
};

#endif // COMPLIANCEDASHBOARD_HPP
