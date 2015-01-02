#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "cvsimport.h"
#include <QMainWindow>
#include <QTableWidget>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void importCvs();

private:
    Ui::MainWindow *ui;
    CvsImport *cvs;
};

#endif // MAINWINDOW_H
