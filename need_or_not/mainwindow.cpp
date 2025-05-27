#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QHeaderView>
#include <QTextStream>
#include <QRegularExpression>
#include "queryresults.h"
#include "chartwindow.h"

AddStudentDialog::AddStudentDialog(QWidget *parent) : QDialog(parent) {
    QFormLayout *layout = new QFormLayout(this);

    editGroup = new QLineEdit(this);
    comboCourse = new QComboBox(this);
    for (int i = 1; i <= 6; ++i) {
        comboCourse->addItem(QString::number(i));
    }
    editSurname = new QLineEdit(this);
    editName = new QLineEdit(this);
    editPatronymic = new QLineEdit(this);
    editScholarship = new QLineEdit(this);
    editMonitor = new QLineEdit(this);
    editCurator = new QLineEdit(this);

    comboDebt = new QComboBox(this);
    comboDebt->addItems({"Нет", "Да"});
    connect(comboDebt, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &AddStudentDialog::onDebtChanged);

    editFaculty = new QLineEdit(this);

    layout->addRow("Группа:", editGroup);
    layout->addRow("Курс:", comboCourse);
    layout->addRow("Фамилия:", editSurname);
    layout->addRow("Имя:", editName);
    layout->addRow("Отчество:", editPatronymic);
    layout->addRow("Стипендия:", editScholarship);
    layout->addRow("Староста:", editMonitor);
    layout->addRow("Куратор:", editCurator);
    layout->addRow("Задолженность:", comboDebt);
    layout->addRow("Факультет:", editFaculty);

    QPushButton *btnOk = new QPushButton("Добавить", this);
    connect(btnOk, &QPushButton::clicked, this, &AddStudentDialog::onAddClicked);
    layout->addRow(btnOk);
}

void AddStudentDialog::onDebtChanged(int index) {
    if (index == 1) {
        editScholarship->setText("0");
        editScholarship->setEnabled(false);
    } else {
        editScholarship->setEnabled(true);
    }
}

QStringList AddStudentDialog::getData() const {
    return {
        editGroup->text(),
        comboCourse->currentText(),
        editSurname->text(),
        editName->text(),
        editPatronymic->text(),
        editScholarship->text(),
        editMonitor->text(),
        editCurator->text(),
        comboDebt->currentText(),
        editFaculty->text()
    };
}

void AddStudentDialog::onAddClicked() {
    QString errorMsg;
    if (validateInput(errorMsg)) {
        accept();
    } else {
        QMessageBox::critical(this, "Ошибка ввода", errorMsg);
    }
}

bool AddStudentDialog::validateInput(QString &errorMsg) const {
    // Группа: любые символы, не пусто
    if (editGroup->text().trimmed().isEmpty()) {
        errorMsg = "Поле 'Группа' не должно быть пустым.";
        return false;
    }
    // Фамилия, Имя, Отчество: только буквы
    QRegularExpression reLetters("^[А-Яа-яA-Za-zЁё]+$");
    if (!reLetters.match(editSurname->text().trimmed()).hasMatch()) {
        errorMsg = "Поле 'Фамилия' должно содержать только буквы.";
        return false;
    }
    if (!reLetters.match(editName->text().trimmed()).hasMatch()) {
        errorMsg = "Поле 'Имя' должно содержать только буквы.";
        return false;
    }
    if (!reLetters.match(editPatronymic->text().trimmed()).hasMatch()) {
        errorMsg = "Поле 'Отчество' должно содержать только буквы.";
        return false;
    }
    // Стипендия: только цифры
    QRegularExpression reDigits("^\\d+$");
    if (!reDigits.match(editScholarship->text().trimmed()).hasMatch()) {
        errorMsg = "Поле 'Стипендия' должно содержать только цифры.";
        return false;
    }
    // Староста: только буквы (может быть пустым)
    QString monitor = editMonitor->text().trimmed();
    if (!monitor.isEmpty() && !reLetters.match(monitor).hasMatch()) {
        errorMsg = "Поле 'Староста' должно содержать только буквы или быть пустым.";
        return false;
    }
    // Куратор: только буквы (может быть пустым)
    QString curator = editCurator->text().trimmed();
    if (!curator.isEmpty() && !reLetters.match(curator).hasMatch()) {
        errorMsg = "Поле 'Куратор' должно содержать только буквы или быть пустым.";
        return false;
    }
    // Факультет: только буквы
    if (!reLetters.match(editFaculty->text().trimmed()).hasMatch()) {
        errorMsg = "Поле 'Факультет' должно содержать только буквы.";
        return false;
    }
    return true;
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    QStringList headers = {"Группа", "Курс •", "Фамилия", "Имя", "Отчество",
                         "Стипендия", "Староста", "Куратор", "Задолженность", "Факультет"};
    ui->tableWidget->setColumnCount(headers.size());
    ui->tableWidget->setHorizontalHeaderLabels(headers);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableWidget->setSortingEnabled(true);

    connect(ui->tableWidget->horizontalHeader(), &QHeaderView::sectionClicked,
            this, &MainWindow::onHeaderClicked);
    connect(ui->btnLoad, &QPushButton::clicked, this, &MainWindow::onLoadClicked);
    connect(ui->btnAdd, &QPushButton::clicked, this, &MainWindow::onAddClicked);
    connect(ui->btnDelete, &QPushButton::clicked, this, &MainWindow::onDeleteClicked);
    connect(ui->btnSave, &QPushButton::clicked, this, &MainWindow::onSaveClicked);
    connect(ui->btnQueries, &QPushButton::clicked, this, &MainWindow::onQueriesClicked);
    connect(ui->btnCharts, &QPushButton::clicked, this, &MainWindow::onChartsClicked);

    setupCourseFilter();
}

