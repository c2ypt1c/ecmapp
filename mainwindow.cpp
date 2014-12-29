#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QClipboard>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->actionQuit, SIGNAL(triggered()), this, SLOT(close()));

    MainWindow::veTable = ui->veTableWidget;
    initVETable();
}

MainWindow::~MainWindow()
{
    delete ui;
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
        "22.0(7.3)", "23.9(9.2)", "25.7(11.0)", "29.4(14.7)",
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

    veTable->horizontalHeader()->setDefaultSectionSize(54);
    veTable->verticalHeader()->setDefaultSectionSize(25);

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
    veCopyAction = new QAction("Copy", this);
    connect(veCopyAction, SIGNAL(triggered()), this, SLOT(veCopy()));

    vePasteAction = new QAction("Paste", this);
    connect(vePasteAction, SIGNAL(triggered()), this, SLOT(vePaste()));

    veLoad2gAction = new QAction("Load Default 2G Table", this);
    connect(veLoad2gAction, SIGNAL(triggered()), this, SLOT(veLoad2g()));

    veLoad1gAction = new QAction("Load Default 1G Table", this);
    veLoadEvoAction = new QAction("Load Default EVO8 Table", this);
}

void MainWindow::veCopy()
{
    qDebug() << "Copied";
}

void MainWindow::vePaste()
{
    // get clipboard contents
    QClipboard *clipboard = QApplication::clipboard();
    QString clipText = clipboard->text();

    QStringList clipRows = clipText.split("\n");
    QStringList clipCells = {0};

    for(int i = 0; i < clipRows.length(); i++)
        clipCells.append(clipRows[i].split("\t"));

    clipCells.removeFirst();
    clipCells.removeLast();


    for(int i = 0, j = 0, k = 0; k < clipCells.count(); i++, k++)
    {
        if(i == veTable->columnCount())
        {
            // reset column index
            i = 0;

            // increment row index
            j++;
        }

        qDebug() << clipCells[k];
        QTableWidgetItem *item = veTable->item(j, i);

        if(item)
            item->setText(clipCells[k]);
    }
}

void MainWindow::veLoad2g()
{
    qDebug() << "Loading 2g";
}

void MainWindow::veRightClick(QPoint p)
{
    QMenu *veMenu=new QMenu(this);
    veMenu->addAction(veCopyAction);
    veMenu->addAction(vePasteAction);
    veMenu->addAction(veLoad2gAction);
    veMenu->addAction(veLoad1gAction);
    veMenu->addAction(veLoadEvoAction);
    veMenu->popup(veTable->viewport()->mapToGlobal(p));
}
