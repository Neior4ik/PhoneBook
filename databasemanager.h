#pragma once
#include <QObject>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QDebug>
#include <QString>
#include <QStringList>
#include <QVariant>
#include <QCoreApplication>
#include <QDir>
#include <vector>
#include <QMap>
#include "contact.h"
#include "phonenumber.h"

class DatabaseManager : public QObject
{
    Q_OBJECT

public:
    explicit DatabaseManager(const QString& path, QObject *parent = nullptr);
    ~DatabaseManager();

    bool init();
    bool open();
    void close();
    bool isOpen() const;
    
    bool beginTransaction();
    bool commitTransaction();
    bool rollbackTransaction();
    bool initialize();
    
    bool addContact(const Contact& contact);
    bool updateContact(int id, const Contact& contact);
    bool deleteContact(int id);
    std::vector<Contact> getAllContacts() const;
    std::vector<Contact> findContacts(const QString& pattern) const;
    bool clearAllContacts();
    bool testConnection();
    QString getLastError() const { return m_lastError; }

private:
    QString dbPath;
    QSqlDatabase db;
    QString m_lastError;
    
    bool executeQuery(QSqlQuery& query, const QString& queryStr);
    bool executePreparedQuery(QSqlQuery& query, const QString& queryStr, 
                            const QMap<QString, QVariant>& params);
    bool createTables();
    bool createIndexes();
    Contact contactFromQuery(const QSqlQuery& query) const;
    bool loadPhoneNumbers(Contact& contact) const;
    bool getPhoneNumbers(int contactId, std::vector<PhoneNumber>& phones);
    std::vector<Contact> searchByPhone(const std::string& phoneNumber);
};