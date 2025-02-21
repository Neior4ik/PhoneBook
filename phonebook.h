#pragma once
#include <QObject>
#include <vector>
#include <memory>
#include "contact.h"
#include "istorage.h"

class PhoneBook : public QObject {
    Q_OBJECT

public:
    explicit PhoneBook(std::unique_ptr<IStorage> storage, QObject *parent = nullptr);
    ~PhoneBook() = default;
    
    bool addContact(Contact&& contact);
    bool updateContact(int id, const Contact& contact);
    bool deleteContact(int id);
    std::vector<Contact> getContacts() const;
    std::vector<Contact> findContacts(const QString& query) const;
    QString getLastError() const { return lastError; }

private:
    std::unique_ptr<IStorage> storage;
    QString lastError;
    std::string toLower(const std::string& str) const;
};