void MainWindow::onHeaderClicked(int logicalIndex) {
    ui->tableWidget->setSortingEnabled(false);
    if (logicalIndex == 0) {
        sortOrderGroup = (sortOrderGroup == Qt::AscendingOrder) ? Qt::DescendingOrder : Qt::AscendingOrder;
        ui->tableWidget->sortByColumn(0, sortOrderGroup);
        updateHeaderArrow(0, sortOrderGroup);
    } else if (logicalIndex == 2) {
        sortOrderSurname = (sortOrderSurname == Qt::AscendingOrder) ? Qt::DescendingOrder : Qt::AscendingOrder;
        ui->tableWidget->sortByColumn(2, sortOrderSurname);
        updateHeaderArrow(2, sortOrderSurname);
    } else if (logicalIndex == 8) {
        sortOrderDebt = (sortOrderDebt == Qt::AscendingOrder) ? Qt::DescendingOrder : Qt::AscendingOrder;
        ui->tableWidget->sortByColumn(8, sortOrderDebt);
        updateHeaderArrow(8, sortOrderDebt);
    } else if (logicalIndex == 9) {
        sortOrderFaculty = (sortOrderFaculty == Qt::AscendingOrder) ? Qt::DescendingOrder : Qt::AscendingOrder;
        ui->tableWidget->sortByColumn(9, sortOrderFaculty);
        updateHeaderArrow(9, sortOrderFaculty);
    } else if (logicalIndex == 1) {
        for (int row = 0; row < ui->tableWidget->rowCount(); ++row) {
            auto item = ui->tableWidget->item(row, 1);
            item->setData(Qt::UserRole, item->text().toInt());
        }
        ui->tableWidget->sortByColumn(1, Qt::AscendingOrder);
    }
    else {
        ui->tableWidget->setSortingEnabled(false);
    }
}

void MainWindow::updateHeaderArrow(int column, Qt::SortOrder order) {
    QString arrow = (order == Qt::AscendingOrder) ? "↑" : "↓";
    QString headerText = ui->tableWidget->horizontalHeaderItem(column)->text().split(" ").first();
    ui->tableWidget->horizontalHeaderItem(column)->setText(headerText + " " + arrow);
}

void MainWindow::setupCourseFilter() {
    courseMenu = new QMenu(this);
    for (int i = 1; i <= 6; ++i) {
        QCheckBox *checkBox = new QCheckBox(QString("Курс %1").arg(i), courseMenu);
        checkBox->setChecked(true);
        connect(checkBox, &QCheckBox::stateChanged, this, &MainWindow::updateCourseFilter);
        QWidgetAction *action = new QWidgetAction(courseMenu);
        action->setDefaultWidget(checkBox);
        courseMenu->addAction(action);
        courseCheckboxes.append(checkBox);
        selectedCourses.insert(i);
    }
    connect(ui->tableWidget->horizontalHeader(), &QHeaderView::sectionClicked,
            this, &MainWindow::onCourseHeaderClicked);
}

