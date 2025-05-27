#ifndef CHARTWINDOW_H
#define CHARTWINDOW_H

#include <QDialog>
#include <QTableWidget>
#include <QComboBox>
#include <QPushButton>
#include <QtCharts/QChartView>
#include <QtCharts/QPieSeries>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QValueAxis>

QT_CHARTS_USE_NAMESPACE

class ChartWindow : public QDialog {
    Q_OBJECT

public:
    explicit ChartWindow(QWidget *parent = nullptr);
    void setData(QTableWidget *table);

private slots:
    void updateChart();

private:
    QTableWidget *dataTable;

    QChartView *chartView;
    QComboBox *comboBoxChartType;
    QPushButton *btnShowChart;

    QChart* createPieChartCourseDistribution();
    QChart* createPieChartThirdCoursePercentage();
    QChart* createBarChartScholarship();
};

#endif
