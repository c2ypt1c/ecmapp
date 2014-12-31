#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QClipboard>
#include <QDebug>
#include <QRegularExpression>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->actionQuit, SIGNAL(triggered()), this, SLOT(close()));
    connect(ui->actionImport_Log, SIGNAL(triggered()), this, SLOT(importLog()));

    MainWindow::veTable = ui->veTableWidget;
    initVETable();

    MainWindow::mafTable = ui->mafTableWidget;
    initMafTable();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::importLog()
{
    qDebug() << "import!";
}

void MainWindow::initMafTable()
{
    // do something
    const QStringList mafColums = {"Adj %"};

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

    mafTable->setHorizontalHeaderLabels(mafColums);
    mafTable->setVerticalHeaderLabels(mafRows);

    mafTable->horizontalHeader()->setDefaultSectionSize(50);
    mafTable->verticalHeader()->setDefaultSectionSize(15);
}

void MainWindow::initVETable()
{
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
    veTable->verticalHeader()->setDefaultSectionSize(23);

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

void MainWindow::veCreateActions()
{
    veCopyAction = new QAction("Copy Table", this);
    connect(veCopyAction, SIGNAL(triggered()), this, SLOT(veCopy()));

    vePasteAction = new QAction("Paste Table", this);
    connect(vePasteAction, SIGNAL(triggered()), this, SLOT(vePaste()));

    veLoadDefaultAction = new QAction("Load A Default VE Table...", this);
    connect(veLoadDefaultAction, SIGNAL(triggered()), this, SLOT(veLoadDefault()));
}

void MainWindow::veSetCell(QTableWidgetItem *itm, QString val)
{
    itm->setText(val);

    float v = val.toFloat();

    QColor c;

    // set edge cases
    if(v <= 50)
    {
        itm->setBackgroundColor(QColor(0,240,240));
        return;
    }

    else if(v >= 115)
    {
        itm->setBackgroundColor((QColor(255,0,0)));
        return;
    }

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

    itm->setBackgroundColor(c);
}

void MainWindow::veCopy()
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

    //qDebug() << clipboard->text();
}

void MainWindow::vePaste()
{
    // get clipboard contents
    QClipboard *clipboard = QApplication::clipboard();
    QString clipText = clipboard->text();

    // input validation

    QRegularExpressionMatch m = QRegularExpression("([0-9]+\\.[0-9]+[\\t\\n])+").match(clipText);

    if(!m.hasMatch())
    {
        qDebug() << "[Error] Clipboard contains non-numeric values";
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
        qDebug() << "[Error] Invalid Data";
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

        //qDebug() << clipStrings[k];

        QTableWidgetItem *item = veTable->item(j, i);

        if(item)
            veSetCell(item, clipStrings[k]);
            //item->setText(clipStrings[k]);
    }
}

void MainWindow::veLoadDefault()
{
    // pop to choose between 1g, 2g, or evo
    qDebug() << "Loading Default";
}

void MainWindow::veRightClick(QPoint p)
{
    QMenu *veMenu=new QMenu(this);
    veMenu->addAction(veCopyAction);
    veMenu->addAction(vePasteAction);
    veMenu->addSeparator();
    veMenu->addAction(veLoadDefaultAction);
    veMenu->popup(veTable->viewport()->mapToGlobal(p));
}
