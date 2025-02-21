#include "contactdialog.h"
#include <QMessageBox>
#include <QRegularExpression>
#include <QFormLayout>
#include <QLabel>

// Реализация PhoneWidget
// PhoneWidget::PhoneWidget(QWidget* parent) : QWidget(parent) { ... }

// Реализация ContactDialog
ContactDialog::ContactDialog(QWidget* parent)
    : QDialog(parent)
{
    setupUI();
    setupConnections();
}

ContactDialog::ContactDialog(const Contact& initialContact, QWidget* parent)
    : QDialog(parent), contact(initialContact)
{
    setupUI();
    setupConnections();

    // Заполняем поля данными контакта
    firstNameEdit->setText(QString::fromStdString(contact.getFirstName()));
    lastNameEdit->setText(QString::fromStdString(contact.getLastName()));
    middleNameEdit->setText(QString::fromStdString(contact.getMiddleName()));
    emailEdit->setText(QString::fromStdString(contact.getEmail()));
    addressEdit->setText(QString::fromStdString(contact.getAddress()));
    
    if (!contact.getBirthDate().empty()) {
        birthDateEdit->setDate(QDate::fromString(
            QString::fromStdString(contact.getBirthDate()), 
            "yyyy-MM-dd"
        ));
    }

    // Добавляем телефоны
    for (const auto& phone : contact.getPhoneNumbers()) {
        auto widget = new PhoneWidget(this);
        widget->setPhoneNumber(phone);
        connect(widget, &PhoneWidget::removeClicked, this, &ContactDialog::onRemovePhone);
        phoneWidgets.append(widget);
        dynamic_cast<QBoxLayout*>(layout())->insertWidget(
            layout()->indexOf(addPhoneButton),
            widget
        );
    }
}

void ContactDialog::setupUI() {
    auto mainLayout = new QVBoxLayout(this);
    
    // Основные поля
    auto formLayout = new QFormLayout;
    lastNameEdit = new QLineEdit(this);
    firstNameEdit = new QLineEdit(this);
    middleNameEdit = new QLineEdit(this);
    birthDateEdit = new QDateEdit(this);
    emailEdit = new QLineEdit(this);
    addressEdit = new QLineEdit(this);
    
    birthDateEdit->setCalendarPopup(true);
    birthDateEdit->setDisplayFormat("yyyy-MM-dd");
    birthDateEdit->setDate(QDate::currentDate());
    
    formLayout->addRow("Фамилия*:", lastNameEdit);
    formLayout->addRow("Имя*:", firstNameEdit);
    formLayout->addRow("Отчество:", middleNameEdit);
    formLayout->addRow("Дата рождения:", birthDateEdit);
    formLayout->addRow("Email:", emailEdit);
    formLayout->addRow("Адрес:", addressEdit);
    
    mainLayout->addLayout(formLayout);
    
    // Телефоны
    auto phoneLabel = new QLabel("Телефоны:", this);
    mainLayout->addWidget(phoneLabel);
    
    addPhoneButton = new QPushButton("Добавить телефон", this);
    mainLayout->addWidget(addPhoneButton);
    
    // Кнопки OK и Cancel
    auto buttonLayout = new QHBoxLayout;
    okButton = new QPushButton("OK", this);
    cancelButton = new QPushButton("Отмена", this);
    
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);
    mainLayout->addLayout(buttonLayout);
    
    setLayout(mainLayout);
    setWindowTitle("Контакт");
    setModal(true);
}

void ContactDialog::setupConnections() {
    connect(addPhoneButton, &QPushButton::clicked, this, &ContactDialog::onAddPhone);
    connect(okButton, &QPushButton::clicked, this, &ContactDialog::onAccept);
    connect(cancelButton, &QPushButton::clicked, this, &ContactDialog::reject);
    
    // Подключаем сигнал удаления для каждого виджета телефона
    for (auto* widget : phoneWidgets) {
        connect(widget, &PhoneWidget::removeClicked, this, &ContactDialog::onRemovePhone);
    }
}

void ContactDialog::addPhoneWidget() {
    auto widget = new PhoneWidget(this);
    phoneWidgets.append(widget);
    
    // Подключаем сигнал удаления для нового виджета
    connect(widget, &PhoneWidget::removeClicked, this, &ContactDialog::onRemovePhone);
    
    dynamic_cast<QBoxLayout*>(layout())->insertWidget(
        layout()->indexOf(addPhoneButton),
        widget
    );
}

