// mainwindow.cpp
#include "mainwindow.h"
#include "contactdialog.h"
#include <QMessageBox>
#include <QGuiApplication>
#include <QScreen>
#include <QRect>
#include <QDebug>
#include <QHeaderView>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QCoreApplication>
#include <algorithm>

MainWindow::MainWindow(std::unique_ptr<IStorage> storage, QWidget *parent)
    : QMainWindow(parent)
{
    setupUI();
    
    // Инициализируем телефонную книгу
    phoneBook = std::make_unique<PhoneBook>(std::move(storage));
    
    // Обновляем таблицу
    updateTable();
    
    // Подключаем сигналы
    connect(searchEdit, &QLineEdit::textChanged, this, &MainWindow::onSearch);
}

void MainWindow::setupUI() {
    auto mainLayout = new QVBoxLayout;
    auto widget = new QWidget(this);
    widget->setLayout(mainLayout);
    setCentralWidget(widget);
    
    // Search
    auto searchLayout = new QHBoxLayout;
    searchEdit = new QLineEdit(this);
    searchEdit->setPlaceholderText("Поиск...");
    searchLayout->addWidget(searchEdit);
    mainLayout->addLayout(searchLayout);
    
    // Table
    contactsTable = new QTableWidget(this);
    contactsTable->setColumnCount(7);
    contactsTable->setHorizontalHeaderLabels({
        "Фамилия", "Имя", "Отчество", "Дата рождения", 
        "Адрес", "Email", "Телефоны"
    });
    contactsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    mainLayout->addWidget(contactsTable);
    
    // Buttons
    auto buttonLayout = new QHBoxLayout;
    auto addButton = new QPushButton("Добавить", this);
    auto editButton = new QPushButton("Изменить", this);
    auto deleteButton = new QPushButton("Удалить", this);
    // auto statsButton = new QPushButton("Статистика", this);
    
    buttonLayout->addWidget(addButton);
    buttonLayout->addWidget(editButton);
    buttonLayout->addWidget(deleteButton);
    // buttonLayout->addWidget(statsButton);
    mainLayout->addLayout(buttonLayout);
    
    // Connections
    connect(addButton, &QPushButton::clicked, this, &MainWindow::onAdd);
    connect(editButton, &QPushButton::clicked, this, &MainWindow::onEdit);
    connect(deleteButton, &QPushButton::clicked, this, &MainWindow::onDelete);
    // connect(statsButton, &QPushButton::clicked, this, [this]() {
    //     QMessageBox::information(this, "Статистика", Contact::getStats());
    // });
    
    setWindowTitle("Телефонная книга");
    resize(1400, 600);

    // Центрируем окно
    QRect screenGeometry = QGuiApplication::primaryScreen()->geometry();
    int x = (screenGeometry.width() - width()) / 2;
    int y = (screenGeometry.height() - height()) / 2;
    move(x, y);

    // Включаем сортировку
    contactsTable->setSortingEnabled(true);
    
    // Устанавливаем сортировку по умолчанию по фамилии
    contactsTable->sortByColumn(0, Qt::AscendingOrder);
}

void MainWindow::updateTable() {
    contactsTable->setSortingEnabled(false);  // Отключаем сортировку на время обновления
    contactsTable->setRowCount(0);
    
    auto contacts = phoneBook->getContacts();
    for (const auto& contact : contacts) {
        int row = contactsTable->rowCount();
        contactsTable->insertRow(row);
        
        auto* lastNameItem = new QTableWidgetItem(QString::fromStdString(contact.getLastName()));
        lastNameItem->setData(Qt::UserRole, contact.getId());
        contactsTable->setItem(row, 0, lastNameItem);
        
        contactsTable->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(contact.getFirstName())));
        contactsTable->setItem(row, 2, new QTableWidgetItem(QString::fromStdString(contact.getMiddleName())));
        contactsTable->setItem(row, 3, new QTableWidgetItem(QString::fromStdString(contact.getBirthDate())));
        contactsTable->setItem(row, 4, new QTableWidgetItem(QString::fromStdString(contact.getAddress())));
        contactsTable->setItem(row, 5, new QTableWidgetItem(QString::fromStdString(contact.getEmail())));
        
        QString phones;
        for (const auto& phone : contact.getPhoneNumbers()) {
            if (!phones.isEmpty()) phones += "\n";
            phones += QString::fromStdString(phone.getType()) + ": " +
                     QString::fromStdString(phone.getNumber());
        }
        contactsTable->setItem(row, 6, new QTableWidgetItem(phones));
    }
    
    contactsTable->setSortingEnabled(true);  // Включаем сортировку обратно
}

