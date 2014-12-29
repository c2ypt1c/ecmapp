#ifndef MAINWINDOW_H
#define MAINWINDOW_H

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
    void initVETable();
    void veCreateActions();

public slots:
    void veRightClick(QPoint);
    void veCopy();
    void vePaste();
    void veLoad2g();
    //void veLoad1g();
    //void veLoadEvo();

private:
    Ui::MainWindow *ui;
    QTableWidget *veTable;

    // actions
    QAction *veCopyAction;
    QAction *vePasteAction;
    QAction *veLoad2gAction;
    QAction *veLoad1gAction;
    QAction *veLoadEvoAction;
};

#endif // MAINWINDOW_H
