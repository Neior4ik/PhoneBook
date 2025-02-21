#include "contact.h"
#include <regex>
#include <QDate>

bool Contact::validateName(const std::string& name) {
    if (name.empty()) return false;
    
    // Регулярное выражение для проверки имени:
    // ^ - начало строки
    // [А-ЯA-Z] - первая буква должна быть заглавной (русской или английской)
    // [а-яА-Яa-zA-Z0-9\s-]* - далее могут быть буквы, цифры, пробелы и дефис
    // [^-\s] - не должно заканчиваться на дефис или пробел
    // $ - конец строки
    static const QRegExp nameRegex("^[А-ЯA-Z][а-яА-Яa-zA-Z0-9\\s-]*[^-\\s]$");
    
    QString qName = QString::fromStdString(name.c_str()).trimmed();
    return nameRegex.exactMatch(qName);
}

bool Contact::validateEmail(const std::string& email) {
    if (email.empty()) return true; // Email может быть пустым
    
    // Регулярное выражение для email
    static const QRegExp emailRegex(
        "^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\\.[a-zA-Z]{2,}$"
    );
    
    QString qEmail = QString::fromStdString(email).trimmed();
    return emailRegex.exactMatch(qEmail);
}

bool Contact::validateDate(const std::string& date) {
    if (date.empty()) return true;
    
    QDate qdate = QDate::fromString(QString::fromStdString(date), "yyyy-MM-dd");
    return qdate.isValid() && qdate <= QDate::currentDate();
}

bool Contact::setFirstName(const std::string& name) {
    if (!validateName(name)) return false;
    firstName = name;
    return true;
}

bool Contact::setLastName(const std::string& name) {
    if (!validateName(name)) return false;
    lastName = name;
    return true;
}

bool Contact::setMiddleName(const std::string& name) {
    middleName = name;
    return true;
}

bool Contact::setBirthDate(const std::string& date) {
    birthDate = date;
    return true;
}

bool Contact::setEmail(const std::string& newEmail) {
    if (!validateEmail(newEmail)) return false;
    email = newEmail;
    return true;
}

void Contact::addPhoneNumber(const PhoneNumber& phone) {
    if (phone.isValid()) {
        phoneNumbers.push_back(phone);
    }
}

void Contact::setAddress(const std::string& newAddress) {
    address = newAddress;
}
