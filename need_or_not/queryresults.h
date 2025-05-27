#ifndef QUERYRESULTS_H
#define QUERYRESULTS_H

#include <QDialog>
#include <QTableWidget>
#include <QComboBox>
#include <QVBoxLayout>

class QueryResults : public QDialog {
    Q_OBJECT
public:
    explicit QueryResults(QWidget *parent = nullptr);
    void setData(QTableWidget *table);

private slots:
    void executeQuery(int index);
    void setupTable();

private:
    QTableWidget *dataTable;
    QTableWidget *resultTable;
    QComboBox *querySelector;
};

#endif
