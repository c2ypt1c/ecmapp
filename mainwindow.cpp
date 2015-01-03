#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QClipboard>
#include <QDebug>
#include <QRegularExpression>
#include <QFileDialog>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->actionQuit, SIGNAL(triggered()), this, SLOT(close()));
    connect(ui->actionImportCvs, SIGNAL(triggered()), this, SLOT(importCvs()));

    connect(ui->veButton, SIGNAL(clicked()), SLOT(sdMode()));
    connect(ui->mafButton, SIGNAL(clicked()), SLOT(mafMode()));

    QString labelStyle = "QPushButton{color: rgba(0,0,100,160); font: bold}";
    ui->veButton->setStyleSheet(labelStyle);
    ui->mafButton->setStyleSheet(labelStyle);

    // disable tables until file loaded
    ui->veTableWidget->setDisabled(true);
    ui->mafTableWidget->setDisabled(true);

    airflowMode = 0;
    fileImported = false;
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
        parseCsv(filenames[0]);
    }
}

void MainWindow::parseCsv(QString n)
{
    csvFile = new QFile(n);

    if(!csvFile->open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    else
        qDebug() << "file opened!";

    QString l = csvFile->readLine();

    setMode(l);
    QStringList lineSplit = l.split(",");
    int afIndex = findAirflowIndex(lineSplit);
    int wbFactorIndex = findWBFactorIndex(lineSplit);

    parseData(csvFile, afIndex, wbFactorIndex);
}

// determine which mode based on first line in csv file
void MainWindow::setMode(QString line)
{
    QRegularExpressionMatch match;
    QRegularExpression re("((MAFRaw.*VE|VE.*MAFRaw)|(MAFRaw|VE))");
    match = re.match(line);

    if(match.hasMatch())
    {
        // set file imported flag
        fileImported = true;
        ui->mafTableWidget->fileImported = true;

        // if MAFRaw and VE are found
        if(match.captured(2) != "")
        {
            QMessageBox msgBox;
            msgBox.setText("VE and MAFRaw detected, choose which mode to operate in");
            QPushButton *speedDensityButton = msgBox.addButton("Speed Density", QMessageBox::ActionRole);
            QPushButton *massAirFlowButton = msgBox.addButton("Mass Air Flow", QMessageBox::ActionRole);

            msgBox.exec();
            if (msgBox.clickedButton() == (QAbstractButton*)speedDensityButton)
                sdMode();

            else if (msgBox.clickedButton() == (QAbstractButton*)massAirFlowButton)
                mafMode();
        }

        // if VE only
        else if(match.captured(1) == "VE")
        {
            qDebug() << "ve detected";
            sdMode();
        }

        // if MAFRaw only
        else if(match.captured(1) == "MAFRaw")
        {
            qDebug() << "mafraw detected";
            mafMode();
        }
    }

    else
        qDebug() << "no match";
}

void MainWindow::sdMode()
{
    airflowMode = 1;
    ui->veTableWidget->setEnabled(true);
    ui->mafTableWidget->setDisabled(true);
    ui->mafTableWidget->clearSelection();

    if(fileImported)
    {
        ui->mafButton->setDisabled(true);
        ui->veButton->setDisabled(true);
    }
}

void MainWindow::mafMode()
{
    airflowMode = 2;
    ui->mafTableWidget->setEnabled(true);
    ui->veTableWidget->setDisabled(true);
    ui->veTableWidget->clearSelection();

    if(fileImported)
    {
        ui->veButton->setDisabled(true);
        ui->mafButton->setDisabled(true);
    }
}

int MainWindow::findAirflowIndex(QStringList fields)
{
    qDebug() << fields;
    if(airflowMode == 1)
    {
        for(int i = 0; i < fields.length(); i++)
        {
            if(fields[i].contains("VE"))
                return i;
        }
    }

    else if(airflowMode == 2)
    {
        for(int i = 0; i < fields.length(); i++)
        {
            if(fields[i].contains("MAFRaw"))
                return i;
        }
    }

    return 0;
}

int MainWindow::findWBFactorIndex(QStringList fields)
{
    for(int i = 0; i < fields.length(); i++)
    {
        if(fields[i].contains("WBFactor"))
            return i;
    }

    return 0;
}

void MainWindow::parseData(QFile *f, int afIndex, int wbfIndex)
{
    while(!f->atEnd())
    {
        QString line = f->readLine();
        QStringList lineSplit = line.split(",");

        if(airflowMode == 1)
            VEList.append(lineSplit[afIndex]);

        else if(airflowMode == 2)
            MAFRawList.append(lineSplit[afIndex]);

        WBFactorList.append(lineSplit[wbfIndex]);
    }

    qDebug() << "VEList: " << VEList;
    qDebug() << "MAFRawList: " << MAFRawList;
    qDebug() << "WBFactorList: " << WBFactorList;
}

QStringList MainWindow::getVEList()
{
    return VEList;
}

QStringList MainWindow::getMAFRawList()
{
    return MAFRawList;
}

QStringList MainWindow::getWBFactorList()
{
    return WBFactorList;
}
