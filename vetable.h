#ifndef VETABLE_H
#define VETABLE_H

#include <QTableWidget>
#include <QVector>

class VeTable: public QTableWidget
{
    Q_OBJECT

public:
    VeTable(QWidget *parent = 0);
    ~VeTable();

    void initVETable();
    void createActions();

    QList<float> veList;
    QList<float> rpmList;
    QList<float> psiList;
    QList<float> wbfList;

    QList<QList<float>> rpmPercents;
    QList<float> rpmIndecies;
    QList<QList<float>> psiPercents;
    QList<float> psiIndecies;


public slots:
    void updateCell(QTableWidgetItem *);
    void rightClick(QPoint);
    void copy();
    void paste();
    void loadDefault();
    void showAffectedCells();

private:
    QTableWidget *veTable;

    void setCell(QTableWidgetItem *, QString);

    QAction *copyAction;
    QAction *pasteAction;
    QAction *loadDefaultAction;
    QAction *showAffectedAction;
};

#endif // VETABLE_H
