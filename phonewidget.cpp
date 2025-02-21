#include "phonewidget.h"
#include <QHBoxLayout>
#include <QPushButton>

PhoneWidget::PhoneWidget(QWidget* parent) : QWidget(parent) {
    auto layout = new QHBoxLayout(this);
    layout->setMargin(0);

    numberEdit = new QLineEdit(this);
    numberEdit->setPlaceholderText("Номер телефона");
    
    typeCombo = new QComboBox(this);
    typeCombo->addItems({"Мобильный", "Домашний", "Рабочий"});
    
    auto removeButton = new QPushButton("Удалить", this);
    
    layout->addWidget(numberEdit);
    layout->addWidget(typeCombo);
    layout->addWidget(removeButton);
    
    connect(removeButton, &QPushButton::clicked, 
            this, &PhoneWidget::removeClicked);
}

PhoneNumber PhoneWidget::getPhoneNumber() const {
    QString number = numberEdit->text().trimmed();
    
    // Форматируем номер телефона
    number.remove(QRegExp("[\\s-()]"));
    if (number.startsWith("8")) {
        number.replace(0, 1, "+7");
    }
    
    return PhoneNumber(
        number.toStdString(),
        typeCombo->currentText().toStdString()
    );
}

void PhoneWidget::setPhoneNumber(const PhoneNumber& number) {
    numberEdit->setText(QString::fromStdString(number.getNumber()));
    typeCombo->setCurrentText(QString::fromStdString(number.getType()));
}

void PhoneWidget::clear() {
    numberEdit->clear();
    typeCombo->setCurrentIndex(0);
} 