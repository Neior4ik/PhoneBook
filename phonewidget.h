#pragma once
#include <QWidget>
#include <QLineEdit>
#include <QComboBox>
#include <QHBoxLayout>
#include "phonenumber.h"

class PhoneWidget : public QWidget {
    Q_OBJECT
public:
    explicit PhoneWidget(QWidget* parent = nullptr);
    PhoneNumber getPhoneNumber() const;
    void setPhoneNumber(const PhoneNumber& number);
    void clear();

signals:
    void removeClicked();

private:
    QLineEdit* numberEdit;
    QComboBox* typeCombo;
}; 