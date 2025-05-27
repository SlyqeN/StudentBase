#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ui_mainwindow.h"
#include <QMainWindow>
#include <QTableWidgetItem>
#include <QDialog>
#include <QLineEdit>
#include <QFormLayout>
#include <QCheckBox>
#include <QWidgetAction>
#include <QMenu>
#include <QComboBox>
#include <QStringList>
#include <QSet>
#include <QRegularExpression>


class NumericTableWidgetItem : public QTableWidgetItem {
public:
    bool operator<(const QTableWidgetItem &other) const override {
        return this->data(Qt::UserRole).toInt() < other.data(Qt::UserRole).toInt();
    }
};


class GroupTableWidgetItem : public QTableWidgetItem {
public:
    bool operator<(const QTableWidgetItem &other) const override {
        QString thisText = text().toLower();
        QString otherText = other.text().toLower();

        QRegularExpression re("^(\\d+)([а-яa-z]*)$");
        auto thisMatch = re.match(thisText);
        auto otherMatch = re.match(otherText);

        if (thisMatch.hasMatch() && otherMatch.hasMatch()) {
            int thisNum = thisMatch.captured(1).toInt();
            int otherNum = otherMatch.captured(1).toInt();

            if (thisNum != otherNum) {
                return thisNum < otherNum;
            }
            return thisMatch.captured(2) < otherMatch.captured(2);
        }
        return QTableWidgetItem::operator<(other);
    }
};

/**
 * @brief Диалог для добавления нового студента.
 * Позволяет ввести все необходимые данные о студенте: группу, курс, ФИО, стипендию, старосту, куратора, задолженность и факультет.
 * При наличии задолженности поле стипендии блокируется и устанавливается в 0.
 */
class AddStudentDialog : public QDialog {
    Q_OBJECT

public:
    /**
     * @brief Конструктор диалога добавления студента
     * @param parent Родительский виджет
     */
    explicit AddStudentDialog(QWidget *parent = nullptr);

    /**
     * @brief Получить введённые пользователем данные
     * @return Список строк: группа, курс, фамилия, имя, отчество, стипендия, староста, куратор, задолженность, факультет
     */
    QStringList getData() const;

private slots:
    /**
     * @brief Слот для обработки изменения значения задолженности
     * Если выбрано "Да", поле стипендии блокируется и устанавливается в 0
     * @param index Индекс выбранного значения в comboDebt
     */
    void onDebtChanged(int index);
    void onAddClicked();

private:
    QLineEdit *editGroup;        ///< Поле для ввода группы
    QComboBox *comboCourse;      ///< Выпадающий список для выбора курса
    QLineEdit *editSurname;      ///< Поле для ввода фамилии
    QLineEdit *editName;         ///< Поле для ввода имени
    QLineEdit *editPatronymic;   ///< Поле для ввода отчества
    QLineEdit *editScholarship;  ///< Поле для ввода стипендии
    QLineEdit *editMonitor;      ///< Поле для ввода старосты
    QLineEdit *editCurator;      ///< Поле для ввода куратора
    QComboBox *comboDebt;        ///< Выпадающий список для выбора наличия задолженности
    QLineEdit *editFaculty;      ///< Поле для ввода факультета

    bool validateInput(QString &errorMsg) const;
};

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onCourseHeaderClicked(int logicalIndex);
    void updateCourseFilter();
    void onLoadClicked();
    void onAddClicked();
    void onDeleteClicked();
    void onSaveClicked();
    void onHeaderClicked(int logicalIndex);
    void onQueriesClicked();
    void onChartsClicked();

private:
    Ui::MainWindow *ui;
    QMenu *courseMenu;
    QList<QCheckBox*> courseCheckboxes;
    QSet<int> selectedCourses;
    QString currentFile;
    Qt::SortOrder sortOrderGroup = Qt::AscendingOrder;
    Qt::SortOrder sortOrderSurname = Qt::AscendingOrder;
    Qt::SortOrder sortOrderDebt = Qt::AscendingOrder;
    Qt::SortOrder sortOrderFaculty = Qt::AscendingOrder;

    void setupCourseFilter();
    void filterTable();
    void loadData(const QString &filename);
    void saveData(const QString &filename);
    void addStudent(const QStringList &data);
    bool validateData(const QStringList &data);
    void updateHeaderArrow(int column, Qt::SortOrder order);
};

#endif
