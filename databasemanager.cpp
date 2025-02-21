#include "databasemanager.h"
#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>
#include <QDebug>
#include <QFile>
#include <memory>

DatabaseManager::DatabaseManager(const QString& path, QObject *parent)
    : QObject(parent)
    , dbPath(path)
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(dbPath);
    
    if (!initialize()) {
        qDebug() << "Failed to initialize database:" << getLastError();
    }
}

DatabaseManager::~DatabaseManager()
{
    if (db.isOpen()) {
        db.close();
    }
}

bool DatabaseManager::init()
{
    qDebug() << "Initializing database at:" << dbPath;  // Для отладки
    
    if (!open()) {
        qDebug() << "Failed to open database:" << m_lastError;  // Для отладки
        return false;
    }

    if (!createTables()) {
        qDebug() << "Failed to create tables:" << m_lastError;  // Для отладки
        close();
        return false;
    }

    if (!createIndexes()) {
        close();
        return false;
    }

    return true;
}

bool DatabaseManager::open()
{
    if (!db.open()) {
        m_lastError = db.lastError().text();
        return false;
    }
    return true;
}

void DatabaseManager::close()
{
    db.close();
}

bool DatabaseManager::isOpen() const
{
    return db.isOpen();
}

bool DatabaseManager::beginTransaction()
{
    return db.transaction();
}

bool DatabaseManager::commitTransaction()
{
    return db.commit();
}

bool DatabaseManager::rollbackTransaction()
{
    return db.rollback();
}

bool DatabaseManager::initialize()
{
    return init();
}

bool DatabaseManager::addContact(const Contact& contact)
{
    if (!isOpen() && !open()) {
        return false;
    }

    beginTransaction();

    QSqlQuery query(db);
    query.prepare("INSERT INTO contacts (first_name, last_name, middle_name, birth_date, address, email) "
                 "VALUES (:first_name, :last_name, :middle_name, :birth_date, :address, :email)");
    
    query.bindValue(":first_name", QString::fromStdString(contact.getFirstName()));
    query.bindValue(":last_name", QString::fromStdString(contact.getLastName()));
    query.bindValue(":middle_name", QString::fromStdString(contact.getMiddleName()));
    query.bindValue(":birth_date", QString::fromStdString(contact.getBirthDate()));
    query.bindValue(":address", QString::fromStdString(contact.getAddress()));
    query.bindValue(":email", QString::fromStdString(contact.getEmail()));

    if (!query.exec()) {
        m_lastError = query.lastError().text();
        rollbackTransaction();
        return false;
    }

    int contactId = query.lastInsertId().toInt();

    // Добавляем телефоны
    for (const auto& phone : contact.getPhoneNumbers()) {
        query.prepare("INSERT INTO phone_numbers (contact_id, number, type) "
                     "VALUES (:contact_id, :number, :type)");
        
        query.bindValue(":contact_id", contactId);
        query.bindValue(":number", QString::fromStdString(phone.getNumber()));
        query.bindValue(":type", QString::fromStdString(phone.getType()));

        if (!query.exec()) {
            m_lastError = query.lastError().text();
            rollbackTransaction();
            return false;
        }
    }

    return commitTransaction();
}

