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
    connect(mafTable, SIGNAL(itemChanged(QTableWidgetItem*)), SLOT(mafUpdate(QTableWidgetItem*)));

    initMafTable();
}

MafTable::~MafTable()
{

}

void MafTable::mafUpdate(QTableWidgetItem *itm)
{
    if(itm->text().toFloat() > 70)
        itm->setText("70");

    else if(itm->text().toFloat() < -50)
        itm->setText("-50");
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
    QRegularExpressionMatch m = QRegularExpression("(-?[0-9]+\\.[0-9]+[\\t\\n])+").match(clipText);
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
        m = QRegularExpression("[0-9]+\\t(-?[0-9]+(\\.[0-9]+)?)").match(clipRows[i]);

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
            item->setText(clipStrings[row]);
    }
}

void MafTable::calcAffectedCells()
{
    // remove old data
    affectedRows.clear();

    // populates MafTable::affectedRows to correspond with wbfactor entries
    for(int i = 0, rowVal = 0; i < mafRawList->length(); i++)
    {
        // qRound does the dirty work for us
        rowVal = qRound(mafRawList->at(i)/100);
        affectedRows.append(rowVal*100);
    }
}

void MafTable::mafShowAffectedCells()
{
    mafTable->clearSelection();
    calcAffectedCells();

    // remove duplicate entries from affected rows
    QList<int> trimmedRowList;
    for(int i = 0, aVal = 0; i < affectedRows.count(); i++)
    {
        aVal = affectedRows[i];

        if(!trimmedRowList.contains(aVal) && aVal >= 300 && aVal <= 3600)
            trimmedRowList.append(aVal);
    }

    // highlight affected cells
    QString text;
    for(int i = 0; i < mafTable->rowCount(); i++)
    {
        text = mafTable->verticalHeaderItem(i)->text();
        if(trimmedRowList.contains(text.toInt()))
            mafTable->item(i, 0)->setSelected(true);
    }
}

void MafTable::mafApplyCorrections()
{
    // TODO: use slot when file is imported
    calcAffectedCells();


    if(affectedRows.count() != wbfList->count())
    {
        qDebug() << "count mismatch";
        return;
    }

    // trimmed affected rows
    QList<int> trAffectedRows;

    QList<int> avgList;

    // determine denominator for average
    for(int i = 0, aVal; i < affectedRows.count(); i++)
    {
        aVal = affectedRows[i];

        int j = trAffectedRows.indexOf(aVal);
        if(j != -1)
            avgList[j] += 1;

        else
        {
            trAffectedRows.append(aVal);
            avgList.append(1);
        }
    }

    // calculate average and store in correctedList
    QList<float> correctedList;
    for(int i = 0, wbfIndex = 0; i < avgList.count(); i++)
    {
        float correction = 0.0;
        int j = 0;
        while(j < avgList[i])
        {
            correction += wbfList->at(wbfIndex);
            j++;
            wbfIndex++;
        }

        correctedList.append(correction/j);
    }

    // set cell to suggested correction
    for(int i = 0; i < trAffectedRows.count(); i++)
    {
        qDebug() << trAffectedRows[i] << ": " << correctedList[i];

        // TODO: this seems inefficient
        for(int j = 0; j < mafTable->rowCount(); j++)
        {
            QTableWidgetItem *item = mafTable->verticalHeaderItem(j);
            if(trAffectedRows[i] == item->text().toFloat())
            {
                float val = mafTable->item(j, 0)->text().toFloat();
                val = val + correctedList[i];
                mafTable->item(j, 0)->setText(QString::number(val, 'f', 2));
                mafShowAffectedCells();
            }
        }
    }
}

void MafTable::mafCreateActions()
{
    mafCopyAction = new QAction("Copy Table", this);
    connect(mafCopyAction, SIGNAL(triggered()), this, SLOT(mafCopy()));

    mafPasteAction = new QAction("Paste Table", this);
    connect(mafPasteAction, SIGNAL(triggered()), this, SLOT(mafPaste()));

    mafShowAffectedAction = new QAction("Show Affected Cells", this);
    connect(mafShowAffectedAction, SIGNAL(triggered()), SLOT(mafShowAffectedCells()));

    mafApplyCorrectionsAction = new QAction("Apply Corrections", this);
    connect(mafApplyCorrectionsAction, SIGNAL(triggered()), SLOT(mafApplyCorrections()));
}

void MafTable::mafRightClick(QPoint p)
{
    QMenu *mafMenu=new QMenu(this);
    mafMenu->addAction(mafCopyAction);
    mafMenu->addAction(mafPasteAction);
    mafMenu->addSeparator();
    mafMenu->addAction(mafShowAffectedAction);
    mafMenu->addAction(mafApplyCorrectionsAction);

    mafMenu->popup(mafTable->viewport()->mapToGlobal(p));
}
