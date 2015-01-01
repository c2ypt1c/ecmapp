#ifndef VETABLE_H
#define VETABLE_H

#include <QTableWidget>


class VeTable: public QTableWidget
{
    Q_OBJECT

public:
    VeTable(QWidget *parent = 0);
    ~VeTable();

    void initVETable();
    void veCreateActions();

public slots:
    void veUpdate(QTableWidgetItem *);
    void veRightClick(QPoint);
    void veCopy();
    void vePaste();
    void veLoadDefault();

private:
    QTableWidget *veTable;

    void veSetCell(QTableWidgetItem *, QString);

    QAction *veCopyAction;
    QAction *vePasteAction;
    QAction *veLoadDefaultAction;
};

#endif // VETABLE_H
