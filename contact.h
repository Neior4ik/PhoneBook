#pragma once
#include <string>
#include <vector>
#include <QRegExp>
#include <QDate>
#include <QDebug>
#include "phonenumber.h"

class Contact {
private:
    int id;
    std::string firstName;
    std::string lastName;
    std::string middleName;
    std::string birthDate;
    std::string email;
    std::string address;
    std::vector<PhoneNumber> phoneNumbers;
    
    // Счетчики для анализа операций
    inline static int constructorCount = 0;  // Конструктор по умолчанию
    inline static int copyCount = 0;         // Копирование
    inline static int moveCount = 0;         // Перемещение
    inline static int deleteCount = 0;       // Удаление
    inline static int assignCount = 0;       // Присваивание
    inline static int moveAssignCount = 0;   // Перемещающее присваивание

public:
    // Переносим методы валидации в public секцию
    static bool validateName(const std::string& name);
    static bool validateEmail(const std::string& email);
    static bool validateDate(const std::string& date);

    // Конструктор по умолчанию
    Contact() : id(-1) {
        ++constructorCount;
        qDebug() << "Default constructor called. Total:" << constructorCount;
    }
    
    // Конструктор копирования
    Contact(const Contact& other) 
        : id(other.id)
        , firstName(other.firstName)
        , lastName(other.lastName)
        , middleName(other.middleName)
        , birthDate(other.birthDate)
        , email(other.email)
        , address(other.address)
        , phoneNumbers(other.phoneNumbers)
    {
        ++copyCount;
        qDebug() << "Contact copy constructor. Total copies:" << copyCount;
    }
    
    // Конструктор перемещения
    Contact(Contact&& other) noexcept
        : id(other.id)
        , firstName(std::move(other.firstName))
        , lastName(std::move(other.lastName))
        , middleName(std::move(other.middleName))
        , birthDate(std::move(other.birthDate))
        , email(std::move(other.email))
        , address(std::move(other.address))
        , phoneNumbers(std::move(other.phoneNumbers))
    {
        other.id = -1;
        ++moveCount;
        qDebug() << "Contact move constructor. Total moves:" << moveCount;
    }
    
    // Оператор присваивания копированием
    Contact& operator=(const Contact& other) {
        if (this != &other) {
            id = other.id;
            firstName = other.firstName;
            lastName = other.lastName;
            middleName = other.middleName;
            birthDate = other.birthDate;
            email = other.email;
            address = other.address;
            phoneNumbers = other.phoneNumbers;
            ++assignCount;
            qDebug() << "Contact copy assignment. Total:" << assignCount;
        }
        return *this;
    }
    
    // Оператор присваивания перемещением
    Contact& operator=(Contact&& other) noexcept {
        if (this != &other) {
            id = other.id;
            firstName = std::move(other.firstName);
            lastName = std::move(other.lastName);
            middleName = std::move(other.middleName);
            birthDate = std::move(other.birthDate);
            email = std::move(other.email);
            address = std::move(other.address);
            phoneNumbers = std::move(other.phoneNumbers);
            other.id = -1;
            ++moveAssignCount;
            qDebug() << "Contact move assignment. Total:" << moveAssignCount;
        }
        return *this;
    }
    
    ~Contact() {
        ++deleteCount;
        qDebug() << "Destructor called. Total:" << deleteCount;
    }

    // Статический метод для получения статистики
    static QString getStats() {
        return QString("Contact statistics:\n"
                      "- Default constructions: %1\n"
                      "- Copy constructions: %2\n"
                      "- Move constructions: %3\n"
                      "- Copy assignments: %4\n"
                      "- Move assignments: %5\n"
                      "- Destructions: %6")
            .arg(constructorCount)
            .arg(copyCount)
            .arg(moveCount)
            .arg(assignCount)
            .arg(moveAssignCount)
            .arg(deleteCount);
    }

    // Переопределение new
    void* operator new(size_t size) {
        void* ptr = ::operator new(size);
        qDebug() << "New operator called";
        return ptr;
    }
    
    // Геттеры
    int getId() const { return id; }
    std::string getFirstName() const { return firstName; }
    std::string getLastName() const { return lastName; }
    std::string getMiddleName() const { return middleName; }
    std::string getBirthDate() const { return birthDate; }
    std::string getEmail() const { return email; }
    std::string getAddress() const { return address; }
    const std::vector<PhoneNumber>& getPhoneNumbers() const { return phoneNumbers; }

    // Сеттеры
    void setId(int newId) { id = newId; }
    bool setFirstName(const std::string& name);
    bool setLastName(const std::string& name);
    bool setMiddleName(const std::string& name);
    bool setBirthDate(const std::string& date);
    bool setEmail(const std::string& email);
    void setAddress(const std::string& addr);
    void addPhoneNumber(const PhoneNumber& phone);
    void clearPhoneNumbers() { phoneNumbers.clear(); }
    void setPhoneNumbers(const std::vector<PhoneNumber>& numbers) {
        phoneNumbers = numbers;
    }
};
