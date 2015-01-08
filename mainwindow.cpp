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
    connect(ui->actionImportCvs, SIGNAL(triggered()), this, SLOT(importCsv()));

    QString labelStyle = "QLabel{color: rgba(0,0,100,160); font: bold}";
    ui->veLabel->setStyleSheet(labelStyle);
    ui->mafLabel->setStyleSheet(labelStyle);

    // disable tables until file loaded
    ui->veTableWidget->setDisabled(true);
    ui->mafTableWidget->setDisabled(true);

    airflowMode = 0;
}

MainWindow::~MainWindow()
{
    delete ui;
}

// open file dialog
void MainWindow::importCsv()
{
    QFileDialog importDialog(this);
    importDialog.setNameFilter("ECMLink Export (*.csv)");

    if(importDialog.exec())
    {
        QStringList filenames = importDialog.selectedFiles();
        parseCsv(filenames[0]);
    }
}

// parse exported ecmlink file
void MainWindow::parseCsv(QString n)
{
    // field indecies
    int ve, rpm, psi;
    int mafraw;

    QFile *csvFile = new QFile(n);

    // if file can't be opened, bail out
    if(!csvFile->open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    // read first (header) line and set mode
    QString l = csvFile->readLine().trimmed();

    //////////
    setMode();
    //////////

    QStringList fields = l.split(",");

    // if wbfactor data not found, bail out
    int wbfactor = fields.indexOf(QRegularExpression("^WBFactor.*"));
    if(wbfactor == -1)
    {
        QMessageBox::warning(this, NULL, "WBFactor not found");
        return;
    }

    if(airflowMode == 1)
    {
        ve = fields.indexOf(QRegularExpression("^VE.*"));
        rpm = fields.indexOf(QRegularExpression("^RPM.*"));
        psi = fields.indexOf(QRegularExpression(".*-inHg/\\+psi.*"));

        // if sd data not found, bail out
        if(ve == -1||rpm == -1||psi == -1)
        {
            QMessageBox::warning(this, NULL, "Speed Density requires RPM, Boost, and VE data");
            return;
        }

        ui->veTableWidget->veList = new QList<float>;
        ui->veTableWidget->rpmList = new QList<float>;
        ui->veTableWidget->psiList = new QList<float>;
        ui->veTableWidget->wbfList = new QList<float>;
    }


    else if(airflowMode == 2)
    {
        mafraw = fields.indexOf(QRegularExpression("^MAFRaw.*"));

        // if mafraw data not found, bail out
        if(mafraw == -1)
        {
            QMessageBox::warning(this, NULL, "Mass Airflow requires MAFRaw data");
            return;
        }

        ui->mafTableWidget->mafRawList = new QList<float>;
        ui->mafTableWidget->wbfList = new QList<float>;
    }

    // read file line-by-line
    while(!csvFile->atEnd())
    {
        QString line = csvFile->readLine().trimmed();
        QStringList lineSplit = line.split(",");

        // if sd mode, fill ve table lists
        if(airflowMode == 1)
        {
            ui->veTableWidget->veList->append(lineSplit[ve].toFloat());
            ui->veTableWidget->rpmList->append(lineSplit[rpm].toFloat());
            ui->veTableWidget->psiList->append(lineSplit[psi].toFloat());
            ui->veTableWidget->wbfList->append(lineSplit[wbfactor].toFloat());
        }

        // if maf mode, fill maf table lists
        else if(airflowMode == 2)
        {
            ui->mafTableWidget->mafRawList->append(lineSplit[mafraw].toFloat());
            ui->mafTableWidget->wbfList->append(lineSplit[wbfactor].toFloat());
        }
    }

    if(airflowMode == 1)
        ui->veTableWidget->fileLoaded();
}

// set sd or maf mode
void MainWindow::setMode()
{
    QMessageBox msgBox;
    msgBox.setText("Select operation mode");
    QPushButton *speedDensityButton = msgBox.addButton("Speed Density", QMessageBox::ActionRole);
    QPushButton *massAirFlowButton = msgBox.addButton("Mass Air Flow", QMessageBox::ActionRole);
    msgBox.exec();

    if (msgBox.clickedButton() == (QAbstractButton*)speedDensityButton)
        sdMode();

    else if (msgBox.clickedButton() == (QAbstractButton*)massAirFlowButton)
        mafMode();
}

void MainWindow::sdMode()
{
    airflowMode = 1;
    ui->veTableWidget->setEnabled(true);
    ui->mafTableWidget->setDisabled(true);
    ui->mafTableWidget->clearSelection();
}

void MainWindow::mafMode()
{
    airflowMode = 2;
    ui->mafTableWidget->setEnabled(true);
    ui->veTableWidget->setDisabled(true);
    ui->veTableWidget->clearSelection();
}
