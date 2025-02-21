#pragma once
#include <string>
#include <QRegExp>
#include <QDebug>

class PhoneNumber {
private:
    std::string number;
    std::string type; // "home", "work", "mobile"
    
    inline static int copyCount = 0;
    inline static int moveCount = 0;

public:
    PhoneNumber(const std::string& num = "", const std::string& t = "мобильный");
    bool isValid() const;
    std::string getNumber() const;
    std::string getType() const;
    std::string getFormattedNumber() const;
    bool isValidType(const std::string& t) const;
    
    // Конструктор копирования
    PhoneNumber(const PhoneNumber& other) 
        : number(other.number)
        , type(other.type) 
    {
        ++copyCount;
        qDebug() << "PhoneNumber copy constructor. Total copies:" << copyCount;
    }
    
    // Оператор присваивания копированием
    PhoneNumber& operator=(const PhoneNumber& other) {
        if (this != &other) {
            number = other.number;
            type = other.type;
            ++copyCount;
            qDebug() << "PhoneNumber copy assignment. Total copies:" << copyCount;
        }
        return *this;
    }
    
    // Конструктор перемещения
    PhoneNumber(PhoneNumber&& other) noexcept
        : number(std::move(other.number))
        , type(std::move(other.type))
    {
        ++moveCount;
        qDebug() << "PhoneNumber move constructor. Total moves:" << moveCount;
    }
    
    // Оператор присваивания перемещением
    PhoneNumber& operator=(PhoneNumber&& other) noexcept {
        if (this != &other) {
            number = std::move(other.number);
            type = std::move(other.type);
            ++moveCount;
            qDebug() << "PhoneNumber move assignment. Total moves:" << moveCount;
        }
        return *this;
    }
    
    // Валидация
    static bool isValidNumber(const std::string& num) {
        QString qNum = QString::fromStdString(num).trimmed();
        
        // Поддерживаемые форматы:
        // +78121234567
        // 88121234567
        // +7(812)1234567
        // 8(812)1234567
        // +7(812)123-45-67
        // 8(812)123-45-67
        
        QRegExp phoneRegex("^(\\+7|8)(\\(\\d{3}\\)|\\d{3})(\\d{7}|\\d{3}-\\d{2}-\\d{2})$");
        return phoneRegex.exactMatch(qNum);
    }
    
    // Нормализация номера (приведение к единому формату)
    static std::string normalizeNumber(const std::string& num) {
        QString qNum = QString::fromStdString(num).trimmed();
        
        // Удаляем все не цифры, кроме +
        QString normalized;
        for (QChar c : qNum) {
            if (c.isDigit() || c == '+') {
                normalized += c;
            }
        }
        
        // Если номер начинается с 8, заменяем на +7
        if (normalized.startsWith('8')) {
            normalized.replace(0, 1, "+7");
        }
        
        return normalized.toStdString();
    }
    
    // Геттеры и сеттеры
    void setNumber(const std::string& newNumber);
    void setType(const std::string& newType);
    
    // Статистика
    static int getCopyCount() { return copyCount; }
    static int getMoveCount() { return moveCount; }
    static void resetCounters() {
        copyCount = 0;
        moveCount = 0;
    }
    
    static bool validatePhoneNumber(const std::string& phone);
}; 