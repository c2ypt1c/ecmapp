#ifndef MAFTABLE_H
#define MAFTABLE_H

#include <QTableWidget>

class MafTable : public QTableWidget
{
    Q_OBJECT

public:
    MafTable(QWidget *parent = 0);
    void initMafTable();
    ~MafTable();

private:
    QTableWidget *mafTable;
};

#endif // MAFTABLE_H
