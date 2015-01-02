#ifndef MAFTABLE_H
#define MAFTABLE_H

#include <QTableWidget>

class MafTable : public QTableWidget
{
    Q_OBJECT

public:
    MafTable(QWidget *parent = 0);
    void initMafTable();
    void mafCreateActions();
    ~MafTable();

public slots:
    void mafRightClick(QPoint);
    void mafCopy();
    void mafPaste();

private:
    QTableWidget *mafTable;

    QAction *mafCopyAction;
    QAction *mafPasteAction;
};

#endif // MAFTABLE_H
