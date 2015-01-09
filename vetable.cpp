#include "vetable.h"
#include <QTableWidget>
#include <QHeaderView>
#include <QAction>
#include <QApplication>
#include <QMenu>
#include <QClipboard>
#include <QDebug>
#include <QMessageBox>

VeTable::VeTable(QWidget *parent):
    QTableWidget (parent)
{
    veTable = this;

    connect(veTable, SIGNAL(itemChanged(QTableWidgetItem*)), SLOT(updateCell(QTableWidgetItem*)));

    const QStringList veColumns = {
        "0rpm", "500", "1000", "1500",
        "2000", "2500", "3000", "3500",
        "4000", "4500", "5000", "5500",
        "6000", "6500", "7000", "7500",
        "8000", "8500", "9000", "9500", "10k"
    };

    const QStringList veRows = {
        "0(30\"hg)", "1.8(26.3)", "3.7(22.5)", "5.5(18.8)",
        "7.3(15.1)", "9.2(11.2)", "11.0(7.6)", "12.9(3.7)",
        "14.7(0psi)", "16.5(1.8)", "18.4(3.7)", "20.2(5.5)",
        "22.0(7.3)", "23.9(9.2)", "25.7(11.0)", "27.6(12.9)", "29.4(14.7)",
        "31.2(16.5)", "33.1(18.4)", "36.7(22.0)", "40.4(25.7)",
        "44.1(29.4)", "47.8(33.1)", "51.4(36.7)", "55.1(40.4)"
    };

    for(int i = 0; i < veColumns.length(); i++)
        veTable->insertColumn(i);

    for(int i = 0; i < veRows.length(); i++)
        veTable->insertRow(i);

    for(int r = 0, c = 0, i = 0; i < veColumns.length()*veRows.length(); c++, i++)
    {
        if(i == veTable->columnCount())
        {
            c = 0;
            r++;
        }

        QTableWidgetItem *item = new QTableWidgetItem();
        item->setTextAlignment(Qt::AlignRight);

        veTable->setItem(r, c, item);
    }

    veTable->setHorizontalHeaderLabels(veColumns);
    veTable->setVerticalHeaderLabels(veRows);
    veTable->verticalHeader()->setDefaultAlignment(Qt::AlignRight);

    veTable->horizontalHeader()->setDefaultSectionSize(49);
    veTable->verticalHeader()->setDefaultSectionSize(24);

    // underline vacuum and psi
    QFont f;
    f.setUnderline(true);
    veTable->verticalHeaderItem(0)->setFont(f);
    veTable->verticalHeaderItem(8)->setFont(f);

    // set up right-click context
    createActions();
    veTable->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(veTable, SIGNAL(customContextMenuRequested(QPoint)), SLOT(rightClick(QPoint)));

    loadDefault();
}

VeTable::~VeTable()
{
    //delete veTable;
}

void VeTable::updateCell(QTableWidgetItem *itm)
{
    QString t = itm->text();

    // if empty
    if(t == "")
        return;

    // if non-number
    QRegularExpressionMatch m = QRegularExpression("^[0-9]+(\\.[0-9]+)?$").match(t);
    if(!m.hasMatch())
    {
        itm->setText("0.0");
        return;
    }

    float v = t.toFloat();

    if(v <= 50)
    {
        itm->setBackgroundColor(QColor(0,240,240));
        return;
    }

    else if(v >= 115)
    {
        itm->setBackgroundColor((QColor(255,0,0)));

        if(v > 127.5)
            itm->setText("127.5");

        return;
    }

    QColor c;
    if(v >= 50 && v < 62)
    {
        c.setBlue(240);
        c.setGreen(240-(v-50)*7.4);
        c.setRed(0);
    }

    else if(v >= 62 && v < 75)
    {
        c.setBlue(240-((v-62)*18.4));
        c.setGreen(155+(v-62)*8);
        c.setRed(0);
    }

    else if(v >= 75 && v < 82)
    {
        c.setBlue((v-75)*19.4);
        c.setGreen(255);
        c.setRed((v-75)*36.4);
    }

    else if(v >= 82 && v < 89)
    {
        c.setBlue(136-((v-82)*19.4));
        c.setGreen(250);
        c.setRed(255);
    }

    else if(v >= 89 && v < 115)
    {
        c.setBlue(0);
        c.setGreen(250-(v-89)*9.6);
        c.setRed(255);
    }

    else
        return;

    itm->setBackgroundColor(c);
}

