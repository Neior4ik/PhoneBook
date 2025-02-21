#pragma once
#include "istorage.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

class FileStorage : public IStorage {
public:
    explicit FileStorage(const QString& filePath);
    
    bool addContact(const Contact& contact) override;
    bool updateContact(int id, const Contact& contact) override;
    bool deleteContact(int id) override;
    std::vector<Contact> getAllContacts() const override;
    std::vector<Contact> findContacts(const QString& query) const override;
    QString getLastError() const override;

private:
    QString m_filePath;
    QString m_lastError;
    int m_nextId;
    
    bool saveContacts(const std::vector<Contact>& contacts);
    bool loadContacts(std::vector<Contact>& contacts) const;
    QJsonObject contactToJson(const Contact& contact) const;
    Contact jsonToContact(const QJsonObject& json) const;
}; 