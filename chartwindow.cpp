#include "chartwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QComboBox>
#include <QPushButton>
#include <QMap>
#include <QStringList>

ChartWindow::ChartWindow(QWidget *parent) : QDialog(parent) {
    setWindowTitle("Диаграммы");
    dataTable = nullptr;

    comboBoxChartType = new QComboBox(this);
    comboBoxChartType->addItem("Круговая по курсам");
    comboBoxChartType->addItem("Круговая по 3 курсу");
    comboBoxChartType->addItem("Столбчатая по стипендии");

    btnShowChart = new QPushButton("Показать", this);

    chartView = new QChartView(this);
    chartView->setRenderHint(QPainter::Antialiasing);

    QHBoxLayout *controlsLayout = new QHBoxLayout();
    controlsLayout->addWidget(comboBoxChartType);
    controlsLayout->addWidget(btnShowChart);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(controlsLayout);
    mainLayout->addWidget(chartView);

    setLayout(mainLayout);

    connect(btnShowChart, &QPushButton::clicked, this, &ChartWindow::updateChart);
}

void ChartWindow::setData(QTableWidget *table) {
    dataTable = table;
    updateChart();
}

void ChartWindow::updateChart() {
    if (!dataTable) return;

    QString selected = comboBoxChartType->currentText();
    QChart *chart = nullptr;

    if (selected == "Круговая по курсам") {
        chart = createPieChartCourseDistribution();
    } else if (selected == "Круговая по 3 курсу") {
        chart = createPieChartThirdCoursePercentage();
    } else if (selected == "Столбчатая по стипендии") {
        chart = createBarChartScholarship();
    }

    if (chart) {
        chartView->setChart(chart);
    }
}

QChart* ChartWindow::createPieChartCourseDistribution() {
    QMap<int, int> courseCounts;
    int totalStudents = 0;

    for (int row = 0; row < dataTable->rowCount(); ++row) {
        int course = dataTable->item(row, 1)->text().toInt();
        courseCounts[course]++;
        totalStudents++;
    }

    auto *series = new QPieSeries();

    for (auto it = courseCounts.begin(); it != courseCounts.end(); ++it) {
        double percent = double(it.value()) / totalStudents * 100;
        QString label = QString("Курс %1\n%2%").arg(it.key()).arg(percent, 0, 'f', 1);

        auto slice = series->append(label, it.value());
        slice->setLabelVisible(true);
        slice->setLabelPosition(QPieSlice::LabelOutside);
        slice->setPen(QPen(Qt::black, 2));
        slice->setLabelFont(QFont("Segoe UI", 10, QFont::Bold));

        QColor color;
        switch (it.key()) {
        case 1: color = QColor(255, 99, 132); break;
        case 2: color = QColor(54, 162, 235); break;
        case 3: color = QColor(75, 192, 192); break;
        case 4: color = QColor(255, 206, 86); break;
        default: color = QColor(200, 200, 200); break;
        }
        slice->setBrush(color);

        connect(slice, &QPieSlice::hovered, this, [slice](bool state) {
            slice->setExploded(state);
            slice->setExplodeDistanceFactor(state ? 0.12 : 0.0);
        });
    }

    auto *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("\xf0\x9f\x8e\x93 Распределение студентов по курсам");
    chart->setAnimationOptions(QChart::AllAnimations);
    chart->legend()->hide();
    chart->setTitleFont(QFont("Segoe UI", 12, QFont::Bold));
    return chart;
}

QChart* ChartWindow::createPieChartThirdCoursePercentage() {
    int thirdCourseCount = 0;
    int totalStudents = 0;

    for (int row = 0; row < dataTable->rowCount(); ++row) {
        int course = dataTable->item(row, 1)->text().toInt();
        if (course == 3)
            thirdCourseCount++;
        totalStudents++;
    }

    double percent = (double(thirdCourseCount) / totalStudents) * 100;

    auto *series = new QPieSeries();
    auto slice1 = series->append(QString("3 курс\n%1%").arg(percent, 0, 'f', 1), thirdCourseCount);
    auto slice2 = series->append(QString("Остальные\n%1%").arg(100 - percent, 0, 'f', 1), totalStudents - thirdCourseCount);

    slice1->setBrush(QColor(34, 139, 34));
    slice2->setBrush(QColor(180, 180, 180));

    for (auto slice : series->slices()) {
        slice->setLabelVisible(true);
        slice->setLabelPosition(QPieSlice::LabelOutside);
        slice->setLabelFont(QFont("Segoe UI", 10, QFont::Bold));
        slice->setPen(QPen(Qt::black, 2));

        connect(slice, &QPieSlice::hovered, this, [slice](bool state) {
            slice->setExploded(state);
            slice->setExplodeDistanceFactor(state ? 0.12 : 0.0);
        });
    }

    auto *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("\xf0\x9f\x9f\xa2 Процент студентов 3 курса");
    chart->setAnimationOptions(QChart::AllAnimations);
    chart->legend()->hide();
    chart->setTitleFont(QFont("Segoe UI", 12, QFont::Bold));
    return chart;
}

QChart* ChartWindow::createBarChartScholarship() {
    QMap<int, double> courseScholarships;

    for (int row = 0; row < dataTable->rowCount(); ++row) {
        int course = dataTable->item(row, 1)->text().toInt();
        bool ok;
        double scholarship = dataTable->item(row, 5)->text().toDouble(&ok);
        if (ok) courseScholarships[course] += scholarship;
    }

    auto *set = new QBarSet("Стипендия");
    QStringList categories;
    for (auto it = courseScholarships.begin(); it != courseScholarships.end(); ++it) {
        categories << QString("Курс %1").arg(it.key());
        *set << it.value();
    }

    auto *series = new QBarSeries();
    series->append(set);
    set->setColor(QColor(255, 99, 71));

    auto *axisX = new QBarCategoryAxis();
    axisX->append(categories);

    auto *axisY = new QValueAxis();
    axisY->setTitleText("Сумма стипендии");

    auto *chart = new QChart();
    chart->addSeries(series);
    chart->addAxis(axisX, Qt::AlignBottom);
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisX);
    series->attachAxis(axisY);
    chart->setTitle("Суммарные стипендии по курсам");
    chart->legend()->setAlignment(Qt::AlignBottom);
    return chart;
}