void VeTable::copy()
{
    QClipboard *clipboard = QApplication::clipboard();

    clipboard->clear();

    for(int r = 0, c = 0, i = 0; i < veTable->columnCount()*veTable->rowCount(); c++, i++)
    {
        if(c == veTable->columnCount())
        {
            c = 0;
            r++;
        }

        QTableWidgetItem *item = veTable->item(r, c);

        if(item)
        {
            if(r < 1 && c < 1)
                clipboard->setText(item->text());
            else if(r > 0 && c < 1)
                clipboard->setText(clipboard->text() + "\n" + item->text());
            else
                clipboard->setText(clipboard->text() + "\t" + item->text());
        }
    }
}

void VeTable::paste()
{
    // get clipboard contents
    QClipboard *clipboard = QApplication::clipboard();
    QString clipText = clipboard->text();

    // input validation

    QRegularExpressionMatch m = QRegularExpression("([0-9]+\\.[0-9]+[\\t\\n])+").match(clipText);

    if(!m.hasMatch())
    {
        QMessageBox::warning(NULL, "", "Copy Speed Density table from ECMLink first");
        return;
    }

    QStringList clipRows = clipText.split("\n");
    QStringList clipStrings = {0};

    for(int i = 0; i < clipRows.length(); i++)
        clipStrings.append(clipRows[i].split("\t"));

    // TODO: this causes problems when copying and pasting our own table
    clipStrings.removeFirst();
    clipStrings.removeLast();

    // make sure entire table is copied
    if(clipStrings.length() != veTable->columnCount()*veTable->rowCount())
    {
        QMessageBox::warning(NULL, "", "Copy Speed Density table from ECMLink first");
        return;
    }

    for(int i = 0, j = 0, k = 0; k < clipStrings.count(); i++, k++)
    {
        if(i == veTable->columnCount())
        {
            // reset column index
            i = 0;

            // increment row index
            j++;
        }

        QTableWidgetItem *item = veTable->item(j, i);

        if(item)
            item->setText(clipStrings[k]);
    }
}