std::vector<Contact> DatabaseManager::getAllContacts() const
{
    std::vector<Contact> contacts;
    
    QSqlQuery query(db);
    query.prepare(
        "SELECT c.id, c.last_name, c.first_name, c.middle_name, "
        "c.birth_date, c.address, c.email, "
        "p.number, p.type "
        "FROM contacts c "
        "LEFT JOIN phone_numbers p ON c.id = p.contact_id "
        "ORDER BY c.id"
    );
    
    if (!query.exec()) {
        qDebug() << "Failed to get contacts:" << query.lastError().text();
        return contacts;
    }
    
    int currentId = -1;
    std::unique_ptr<Contact> currentContact;
    
    while (query.next()) {
        int id = query.value("id").toInt();
        
        // Если это новый контакт
        if (id != currentId) {
            // Если был предыдущий контакт, добавляем его в вектор
            if (currentContact) {
                contacts.push_back(*currentContact);
            }
            
            currentContact = std::make_unique<Contact>();
            currentId = id;
            
            currentContact->setId(id);
            currentContact->setLastName(query.value("last_name").toString().toStdString());
            currentContact->setFirstName(query.value("first_name").toString().toStdString());
            currentContact->setMiddleName(query.value("middle_name").toString().toStdString());
            currentContact->setBirthDate(query.value("birth_date").toString().toStdString());
            currentContact->setAddress(query.value("address").toString().toStdString());
            currentContact->setEmail(query.value("email").toString().toStdString());
        }
        
        // Добавляем телефон, если он есть
        if (!query.value("number").isNull()) {
            PhoneNumber phone(
                query.value("number").toString().toStdString(),
                query.value("type").toString().toStdString()
            );
            currentContact->addPhoneNumber(phone);
        }
    }
    
    // Добавляем последний контакт
    if (currentContact) {
        contacts.push_back(*currentContact);
    }
    
    return contacts;
}

bool DatabaseManager::updateContact(int id, const Contact& contact)
{
    if (!isOpen() && !open()) {
        return false;
    }

    beginTransaction();

    QSqlQuery query(db);
    query.prepare("UPDATE contacts SET first_name=:first_name, last_name=:last_name, "
                 "middle_name=:middle_name, birth_date=:birth_date, address=:address, "
                 "email=:email WHERE id=:id");
    
    query.bindValue(":id", id);
    query.bindValue(":first_name", QString::fromStdString(contact.getFirstName()));
    query.bindValue(":last_name", QString::fromStdString(contact.getLastName()));
    query.bindValue(":middle_name", QString::fromStdString(contact.getMiddleName()));
    query.bindValue(":birth_date", QString::fromStdString(contact.getBirthDate()));
    query.bindValue(":address", QString::fromStdString(contact.getAddress()));
    query.bindValue(":email", QString::fromStdString(contact.getEmail()));

    if (!query.exec()) {
        m_lastError = query.lastError().text();
        rollbackTransaction();
        return false;
    }

    // Удаляем старые телефоны
    query.prepare("DELETE FROM phone_numbers WHERE contact_id=:id");
    query.bindValue(":id", id);
    
    if (!query.exec()) {
        m_lastError = query.lastError().text();
        rollbackTransaction();
        return false;
    }

    // Добавляем новые телефоны
    for (const auto& phone : contact.getPhoneNumbers()) {
        query.prepare("INSERT INTO phone_numbers (contact_id, number, type) "
                     "VALUES (:contact_id, :number, :type)");
        
        query.bindValue(":contact_id", id);
        query.bindValue(":number", QString::fromStdString(phone.getNumber()));
        query.bindValue(":type", QString::fromStdString(phone.getType()));

        if (!query.exec()) {
            m_lastError = query.lastError().text();
            rollbackTransaction();
            return false;
        }
    }

    return commitTransaction();
}

bool DatabaseManager::deleteContact(int id)
{
    if (!isOpen() && !open()) {
        return false;
    }

    beginTransaction();

    QSqlQuery query(db);
    
    // Сначала удаляем телефоны
    query.prepare("DELETE FROM phone_numbers WHERE contact_id=:id");
    query.bindValue(":id", id);
    
    if (!query.exec()) {
        m_lastError = query.lastError().text();
        rollbackTransaction();
        return false;
    }

    // Затем удаляем контакт
    query.prepare("DELETE FROM contacts WHERE id=:id");
    query.bindValue(":id", id);
    
    if (!query.exec()) {
        m_lastError = query.lastError().text();
        rollbackTransaction();
        return false;
    }

    return commitTransaction();
}

