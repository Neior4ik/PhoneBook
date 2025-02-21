#pragma once
#include <QMainWindow>
#include <QTableWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <memory>
#include "phonebook.h"
#include "contactdialog.h"
#include "istorage.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(std::unique_ptr<IStorage> storage, QWidget* parent = nullptr);

private slots:
    void onAdd();
    void onEdit();
    void onDelete();
    void onSearch();
    void updateTable();

private:
    std::unique_ptr<PhoneBook> phoneBook;
    QTableWidget* contactsTable;
    QLineEdit* searchEdit;
    QPushButton* addButton;
    QPushButton* editButton;
    QPushButton* deleteButton;

    void setupUI();
};