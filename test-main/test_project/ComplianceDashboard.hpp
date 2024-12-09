#ifndef COMPLIANCEDASHBOARD_HPP
#define COMPLIANCEDASHBOARD_HPP

#include <QMainWindow>
#include <QTableWidget>
#include <QComboBox>
#include <QPushButton>
#include <QTextEdit>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>

#include "WaterSample.hpp"
#include "PollutantSample.hpp"

class ComplianceDashboard : public QMainWindow {
    Q_OBJECT

public:
    ComplianceDashboard(QWidget *parent = nullptr);
    ~ComplianceDashboard();

private:
    void initializeUI();
    void loadTableData(const std::string& filePath);
    void applySearchFilters();

    // Updated method to calculate compliance status
    std::string assessPerformanceStatus(const WaterSample& sample, const std::vector<PollutantSample>& pollutantSamples);
    void displayStats(const std::string& topLocation, const std::string& bottomLocation,
                      const std::string& topYear, const std::string& bottomYear,
                      const std::string& topPollutant, const std::string& bottomPollutant,
                      int totalEntries, int missingEntryCount, int compliantEntries,
                      int averageEntries, int nonCompliantEntries);

    QWidget *mainWidget;
    QVBoxLayout *layoutMain;
    QHBoxLayout *layoutFilters;
    QHBoxLayout *layoutContent;
    QHBoxLayout *layoutCards; 
    QTableWidget *dataTable;
    QComboBox *filterYear;
    QComboBox *filterLocation;
    QComboBox *filterPollutant;
    QComboBox *filterStatus;
    QPushButton *applyFilterButton;
    QTextEdit *infoBox;
    QLabel *footerText;
    QFrame *summaryFrames[4];
    QLabel *headerText;

    // Add other variables as needed...
};

#endif // COMPLIANCEDASHBOARD_HPP
