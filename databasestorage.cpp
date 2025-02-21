#include "databasestorage.h"

DatabaseStorage::DatabaseStorage(const QString& dbPath)
    : db(std::make_unique<DatabaseManager>(dbPath))
{
}

bool DatabaseStorage::addContact(const Contact& contact) {
    return db->addContact(contact);
}

bool DatabaseStorage::updateContact(int id, const Contact& contact) {
    return db->updateContact(id, contact);
}

bool DatabaseStorage::deleteContact(int id) {
    return db->deleteContact(id);
}

std::vector<Contact> DatabaseStorage::getAllContacts() const {
    return db->getAllContacts();
}

std::vector<Contact> DatabaseStorage::findContacts(const QString& query) const {
    return db->findContacts(query);
}

QString DatabaseStorage::getLastError() const {
    return db->getLastError();
} 