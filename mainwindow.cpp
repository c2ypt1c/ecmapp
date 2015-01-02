#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QClipboard>
#include <QDebug>
#include <QRegularExpression>
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->actionQuit, SIGNAL(triggered()), this, SLOT(close()));
    connect(ui->actionImportCvs, SIGNAL(triggered()), this, SLOT(importCvs()));

    QString labelStyle = "QLabel{color: rgba(0,0,100,160); font: bold}";
    ui->veTableLabel->setStyleSheet(labelStyle);
    ui->mafCompLabel->setStyleSheet(labelStyle);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::importCvs()
{
    QFileDialog importDialog(this);
    importDialog.setNameFilter("ECMLink Export (*.csv)");

    if(importDialog.exec())
    {
        QStringList filenames = importDialog.selectedFiles();
        qDebug() << filenames;

        parseCsv(filenames[0]);
    }
}

void MainWindow::parseCsv(QString n)
{
    csvFile = new QFile(n);
    qDebug() << "file opened";
}
