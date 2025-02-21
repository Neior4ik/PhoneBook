#include "phonenumber.h"
#include <QRegExp>

PhoneNumber::PhoneNumber(const std::string& num, const std::string& t)
    : number(num), type(t) {}

bool PhoneNumber::isValid() const {
    return validatePhoneNumber(number);
}

bool PhoneNumber::validatePhoneNumber(const std::string& phone) {
    if (phone.empty()) return false;
    
    static const QRegExp phoneRegex(
        "^(\\+7|8)\\s*\\(?(\\d{3})\\)?[-\\s]?(\\d{3})[-\\s]?(\\d{2})[-\\s]?(\\d{2})$"
    );
    
    QString qPhone = QString::fromStdString(phone).trimmed();
    return phoneRegex.exactMatch(qPhone);
}

std::string PhoneNumber::getNumber() const {
    return number;
}

std::string PhoneNumber::getType() const {
    return type;
}

void PhoneNumber::setNumber(const std::string& newNumber) {
    // Форматируем номер телефона в единый формат
    QString formatted = QString::fromStdString(newNumber)
        .remove(QRegExp("[\\s-]"))  // Удаляем пробелы и дефисы
        .replace(QRegExp("^8"), "+7"); // Заменяем 8 на +7
    
    number = formatted.toStdString();
}

void PhoneNumber::setType(const std::string& newType) {
    type = newType;
}

std::string PhoneNumber::getFormattedNumber() const {
    std::string formatted = number;
    // Удаляем все существующие дефисы
    formatted.erase(std::remove(formatted.begin(), formatted.end(), '-'), formatted.end());
    
    // Если номер начинается с 8, заменяем на +7
    if (formatted[0] == '8') {
        formatted[0] = '+';
        formatted.insert(1, "7");
    }
    
    // Добавляем скобки и дефисы
    if (formatted.length() == 11 || formatted.length() == 12) { // +7 или 8 в начале
        size_t startPos = (formatted[0] == '+') ? 2 : 1;
        formatted.insert(startPos, "(");
        formatted.insert(startPos + 4, ")");
        formatted.insert(startPos + 8, "-");
        formatted.insert(startPos + 11, "-");
    }
    
    return formatted;
} 