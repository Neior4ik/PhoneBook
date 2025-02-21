#pragma once
#include <vector>
#include "contact.h"
#include <QString>

class IStorage {
public:
    virtual ~IStorage() = default;
    
    virtual bool addContact(const Contact& contact) = 0;
    virtual bool updateContact(int id, const Contact& contact) = 0;
    virtual bool deleteContact(int id) = 0;
    virtual std::vector<Contact> getAllContacts() const = 0;
    virtual std::vector<Contact> findContacts(const QString& query) const = 0;
    virtual QString getLastError() const = 0;
}; 