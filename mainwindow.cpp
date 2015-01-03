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

    if(!csvFile->open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    else
        qDebug() << "file opened!";

    QByteArray l = csvFile->readLine();
    qDebug() << l;

    setMode(l);
}

// determine which mode based on exported log items
void MainWindow::setMode(QString line)
{
    QRegularExpressionMatch match;
    QRegularExpression re("((MAFRaw.*VE|VE.*MAFRaw)|(MAFRaw|VE))");
    match = re.match(line);

    if(match.hasMatch())
    {
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

        // if MAFRaw only
        else if(match.captured(1) == "MAFRaw")
        {
            qDebug() << "mafraw detected";
            mafMode();
        }

        // if VE only
        else if(match.captured(1) == "VE")
        {
            qDebug() << "ve detected";
            sdMode();
        }
    }

    else
        qDebug() << "no match";
}

void MainWindow::sdMode()
{
    ui->veTableWidget->setEnabled(true);
    ui->mafTableWidget->setDisabled(true);
    ui->mafTableWidget->clearSelection();
}

void MainWindow::mafMode()
{
    ui->mafTableWidget->setEnabled(true);
    ui->veTableWidget->setDisabled(true);
    ui->veTableWidget->clearSelection();
}
