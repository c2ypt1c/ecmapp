#include "maftable.h"
#include <QHeaderView>
#include <QAction>
#include <QMenu>
#include <QClipboard>
#include <QDebug>
#include <QApplication>
#include <QRegularExpression>
#include <QMessageBox>

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

    for(int row = 0; row < mafRows.length(); row++)
    {
        QTableWidgetItem *item = new QTableWidgetItem();
        item->setText("0.0");
        mafTable->setItem(row, 0, item);
        mafTable->item(row, 0)->setTextAlignment(Qt::AlignRight);
    }

    mafTable->setHorizontalHeaderLabels(mafColumns);
    mafTable->setVerticalHeaderLabels(mafRows);
    mafTable->verticalHeader()->setDefaultAlignment(Qt::AlignCenter);

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
}

void MafTable::mafPaste()
{
    // get clipboard contents
    QClipboard *clipboard = QApplication::clipboard();
    QString clipText = clipboard->text();

    // input validation
    QRegularExpressionMatch m = QRegularExpression("([0-9]+\\.[0-9]+[\\t\\n])+").match(clipText);
    if(!m.hasMatch())
    {
        QMessageBox::warning(NULL, "", "Copy MAF Comp table from ECMLink first");
        return;
    }

    QStringList clipRows = clipText.split("\n");
    clipRows.removeLast();

    QStringList clipStrings = {0};

    for(int i = 0; i < clipRows.length(); i++)
    {
        m = QRegularExpression("[0-9]+\\t([0-9]+(\\.[0-9]+)?)").match(clipRows[i]);

        if(m.hasMatch())
            clipStrings.append(m.captured(1));
    }
    clipStrings.removeFirst();

    // make sure entire table is copied
    if(clipStrings.length() != mafTable->rowCount())
    {
        QMessageBox::warning(NULL, "", "Copy MAF Comp table from ECMLink first");
        return;
    }

    for(int row = 0; row < clipStrings.count(); row++)
    {
        QTableWidgetItem *item = mafTable->item(row, 0);

        if(item)
        {
            item->setText(clipStrings[row]);
            //item->setTextAlignment(Qt::AlignRight);
        }
    }
}

void MafTable::mafRightClick(QPoint p)
{
    QMenu *mafMenu=new QMenu(this);
    mafMenu->addAction(mafCopyAction);
    mafMenu->addAction(mafPasteAction);
    mafMenu->popup(mafTable->viewport()->mapToGlobal(p));
}