void ContactDialog::onAddPhone() {
    addPhoneWidget();
}

bool ContactDialog::validateInputs() const {
    QString errorMsg;
    
    // Проверяем ФИО
    if (!Contact::validateName(lastNameEdit->text().trimmed().toStdString())) {
        errorMsg = "Некорректная фамилия";
    }
    else if (!Contact::validateName(firstNameEdit->text().trimmed().toStdString())) {
        errorMsg = "Некорректное имя";
    }
    else if (!middleNameEdit->text().trimmed().isEmpty() && 
             !Contact::validateName(middleNameEdit->text().trimmed().toStdString())) {
        errorMsg = "Некорректное отчество";
    }
    
    // Проверяем email
    if (errorMsg.isEmpty() && !emailEdit->text().trimmed().isEmpty() && 
        !Contact::validateEmail(emailEdit->text().trimmed().toStdString())) {
        errorMsg = "Некорректный email";
    }
    
    // Проверяем дату рождения
    QDate birthDate = birthDateEdit->date();
    if (errorMsg.isEmpty() && birthDate > QDate::currentDate()) {
        errorMsg = "Дата рождения не может быть больше текущей даты";
    }
    
    // Проверяем телефоны
    for (const auto* widget : phoneWidgets) {
        if (!PhoneNumber::isValidNumber(widget->getPhoneNumber().getNumber())) {
            errorMsg = "Некорректный формат номера телефона";
            break;
        }
    }
    
    if (!errorMsg.isEmpty()) {
        QMessageBox::warning(nullptr, "Ошибка валидации", errorMsg);
        return false;
    }
    
    return true;
}

void ContactDialog::onAccept() {
    try {
        getContact(); // Проверяем валидность данных
        accept();
    }
    catch (const std::exception&) {
        // Ошибка уже показана пользователю в getContact()
    }
}

void ContactDialog::onRemovePhone() {
    auto widget = qobject_cast<PhoneWidget*>(sender());
    if (widget) {
        phoneWidgets.removeOne(widget);
        widget->deleteLater();
    }
}

Contact ContactDialog::getContact() const {
    Contact contact;
    
    if (!validateInputs()) {
        throw std::runtime_error("Некорректные данные");
    }
    
    // Устанавливаем значения и проверяем успешность
    if (!contact.setLastName(lastNameEdit->text().trimmed().toStdString()) ||
        !contact.setFirstName(firstNameEdit->text().trimmed().toStdString()) ||
        !contact.setMiddleName(middleNameEdit->text().trimmed().toStdString()) ||
        !contact.setEmail(emailEdit->text().trimmed().toStdString()) ||
        !contact.setBirthDate(birthDateEdit->date().toString("yyyy-MM-dd").toStdString())) {
        throw std::runtime_error("Ошибка при установке данных контакта");
    }
    
    // Адрес не требует валидации
    contact.setAddress(addressEdit->text().trimmed().toStdString());
    
    // Добавляем телефоны
    for (const auto* widget : phoneWidgets) {
        contact.addPhoneNumber(widget->getPhoneNumber());
    }
    
    return contact;
}

void ContactDialog::setContact(const Contact& newContact) {
    contact = newContact;
    firstNameEdit->setText(QString::fromStdString(contact.getFirstName()));
    lastNameEdit->setText(QString::fromStdString(contact.getLastName()));
    middleNameEdit->setText(QString::fromStdString(contact.getMiddleName()));
    emailEdit->setText(QString::fromStdString(contact.getEmail()));
    addressEdit->setText(QString::fromStdString(contact.getAddress()));
    
    // Очищаем существующие телефоны
    clearPhoneWidgets();
    
    // Добавляем новые телефоны
    for (const auto& phone : contact.getPhoneNumbers()) {
        auto widget = new PhoneWidget(this);
        widget->setPhoneNumber(phone);
        phoneWidgets.append(widget);
        dynamic_cast<QBoxLayout*>(layout())->insertWidget(
            layout()->indexOf(addPhoneButton),
            widget
        );
    }
}

ContactDialog::~ContactDialog() {
    clearPhoneWidgets();
}

void ContactDialog::clearPhoneWidgets() {
    qDeleteAll(phoneWidgets);
    phoneWidgets.clear();
}
