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
    void setupUI();
    void populateTable(const std::string& filename);
    void applyFilters();

    // New method to calculate compliance status
    std::string calculateComplianceStatus(const WaterSample& sample, const std::vector<PollutantSample>& pollutantSamples);
    void populateStats(const std::string& bestLocation, const std::string& worstLocation,
                                         const std::string& bestYear, const std::string& worstYear,
                                         const std::string& bestPollutant, const std::string& worstPollutant,
                                         int totalSamples, int missingDataCount, int compliantSamples,
                                         int mediumSamples, int nonCompliantSamples);

    QWidget *centralWidget;
    QVBoxLayout *mainLayout;
    QHBoxLayout *filtersLayout;
    QHBoxLayout *contentLayout;
    QHBoxLayout *cardsLayout; 
    QTableWidget *detailedTable;
    QComboBox *yearFilter;
    QComboBox *locationFilter;
    QComboBox *pollutantFilter;
    QComboBox *statusFilter;
    QPushButton *filterButton;
    QTextEdit *importantInfo;
    QLabel *footer;
    QFrame *summaryCards[4];
    QLabel *header;

    // Add other variables as needed...
};

#endif // COMPLIANCEDASHBOARD_HPP
