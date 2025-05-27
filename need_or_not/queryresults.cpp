#include "qheaderview.h"
#include "queryresults.h"
#include <QStringList>
#include <QInputDialog>

QueryResults::QueryResults(QWidget *parent) : QDialog(parent) {
    setWindowTitle("Результаты запросов");

    dataTable = nullptr;
    resultTable = new QTableWidget(this);
    querySelector = new QComboBox(this);

    QStringList queries = {
        "1. Список студентов группы ИУК2-21Б",
        "2. Количество стипендиатов 3 курса факультета ИУК",
        "3. Группы с задолженностями",
        "4. Студенты 2 курса факультета МК по начальной букве фамилии",
        "5. Процент студентов 2 и 3 курсов",
        "6. Преподаватели кафедр групп",
        "7. Список старост"
    };
    querySelector->addItems(queries);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(querySelector);
    layout->addWidget(resultTable);

    connect(querySelector, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &QueryResults::executeQuery);

    setupTable();
}

void QueryResults::setData(QTableWidget *table) {
    dataTable = table;
}

void QueryResults::setupTable() {
    resultTable->setColumnCount(4);
    resultTable->setHorizontalHeaderLabels({"ФИО", "Куратор", "Староста", "Факультет"});
    resultTable->horizontalHeader()->setStretchLastSection(true);
    for (int i = 0; i < resultTable->columnCount(); ++i) {
        resultTable->horizontalHeader()->setSectionResizeMode(i, QHeaderView::Stretch);
    }
    resultTable->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

void QueryResults::executeQuery(int index) {
    resultTable->clearContents();
    resultTable->setRowCount(0);

    if (!dataTable) return;

    switch (index) {
    case 0: {
        resultTable->setColumnCount(4);
        resultTable->setHorizontalHeaderLabels({"ФИО", "Куратор", "Староста", "Факультет"});
        for (int row = 0; row < dataTable->rowCount(); ++row) {
            if (dataTable->item(row, 0)->text() == "ИУК2-21Б") {
                int newRow = resultTable->rowCount();
                resultTable->insertRow(newRow);
                resultTable->setItem(newRow, 0, new QTableWidgetItem(dataTable->item(row, 2)->text() + " " +
                                                                     dataTable->item(row, 3)->text() + " " +
                                                                     dataTable->item(row, 4)->text()));
                resultTable->setItem(newRow, 1, new QTableWidgetItem(dataTable->item(row, 7)->text()));
                resultTable->setItem(newRow, 2, new QTableWidgetItem(dataTable->item(row, 6)->text()));
                resultTable->setItem(newRow, 3, new QTableWidgetItem(dataTable->item(row, 9)->text()));
            }
        }
        break;
    }
    case 1: {
        resultTable->setColumnCount(4);
        resultTable->setHorizontalHeaderLabels({"ФИО", "Куратор", "Староста", "Факультет"});
        int count = 0;
        for (int row = 0; row < dataTable->rowCount(); ++row) {
            if (dataTable->item(row, 1)->text() == "3" && dataTable->item(row, 9)->text() == "ИУК" &&
                dataTable->item(row, 5)->text().toInt() > 0) {
                count++;
                int newRow = resultTable->rowCount();
                resultTable->insertRow(newRow);
                resultTable->setItem(newRow, 0, new QTableWidgetItem(dataTable->item(row, 2)->text() + " " +
                                                                     dataTable->item(row, 3)->text() + " " +
                                                                     dataTable->item(row, 4)->text()));
                resultTable->setItem(newRow, 1, new QTableWidgetItem(dataTable->item(row, 7)->text()));
                resultTable->setItem(newRow, 2, new QTableWidgetItem(dataTable->item(row, 6)->text()));
                resultTable->setItem(newRow, 3, new QTableWidgetItem(dataTable->item(row, 9)->text()));
            }
        }
        int totalRow = resultTable->rowCount();
        resultTable->insertRow(totalRow);
        resultTable->setItem(totalRow, 0, new QTableWidgetItem("Всего:"));
        resultTable->setItem(totalRow, 1, new QTableWidgetItem(QString::number(count)));
        break;
    }
    case 2: {
        resultTable->setColumnCount(4);
        resultTable->setHorizontalHeaderLabels({"Группа", "Курс", "Куратор", "Факультет"});

        QSet<QString> addedGroups;

        for (int row = 0; row < dataTable->rowCount(); ++row) {
            if (dataTable->item(row, 8)->text().toLower() == "да") {
                QString group = dataTable->item(row, 0)->text();
                if (!addedGroups.contains(group)) {
                    addedGroups.insert(group);
                    int newRow = resultTable->rowCount();
                    resultTable->insertRow(newRow);
                    resultTable->setItem(newRow, 0, new QTableWidgetItem(group));
                    resultTable->setItem(newRow, 1, new QTableWidgetItem(dataTable->item(row, 1)->text()));
                    resultTable->setItem(newRow, 2, new QTableWidgetItem(dataTable->item(row, 7)->text()));
                    resultTable->setItem(newRow, 3, new QTableWidgetItem(dataTable->item(row, 9)->text()));
                }
            }
        }
        break;
    }
    case 3: {
        QString letter = QInputDialog::getText(this, "Ввод", "Введите первую букву фамилии:");
        if (letter.isEmpty()) return;

        resultTable->setColumnCount(4);
        resultTable->setHorizontalHeaderLabels({"ФИО", "Куратор", "Староста", "Факультет"});
        for (int row = 0; row < dataTable->rowCount(); ++row) {
            if (dataTable->item(row, 1)->text() == "2" && dataTable->item(row, 9)->text() == "МК" &&
                dataTable->item(row, 2)->text().startsWith(letter, Qt::CaseInsensitive)) {
                int newRow = resultTable->rowCount();
                resultTable->insertRow(newRow);
                resultTable->setItem(newRow, 0, new QTableWidgetItem(dataTable->item(row, 2)->text() + " " +
                                                                     dataTable->item(row, 3)->text() + " " +
                                                                     dataTable->item(row, 4)->text()));
                resultTable->setItem(newRow, 1, new QTableWidgetItem(dataTable->item(row, 7)->text()));
                resultTable->setItem(newRow, 2, new QTableWidgetItem(dataTable->item(row, 6)->text()));
                resultTable->setItem(newRow, 3, new QTableWidgetItem(dataTable->item(row, 9)->text()));
            }
        }
        break;
    }
    case 4: {
        resultTable->setColumnCount(2);
        resultTable->setHorizontalHeaderLabels({"Курс", "Процент"});
        int totalStudents = 0, course2Count = 0, course3Count = 0;

        for (int row = 0; row < dataTable->rowCount(); ++row) {
            totalStudents++;
            if (dataTable->item(row, 1)->text() == "2") course2Count++;
            if (dataTable->item(row, 1)->text() == "3") course3Count++;
        }

        double percent2 = (double(course2Count) / totalStudents) * 100;
        double percent3 = (double(course3Count) / totalStudents) * 100;

        resultTable->insertRow(0);
        resultTable->setItem(0, 0, new QTableWidgetItem("2 курс"));
        resultTable->setItem(0, 1, new QTableWidgetItem(QString::number(percent2, 'f', 2) + "%"));

        resultTable->insertRow(1);
        resultTable->setItem(1, 0, new QTableWidgetItem("3 курс"));
        resultTable->setItem(1, 1, new QTableWidgetItem(QString::number(percent3, 'f', 2) + "%"));
        break;
    }
    case 5: {
        resultTable->setColumnCount(2);
        resultTable->setHorizontalHeaderLabels({"Группа", "Преподаватель"});
        QMap<QString, QString> groupTeachers;

        for (int row = 0; row < dataTable->rowCount(); ++row) {
            QString group = dataTable->item(row, 0)->text();
            QString teacher = dataTable->item(row, 7)->text();
            groupTeachers[group] = teacher;
        }

        int row = 0;
        for (auto it = groupTeachers.begin(); it != groupTeachers.end(); ++it) {
            resultTable->insertRow(row);
            resultTable->setItem(row, 0, new QTableWidgetItem(it.key()));
            resultTable->setItem(row, 1, new QTableWidgetItem(it.value()));
            row++;
        }
        break;
    }
    case 6: {
        resultTable->setColumnCount(2);
        resultTable->setHorizontalHeaderLabels({"ФИО", "Группа"});
        for (int row = 0; row < dataTable->rowCount(); ++row) {
            if (!dataTable->item(row, 6)->text().isEmpty()) {
                int newRow = resultTable->rowCount();
                resultTable->insertRow(newRow);
                resultTable->setItem(newRow, 0, new QTableWidgetItem(dataTable->item(row, 2)->text() + " " +
                                                                     dataTable->item(row, 3)->text() + " " +
                                                                     dataTable->item(row, 4)->text()));
                resultTable->setItem(newRow, 1, new QTableWidgetItem(dataTable->item(row, 0)->text()));
            }
        }
        break;
    }
    }
}