void VeTable::loadDefault()
{
    QString vals = "55.0 55.0 57.0 63.5 67.0 66.0 74.0 71.5 75.5 77.5 75.5 75.5 75.5 73.0 72.0 72.0 72.0 72.0 72.0 72.0 72.0 "
            "55.0 55.0 57.0 63.5 67.0 66.0 74.0 71.5 75.5 77.5 75.5 75.5 75.5 73.0 72.0 72.0 72.0 72.0 72.0 72.0 72.0 "
            "55.0 55.0 56.5 62.5 66.5 65.0 73.0 70.5 75.0 76.5 75.0 75.0 75.0 72.5 71.0 71.0 71.0 71.0 71.0 71.0 71.0 "
            "55.0 55.0 56.5 62.5 66.5 65.0 73.0 70.5 75.0 76.5 75.0 75.0 75.0 72.5 71.0 71.0 71.0 71.0 71.0 71.0 71.0 "
            "55.0 55.0 57.0 63.5 67.0 66.0 74.0 71.5 75.5 77.5 75.5 75.5 75.5 73.0 72.0 72.0 72.0 72.0 72.0 72.0 72.0 "
            "55.0 55.0 59.0 65.0 69.0 68.0 76.0 73.5 78.0 79.5 78.0 78.0 78.0 75.5 74.0 74.0 74.0 74.0 74.0 74.0 74.0 "
            "55.0 55.0 62.5 68.5 73.5 72.0 80.0 78.0 82.0 84.0 82.0 82.0 82.0 79.5 78.5 78.5 78.5 78.5 78.5 78.5 78.5 "
            "55.0 56.5 64.5 71.5 76.0 74.5 83.0 80.5 85.5 87.5 85.5 85.5 85.5 82.5 81.0 81.0 81.0 81.0 81.0 81.0 81.0 "
            "57.5 59.5 68.0 75.0 80.0 78.5 88.0 85.0 90.0 92.0 90.0 90.0 90.0 87.0 85.5 85.5 85.5 85.5 85.5 85.5 85.5 "
            "57.5 59.5 68.0 75.0 80.0 78.5 88.0 85.0 90.0 92.0 90.0 90.0 90.0 87.0 85.5 85.5 85.5 85.5 85.5 85.5 85.5 "
            "59.0 61.5 69.5 77.0 82.0 80.5 90.0 87.0 92.5 94.0 92.5 92.5 92.5 89.5 88.0 88.0 88.0 88.0 88.0 88.0 88.0 "
            "59.0 61.5 69.5 77.0 82.0 80.5 90.0 87.0 92.5 94.0 92.5 92.5 92.5 89.5 88.0 88.0 88.0 88.0 88.0 88.0 88.0 "
            "59.0 61.5 69.5 77.0 82.0 80.5 90.0 87.0 92.5 94.0 92.5 92.5 92.5 89.5 88.0 88.0 88.0 88.0 88.0 88.0 88.0 "
            "59.0 61.5 69.5 77.0 82.0 80.5 90.0 87.0 92.5 94.0 92.5 92.5 92.5 89.5 88.0 88.0 88.0 88.0 88.0 88.0 88.0 "
            "62.0 64.0 73.5 80.5 86.0 84.5 94.0 91.5 96.5 99.0 96.5 96.5 96.5 93.5 92.5 92.5 92.5 92.5 92.5 92.5 92.5 "
            "62.0 64.0 73.5 80.5 86.0 84.5 94.0 91.5 96.5 99.0 96.5 96.5 96.5 93.5 92.5 92.5 92.5 92.5 92.5 92.5 92.5 "
            "62.0 64.0 73.5 80.5 86.0 84.5 94.0 91.5 96.5 99.0 96.5 96.5 96.5 93.5 92.5 92.5 92.5 92.5 92.5 92.5 92.5 "
            "62.0 64.0 73.5 80.5 86.0 84.5 94.0 91.5 96.5 99.0 96.5 96.5 96.5 93.5 92.5 92.5 92.5 92.5 92.5 92.5 92.5 "
            "62.0 64.0 73.5 80.5 86.0 84.5 94.0 91.5 96.5 99.0 96.5 96.5 96.5 93.5 92.5 92.5 92.5 92.5 92.5 92.5 92.5 "
            "62.0 64.0 73.5 80.5 86.0 84.5 94.0 91.5 96.5 99.0 96.5 96.5 96.5 93.5 92.5 92.5 92.5 92.5 92.5 92.5 92.5 "
            "62.0 64.0 73.5 80.5 86.0 84.5 94.0 91.5 96.5 99.0 96.5 96.5 96.5 93.5 92.5 92.5 92.5 92.5 92.5 92.5 92.5 "
            "62.0 64.0 73.5 80.5 86.0 84.5 94.0 91.5 96.5 99.0 96.5 96.5 96.5 93.5 92.5 92.5 92.5 92.5 92.5 92.5 92.5 "
            "62.0 64.0 73.5 80.5 86.0 84.5 94.0 91.5 96.5 99.0 96.5 96.5 96.5 93.5 92.5 92.5 92.5 92.5 92.5 92.5 92.5 "
            "62.0 64.0 73.5 80.5 86.0 84.5 94.0 91.5 96.5 99.0 96.5 96.5 96.5 93.5 92.5 92.5 92.5 92.5 92.5 92.5 92.5 "
            "62.0 64.0 73.5 80.5 86.0 84.5 94.0 91.5 96.5 99.0 96.5 96.5 96.5 93.5 92.5 92.5 92.5 92.5 92.5 92.5 92.5";

    QStringList valList = vals.split(" ");

    for(int r = 0, c = 0, i = 0; i < veTable->columnCount()*veTable->rowCount(); c++, i++)
    {
        if(c == veTable->columnCount())
        {
            c = 0;
            r++;
        }

        QTableWidgetItem *itm = veTable->item(r, c);

        if(itm)
            itm->setText(valList[i]);
    }
}

