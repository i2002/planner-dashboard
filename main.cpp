#include "plannerdashboard.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    PlannerDashboard::initializeApp();
    QApplication a(argc, argv);
    PlannerDashboard p;
    #ifdef QT_DEBUG
        p.setupDevTools();
    #endif
    return a.exec();
}
