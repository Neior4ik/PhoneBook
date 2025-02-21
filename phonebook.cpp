#include "phonebook.h"
#include <algorithm>
#include <cctype>
#include <QDebug>

std::string PhoneBook::toLower(const std::string& str) const {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), 
                  [](unsigned char c){ return std::tolower(c); });
    return result;
}

PhoneBook::PhoneBook(std::unique_ptr<IStorage> storage_, QObject *parent)
    : QObject(parent)
    , storage(std::move(storage_))
{
}

bool PhoneBook::addContact(Contact&& contact) {
    if (!storage->addContact(contact)) {
        lastError = storage->getLastError();
        return false;
    }
    return true;
}

bool PhoneBook::updateContact(int id, const Contact& contact) {
    if (!storage->updateContact(id, contact)) {
        lastError = storage->getLastError();
        return false;
    }
    return true;
}

bool PhoneBook::deleteContact(int id) {
    if (!storage->deleteContact(id)) {
        lastError = storage->getLastError();
        return false;
    }
    return true;
}

std::vector<Contact> PhoneBook::getContacts() const {
    return storage->getAllContacts();
}

std::vector<Contact> PhoneBook::findContacts(const QString& query) const {
    return storage->findContacts(query);
}