void MainWindow::onAdd() {
    try {
        ContactDialog dialog(this);
        if (dialog.exec() == QDialog::Accepted) {
            Contact newContact = dialog.getContact();
            if (phoneBook->addContact(std::move(newContact))) {
                // Обновляем таблицу с учетом поиска
                onSearch();  // Вместо updateTable()
            } else {
                QMessageBox::critical(this, "Ошибка",
                    "Не удалось добавить контакт: " + phoneBook->getLastError());
            }
        }
    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Ошибка", 
            QString("Ошибка при добавлении контакта: %1").arg(e.what()));
    }
}

void MainWindow::onEdit() {
    int row = contactsTable->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Предупреждение", "Выберите контакт для редактирования");
        return;
    }
    
    // Получаем ID контакта из таблицы
    int contactId = contactsTable->item(row, 0)->data(Qt::UserRole).toInt();
    
    // Находим контакт по ID
    auto contacts = phoneBook->getContacts();
    auto it = std::find_if(contacts.begin(), contacts.end(),
        [contactId](const Contact& c) { return c.getId() == contactId; });
    
    if (it == contacts.end()) return;
    
    try {
        ContactDialog dialog(*it, this);
        if (dialog.exec() == QDialog::Accepted) {
            Contact updatedContact = dialog.getContact();
            updatedContact.setId(contactId);
            if (phoneBook->updateContact(contactId, std::move(updatedContact))) {
                onSearch();
            } else {
                QMessageBox::critical(this, "Ошибка",
                    "Не удалось обновить контакт: " + phoneBook->getLastError());
            }
        }
    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Ошибка",
            QString("Ошибка при редактировании контакта: %1").arg(e.what()));
    }
}

void MainWindow::onDelete() {
    int row = contactsTable->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Предупреждение",
            "Выберите контакт для удаления");
        return;
    }
    
    auto contacts = phoneBook->getContacts();
    if (row >= static_cast<int>(contacts.size())) {
        return;
    }
    
    if (QMessageBox::question(this, "Подтверждение",
        "Вы действительно хотите удалить этот контакт?",
        QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
        
        if (phoneBook->deleteContact(contacts[row].getId())) {
            // Обновляем таблицу с учетом поиска
            onSearch();  // Вместо updateTable()
        } else {
            QMessageBox::critical(this, "Ошибка",
                "Не удалось удалить контакт: " + phoneBook->getLastError());
        }
    }
}

void MainWindow::onSearch() {
    QString query = searchEdit->text().toLower();
    auto contacts = phoneBook->getContacts();
    
    contactsTable->setSortingEnabled(false);  // Отключаем сортировку на время обновления
    contactsTable->setRowCount(0);
    
    for (const auto& contact : contacts) {
        bool matches = query.isEmpty() ||
            QString::fromStdString(contact.getLastName()).toLower().contains(query) ||
            QString::fromStdString(contact.getFirstName()).toLower().contains(query) ||
            QString::fromStdString(contact.getMiddleName()).toLower().contains(query) ||
            QString::fromStdString(contact.getBirthDate()).toLower().contains(query) ||
            QString::fromStdString(contact.getAddress()).toLower().contains(query) ||
            QString::fromStdString(contact.getEmail()).toLower().contains(query);
        
        for (const auto& phone : contact.getPhoneNumbers()) {
            if (QString::fromStdString(phone.getNumber()).toLower().contains(query)) {
                matches = true;
                break;
            }
        }
        
        if (matches) {
            int row = contactsTable->rowCount();
            contactsTable->insertRow(row);
            
            auto* lastNameItem = new QTableWidgetItem(QString::fromStdString(contact.getLastName()));
            lastNameItem->setData(Qt::UserRole, contact.getId());
            contactsTable->setItem(row, 0, lastNameItem);
            
            contactsTable->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(contact.getFirstName())));
            contactsTable->setItem(row, 2, new QTableWidgetItem(QString::fromStdString(contact.getMiddleName())));
            contactsTable->setItem(row, 3, new QTableWidgetItem(QString::fromStdString(contact.getBirthDate())));
            contactsTable->setItem(row, 4, new QTableWidgetItem(QString::fromStdString(contact.getAddress())));
            contactsTable->setItem(row, 5, new QTableWidgetItem(QString::fromStdString(contact.getEmail())));
            
            QString phones;
            for (const auto& phone : contact.getPhoneNumbers()) {
                if (!phones.isEmpty()) phones += "\n";
                phones += QString::fromStdString(phone.getType()) + ": " +
                         QString::fromStdString(phone.getNumber());
            }
            contactsTable->setItem(row, 6, new QTableWidgetItem(phones));
        }
    }
    
    contactsTable->setSortingEnabled(true);  // Включаем сортировку обратно
}
