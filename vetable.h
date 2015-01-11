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
    void fileLoaded();

    QList<float> *veList;
    QList<float> *rpmList;
    QList<float> *psiList;
    QList<float> *wbfList;

    QList<QList<float>> *vePercentsBox;
    QList<QList<float>> *veValuesBox;
    QList<QList<QTableWidgetItem *>> *veItemsBox;

    QList<float> *rpmHeadIndecies;
    QList<float> *psiHeadIndecies;

    QList<float> *avgWbfList;

public slots:
    void updateCell(QTableWidgetItem *);
    void rightClick(QPoint);
    void copy();
    void paste();
    void loadDefault();
    void showAffectedCells();
    void applyCorrections();

private:
    QTableWidget *veTable;

    void setCell(QTableWidgetItem *, QString);
    void averageWbfactor();

    QAction *copyAction;
    QAction *pasteAction;
    QAction *loadDefaultAction;
    QAction *showAffectedAction;
    QAction *applyCorrectionsAction;
};

#endif // VETABLE_H
