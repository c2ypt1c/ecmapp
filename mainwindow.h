#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableWidget>
#include <QFile>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void parseCsv(QString);
    void setMode(QString);
    int findAirflowIndex(QStringList);
    int findWBFactorIndex(QStringList);
    void parseData(QFile *, int, int);
    QStringList getVEList();
    QStringList getMAFRawList();
    QStringList getWBFactorList();

public slots:
    void importCvs();
    void sdMode();
    void mafMode();

private:
    Ui::MainWindow *ui;
    QFile *csvFile;

    int airflowMode;
    bool fileImported;
    QStringList MAFRawList;
    QStringList VEList;
    QStringList WBFactorList;
};

#endif // MAINWINDOW_H