void MainWindow::onCourseHeaderClicked(int logicalIndex) {
    if (logicalIndex == 1) {
        QPoint pos = ui->tableWidget->horizontalHeader()->mapToGlobal(
            QPoint(ui->tableWidget->horizontalHeader()->sectionViewportPosition(1), 0));
        courseMenu->exec(pos);
    }
}

void MainWindow::updateCourseFilter() {
    selectedCourses.clear();
    for (QCheckBox *cb : courseCheckboxes) {
        if (cb->isChecked()) selectedCourses.insert(cb->text().split(" ").last().toInt());
    }
    filterTable();
}

void MainWindow::filterTable() {
    for (int row = 0; row < ui->tableWidget->rowCount(); ++row) {
        bool visible = selectedCourses.contains(ui->tableWidget->item(row, 1)->text().toInt());
        ui->tableWidget->setRowHidden(row, !visible);
    }
}

void MainWindow::onLoadClicked() {
    QString filename = QFileDialog::getOpenFileName(this, "Выберите файл", "", "Текстовые файлы (*.txt)");
    if (!filename.isEmpty()) loadData(filename);
}

void MainWindow::loadData(const QString &filename) {
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::critical(this, "Ошибка", "Не удалось открыть файл");
        return;
    }
    ui->tableWidget->setRowCount(0);
    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList data = line.split(" ");
        if (data.size() == 10 && validateData(data)) addStudent(data);
    }
    file.close();
    currentFile = filename;
}

void MainWindow::onAddClicked() {
    AddStudentDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        QStringList data = dialog.getData();
        if (validateData(data)) {
            addStudent(data);
        } else {
            QMessageBox::critical(this, "Ошибка", "Некорректные данные!");
        }
    }
}

bool MainWindow::validateData(const QStringList &data) {
    bool ok;
    data[1].toInt(&ok);
    if (!ok || data[1].toInt() < 1 || data[1].toInt() > 6) return false;
    data[5].toInt(&ok);
    if (!ok) return false;
    return !data[9].isEmpty();
}

void MainWindow::addStudent(const QStringList &data) {
    int row = ui->tableWidget->rowCount();
    ui->tableWidget->insertRow(row);
    for (int i = 0; i < data.size(); ++i) {
        QTableWidgetItem *item;
        if (i == 0) {
            item = new GroupTableWidgetItem();
            item->setText(data[i]);
        } else if (i == 1 || i == 8 || i == 9) {
            item = new QTableWidgetItem(data[i]);
            item->setTextAlignment(Qt::AlignCenter);
        } else {
            item = new QTableWidgetItem(data[i]);
        }

        if (i == 0 || i == 1 || i == 5 || i == 8 || i == 9) {
            item->setFlags(item->flags() | Qt::ItemIsEditable);
        } else {
            item->setFlags(item->flags() & ~Qt::ItemIsEditable);
        }
        ui->tableWidget->setItem(row, i, item);
    }
}

void MainWindow::onDeleteClicked() {
    int row = ui->tableWidget->currentRow();
    if (row >= 0) ui->tableWidget->removeRow(row);
}

void MainWindow::onSaveClicked() {
    QString filename = QFileDialog::getSaveFileName(
        this,
        "Сохранить файл",
        "",
        "Текстовые файлы (*.txt)"
    );
    if (!filename.isEmpty()) saveData(filename);
}

void MainWindow::saveData(const QString &filename) {
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::critical(this, "Ошибка", "Не удалось сохранить файл");
        return;
    }
    QTextStream out(&file);
    for (int row = 0; row < ui->tableWidget->rowCount(); ++row) {
        QStringList data;
        for (int col = 0; col < ui->tableWidget->columnCount(); ++col) {
            data << ui->tableWidget->item(row, col)->text();
        }
        out << data.join(" ") << "\n";
    }
    file.close();
}

void MainWindow::onQueriesClicked() {
    QueryResults queryWindow(this);
    queryWindow.setData(ui->tableWidget);
    queryWindow.exec();
}

void MainWindow::onChartsClicked() {
    ChartWindow chartWindow(this);
    chartWindow.setData(ui->tableWidget);
    chartWindow.exec();
}

MainWindow::~MainWindow() {
    delete ui;
}
