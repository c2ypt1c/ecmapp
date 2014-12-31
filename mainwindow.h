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
    void initMafTable();
    void initVETable();
    void veCreateActions();

public slots:
    void importLog();
    void veRightClick(QPoint);
    void veCopy();
    void vePaste();
    void veLoadDefault();

private:
    void veSetCell(QTableWidgetItem *, QString);
    Ui::MainWindow *ui;
    QTableWidget *mafTable;
    QTableWidget *veTable;

    // actions
    QAction *veCopyAction;
    QAction *vePasteAction;
    QAction *veLoadDefaultAction;
};

#endif // MAINWINDOW_H
