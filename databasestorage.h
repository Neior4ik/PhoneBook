#pragma once
#include "istorage.h"
#include "databasemanager.h"

class DatabaseStorage : public IStorage {
public:
    explicit DatabaseStorage(const QString& dbPath);
    
    bool addContact(const Contact& contact) override;
    bool updateContact(int id, const Contact& contact) override;
    bool deleteContact(int id) override;
    std::vector<Contact> getAllContacts() const override;
    std::vector<Contact> findContacts(const QString& query) const override;
    QString getLastError() const override;

private:
    std::unique_ptr<DatabaseManager> db;
}; 