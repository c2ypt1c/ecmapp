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

    QString labelStyle = "QLabel{color: rgba(0,0,100,160); font: bold}";
    ui->veTableLabel->setStyleSheet(labelStyle);
    ui->mafCompLabel->setStyleSheet(labelStyle);


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