void VeTable::fileLoaded()
{
    rpmPercents = new QList<QList<float>>;
    psiPercents = new QList<QList<float>>;

    rpmIndecies = new QList<QList<float>>;
    psiIndecies = new QList<QList<float>>;

    QList<int> rpmHeader = {0, 500, 1000, 1500, 2000, 2500,
                         3000, 3500, 4000, 4500, 5000, 5500,
                         6000, 6500, 7000, 7500, 8000, 8500,
                         9000, 9500, 10000};

    QList<float> psiHeader = {-30, -26.3, -22.5, -18.8, -15.1, -11.2, -7.6, -3.7, 0.0,
                               1.8, 3.7, 5.5, 7.3, 9.2, 11.0, 12.9, 14.7, 16.5, 18.4, 22.0,
                               25.7, 29.4, 33.1, 36.7, 40.4};

    for(int i = 0; i < rpmList->length(); i++)
    {
        for(int j = 0; j < rpmHeader.length()-1; j++)
        {
            int rIndex = j;
            int lIndex = j+1;

            float rpm = rpmList->at(i);
            float rHead = rpmHeader[rIndex];
            float lHead = rpmHeader[lIndex];

            if(rpm >= rHead && rpm < lHead)
            {
                float rPer = rHead/rpm;
                float lPer = rpm/lHead;

                if(lPer >= 0.9772727)
                {
                    QList<float> rpmRLIndecies;
                    rpmRLIndecies.append(j+1);
                    rpmRLIndecies.append(j+1);
                    rpmIndecies->append(rpmRLIndecies);
                }

                else if(rPer >= 0.9772727)
                {
                    QList<float> rpmRLIndecies;
                    rpmRLIndecies.append(j);
                    rpmRLIndecies.append(j);
                    rpmIndecies->append(rpmRLIndecies);
                }

                else
                {
                    QList<float> rpmRLIndecies;
                    rpmRLIndecies.append(j);
                    rpmRLIndecies.append(j+1);
                    rpmIndecies->append(rpmRLIndecies);
                }

                QList<float> percentages;
                percentages.append(rPer);
                percentages.append(lPer);

                rpmPercents->append(percentages);
                qDebug() << "rpm: " << rpm;
                qDebug() << rPer << " (" << rHead << ")";
                qDebug() << lPer << " (" << lHead << ")";
            }
        }
    }

    for(int i = 0; i < psiList->count(); i++)
    {
        for(int j = 0; j < psiHeader.count()-1; j++)
        {
            int rIndex = j;
            int lIndex = j+1;

            float psi = psiList->at(i);
            float rHead = psiHeader[rIndex];
            float lHead = psiHeader[lIndex];

            if(psi >= rHead && psi < lHead)
            {
                float rPer = rHead/psi;
                float lPer = psi/lHead;

                if(lPer >= 0.9772727)
                {
                    QList<float> psiRLIndecies;
                    psiRLIndecies.append(j+1);
                    psiRLIndecies.append(j+1);
                    psiIndecies->append(psiRLIndecies);
                }

                else if(rPer >= 0.9772727)
                {
                    QList<float> psiRLIndecies;
                    psiRLIndecies.append(j);
                    psiRLIndecies.append(j);
                    psiIndecies->append(psiRLIndecies);
                }

                else
                {
                    QList<float> psiRLIndecies;
                    psiRLIndecies.append(j);
                    psiRLIndecies.append(j+1);
                    psiIndecies->append(psiRLIndecies);
                }

                QList<float> percentages;
                percentages.append(rPer);
                percentages.append(lPer);

                psiPercents->append(percentages);
                qDebug() << "psi: " << psi;
                qDebug() << rPer << " (" << rHead << ")";
                qDebug() << lPer << " (" << lHead << ")";
            }
        }
    }
    //qDebug() << "breakpoint";
}

void VeTable::showAffectedCells()
{
    veTable->clearSelection();

    for(int i = 0; i < rpmList->length(); i++)
    {
        veTable->item(psiIndecies->at(i).at(0), rpmIndecies->at(i).at(0))->setSelected(true);
        veTable->item(psiIndecies->at(i).at(1), rpmIndecies->at(i).at(1))->setSelected(true);
    }

    veTable->setSelectionMode(QAbstractItemView::NoSelection);
}

void VeTable::createActions()
{
    copyAction = new QAction("Copy Table", this);
    connect(copyAction, SIGNAL(triggered()), this, SLOT(copy()));

    pasteAction = new QAction("Paste Table", this);
    connect(pasteAction, SIGNAL(triggered()), this, SLOT(paste()));

    loadDefaultAction = new QAction("Load Default", this);
    connect(loadDefaultAction, SIGNAL(triggered()), this, SLOT(loadDefault()));

    showAffectedAction = new QAction("Show Affected Cells", this);
    connect(showAffectedAction, SIGNAL(triggered()), SLOT(showAffectedCells()));
}

void VeTable::rightClick(QPoint p)
{
    QMenu *veMenu=new QMenu(this);
    veMenu->addAction(copyAction);
    veMenu->addAction(pasteAction);
    veMenu->addSeparator();
    veMenu->addAction(showAffectedAction);
    veMenu->addSeparator();
    veMenu->addAction(loadDefaultAction);
    veMenu->popup(veTable->viewport()->mapToGlobal(p));
}