std::vector<Contact> DatabaseManager::findContacts(const QString& pattern) const
{
    std::vector<Contact> results;
    
    if (!db.isOpen()) {
        return results;
    }

    QSqlQuery query(db);
    query.prepare("SELECT DISTINCT c.id, c.first_name, c.last_name, c.email "
                 "FROM contacts c "
                 "LEFT JOIN phone_numbers p ON c.id = p.contact_id "
                 "WHERE c.first_name LIKE :pattern "
                 "OR c.last_name LIKE :pattern "
                 "OR c.email LIKE :pattern "
                 "OR p.number LIKE :pattern");
    
    query.bindValue(":pattern", "%" + pattern + "%");

    if (!query.exec()) {
        qDebug() << "Search failed:" << query.lastError().text();
        return results;
    }

    while (query.next()) {
        Contact contact = contactFromQuery(query);
        if (!loadPhoneNumbers(contact)) {
            qDebug() << "Failed to load phone numbers for contact" << contact.getId();
            continue;
        }
        results.push_back(contact);
    }

    return results;
}

Contact DatabaseManager::contactFromQuery(const QSqlQuery& query) const
{
    Contact contact;
    contact.setId(query.value("id").toInt());
    contact.setFirstName(query.value("first_name").toString().toStdString());
    contact.setLastName(query.value("last_name").toString().toStdString());
    contact.setMiddleName(query.value("middle_name").toString().toStdString());
    contact.setBirthDate(query.value("birth_date").toString().toStdString());
    contact.setAddress(query.value("address").toString().toStdString());
    contact.setEmail(query.value("email").toString().toStdString());
    return contact;
}

bool DatabaseManager::loadPhoneNumbers(Contact& contact) const
{
    QSqlQuery query(db);
    query.prepare("SELECT number, type FROM phone_numbers WHERE contact_id=:id");
    query.bindValue(":id", contact.getId());

    if (!query.exec()) {
        qDebug() << "Failed to load phone numbers:" << query.lastError().text();
        return false;
    }

    while (query.next()) {
        PhoneNumber phone(
            query.value("number").toString().toStdString(),
            query.value("type").toString().toStdString()
        );
        contact.addPhoneNumber(phone);
    }

    return true;
}

bool DatabaseManager::createTables()
{
    QStringList queries = {
        "CREATE TABLE IF NOT EXISTS contacts ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "first_name TEXT NOT NULL,"
        "last_name TEXT NOT NULL,"
        "middle_name TEXT,"
        "birth_date TEXT,"
        "address TEXT,"
        "email TEXT"
        ")",

        "CREATE TABLE IF NOT EXISTS phone_numbers ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "contact_id INTEGER NOT NULL,"
        "number TEXT NOT NULL,"
        "type TEXT NOT NULL,"
        "FOREIGN KEY(contact_id) REFERENCES contacts(id)"
        ")"
    };

    for (const QString& queryStr : queries) {
        QSqlQuery query(db);
        if (!query.exec(queryStr)) {
            m_lastError = query.lastError().text();
            return false;
        }
    }

    return true;
}

bool DatabaseManager::createIndexes()
{
    QStringList queries = {
        "CREATE INDEX IF NOT EXISTS idx_contacts_name ON contacts(first_name, last_name)",
        "CREATE INDEX IF NOT EXISTS idx_phone_numbers_contact ON phone_numbers(contact_id)"
    };

    for (const QString& queryStr : queries) {
        QSqlQuery query(db);
        if (!query.exec(queryStr)) {
            m_lastError = query.lastError().text();
            return false;
        }
    }

    return true;
}

bool DatabaseManager::clearAllContacts()
{
    if (!isOpen() && !open()) {
        return false;
    }

    beginTransaction();

    QSqlQuery query(db);
    if (!query.exec("DELETE FROM phone_numbers") ||
        !query.exec("DELETE FROM contacts")) {
        m_lastError = query.lastError().text();
        rollbackTransaction();
        return false;
    }

    return commitTransaction();
}

bool DatabaseManager::testConnection()
{
    return isOpen() || open();
}
