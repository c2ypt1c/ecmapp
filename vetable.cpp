#include <QtGui>
#include <QTableWidget>
#include <QHeaderView>
#include <QAction>
#include <QApplication>
#include <QMenu>
#include <QMessageBox>
#include "vetable.h"

VeTable::VeTable(QWidget *parent):
    QTableWidget (parent)
{
    veTable = this;

    connect(veTable, SIGNAL(itemChanged(QTableWidgetItem*)), SLOT(veUpdate(QTableWidgetItem*)));

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

    for(int i = 0, j = 0, k = 0; k < veColumns.length()*veRows.length(); i++, k++)
    {
        if(i == veColumns.length())
        {
            i = 0;
            j++;
        }

        QTableWidgetItem *item = new QTableWidgetItem();
        veTable->setItem(j, i, item);
    }

    veTable->setHorizontalHeaderLabels(veColumns);
    veTable->setVerticalHeaderLabels(veRows);

    veTable->horizontalHeader()->setDefaultSectionSize(49);
    veTable->verticalHeader()->setDefaultSectionSize(24);

    // row header text align right
    for(int i = 0; i < veRows.length(); i++)
    {
        QTableWidgetItem *row = veTable->verticalHeaderItem(i);
        row->setTextAlignment(Qt::AlignRight);

        // underline vacuum and psi rows
        if(i == 0||i == 8)
        {
            QFont f = row->font();
            f.setUnderline(true);
            row->setFont(f);
        }
    }

    // set up right-click context
    veCreateActions();
    veTable->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(veTable, SIGNAL(customContextMenuRequested(QPoint)), SLOT(veRightClick(QPoint)));
}

VeTable::~VeTable()
{
    //delete veTable;
}

void VeTable::veCreateActions()
{
    veCopyAction = new QAction("Copy Table", this);
    connect(veCopyAction, SIGNAL(triggered()), this, SLOT(veCopy()));

    vePasteAction = new QAction("Paste Table", this);
    connect(vePasteAction, SIGNAL(triggered()), this, SLOT(vePaste()));

    veLoadDefaultAction = new QAction("Load A Default VE Table...", this);
    connect(veLoadDefaultAction, SIGNAL(triggered()), this, SLOT(veLoadDefault()));
}

void VeTable::veUpdate(QTableWidgetItem *itm)
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

void VeTable::veCopy()
{
    QClipboard *clipboard = QApplication::clipboard();

    clipboard->clear();

    for(int i = 0, j = 0, k = 0; k < veTable->columnCount()*veTable->rowCount(); i++, k++)
    {
        if(i == veTable->columnCount())
        {
            i = 0;
            j++;
        }

        QTableWidgetItem *item = veTable->item(j, i);

        if(item)
        {
            if(i < 1 && j < 1)
                clipboard->setText(item->text());
            else if(i < 1 && j > 0)
                clipboard->setText(clipboard->text() + "\n" + item->text());
            else
                clipboard->setText(clipboard->text() + "\t" + item->text());
        }
    }
}

void VeTable::vePaste()
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

void VeTable::veLoadDefault()
{
    // pop to choose between 1g, 2g, or evo
    qDebug() << "Loading Default";
}

void VeTable::veRightClick(QPoint p)
{
    QMenu *veMenu=new QMenu(this);
    veMenu->addAction(veCopyAction);
    veMenu->addAction(vePasteAction);
    veMenu->addSeparator();
    veMenu->addAction(veLoadDefaultAction);
    veMenu->popup(veTable->viewport()->mapToGlobal(p));
}
