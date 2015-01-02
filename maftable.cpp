#include "maftable.h"
#include <QHeaderView>

MafTable::MafTable(QWidget *parent):
    QTableWidget (parent)
{
    mafTable = this;
    initMafTable();
}

MafTable::~MafTable()
{

}

void MafTable::initMafTable()
{
    // do something
    const QStringList mafColumns = {"Adj %"};

    const QStringList mafRows = {
        "0(hz)", "50", "100", "150", "200",
        "300", "400", "500", "600", "700",
        "800", "900", "1000", "1100", "1200",
        "1300", "1400", "1500", "1600", "1700",
        "1800", "1900", "2000", "2100", "2200",
        "2300", "2400", "2500", "2600", "2700",
        "2800", "2900", "3000", "3100", "3300",
        "3400", "3500", "3600"
    };

    mafTable->insertColumn(0);

    for(int i = 0; i < mafRows.length(); i++)
        mafTable->insertRow(i);

    for(int i = 0, j = 0, k = 0; k < mafColumns.length()*mafRows.length(); i++, k++)
    {
        QTableWidgetItem *item = new QTableWidgetItem();
        mafTable->setItem(j, i, item);
    }

    mafTable->setHorizontalHeaderLabels(mafColumns);
    mafTable->setVerticalHeaderLabels(mafRows);

    mafTable->horizontalHeader()->setDefaultSectionSize(50);
    mafTable->verticalHeader()->setDefaultSectionSize(15);
}
