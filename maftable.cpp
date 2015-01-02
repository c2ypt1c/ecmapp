#include "maftable.h"
#include <QHeaderView>
#include <QAction>
#include <QMenu>
#include <QClipboard>
#include <QDebug>
#include <QApplication>

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
        "0(hz)", "50", "100", "150", "200", "250",
        "300", "400", "500", "600", "700",
        "800", "900", "1000", "1100", "1200",
        "1300", "1400", "1500", "1600", "1700",
        "1800", "1900", "2000", "2100", "2200",
        "2300", "2400", "2500", "2600", "2700",
        "2800", "2900", "3000", "3100", "3200", "3300",
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

    mafCreateActions();
    mafTable->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(mafTable, SIGNAL(customContextMenuRequested(QPoint)),SLOT(mafRightClick(QPoint)));
}

void MafTable::mafCreateActions()
{
    mafCopyAction = new QAction("Copy Table", this);
    connect(mafCopyAction, SIGNAL(triggered()), this, SLOT(mafCopy()));

    mafPasteAction = new QAction("Paste Table", this);
    connect(mafPasteAction, SIGNAL(triggered()), this, SLOT(mafPaste()));
}

void MafTable::mafCopy()
{
    QClipboard *clipboard = QApplication::clipboard();

    clipboard->clear();

    for(int row = 0; row < mafTable->rowCount(); row++)
    {
        QTableWidgetItem *item = mafTable->item(row, 0);

        if(item)
            clipboard->setText(clipboard->text() + QString("%1\t%2\n").arg(mafTable->verticalHeaderItem(row)->text(), mafTable->item(row,0)->text()));
    }

    //qDebug() << clipboard->text();
}

void MafTable::mafPaste()
{

}

void MafTable::mafRightClick(QPoint p)
{
    QMenu *mafMenu=new QMenu(this);
    mafMenu->addAction(mafCopyAction);
    mafMenu->addAction(mafPasteAction);
    mafMenu->popup(mafTable->viewport()->mapToGlobal(p));
}
