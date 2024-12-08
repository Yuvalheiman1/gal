#include <QApplication>
#include "ComplianceDashboard.hpp"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    ComplianceDashboard dashboard;
    dashboard.show();

    return app.exec();
}