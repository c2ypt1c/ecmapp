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
    void sdMode();
    void mafMode();

public slots:
    void importCvs();

private:
    Ui::MainWindow *ui;
    QFile *csvFile;
};

#endif // MAINWINDOW_H
