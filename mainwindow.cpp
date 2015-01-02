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
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::importLog()
{
    qDebug() << "import!";
}
