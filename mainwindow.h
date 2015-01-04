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
    void setMode();

public slots:
    void importCsv();
    void sdMode();
    void mafMode();

private:
    Ui::MainWindow *ui;
    int airflowMode;
};

#endif // MAINWINDOW_H
