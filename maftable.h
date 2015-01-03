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
    void mafCreateActions();
    void calcAffectedCells();

    bool fileImported = false;
    QList<float> mafRawList;
    QList<float> wbfList;
    QList<int> affectedRows;

public slots:
    void mafUpdate(QTableWidgetItem *);
    void mafRightClick(QPoint);
    void mafCopy();
    void mafPaste();
    void mafShowAffectedCells();
    void mafApplyCorrections();


private:
    QTableWidget *mafTable;

    QAction *mafCopyAction;
    QAction *mafPasteAction;
    QAction *mafShowAffectedAction;
    QAction *mafApplyCorrectionsAction;
};

#endif // MAFTABLE_H
