#include "filestorage.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

FileStorage::FileStorage(const QString& filePath) 
    : m_filePath(filePath), m_nextId(1)
{
    // Загружаем существующие контакты для определения следующего ID
    std::vector<Contact> contacts;
    if (loadContacts(contacts)) {
        for (const auto& contact : contacts) {
            if (contact.getId() >= m_nextId) {
                m_nextId = contact.getId() + 1;
            }
        }
    }
}

bool FileStorage::addContact(const Contact& contact) {
    std::vector<Contact> contacts;
    if (!loadContacts(contacts)) {
        return false;
    }
    
    Contact newContact = contact;
    newContact.setId(m_nextId++);
    contacts.push_back(newContact);
    
    return saveContacts(contacts);
}

bool FileStorage::updateContact(int id, const Contact& contact) {
    std::vector<Contact> contacts;
    if (!loadContacts(contacts)) {
        return false;
    }
    
    auto it = std::find_if(contacts.begin(), contacts.end(),
        [id](const Contact& c) { return c.getId() == id; });
    
    if (it == contacts.end()) {
        m_lastError = "Контакт не найден";
        return false;
    }
    
    *it = contact;
    it->setId(id);
    
    return saveContacts(contacts);
}

bool FileStorage::deleteContact(int id) {
    std::vector<Contact> contacts;
    if (!loadContacts(contacts)) {
        return false;
    }
    
    auto it = std::find_if(contacts.begin(), contacts.end(),
        [id](const Contact& c) { return c.getId() == id; });
    
    if (it == contacts.end()) {
        m_lastError = "Контакт не найден";
        return false;
    }
    
    contacts.erase(it);
    return saveContacts(contacts);
}

std::vector<Contact> FileStorage::getAllContacts() const {
    std::vector<Contact> contacts;
    loadContacts(contacts);
    return contacts;
}

std::vector<Contact> FileStorage::findContacts(const QString& query) const {
    std::vector<Contact> allContacts;
    loadContacts(allContacts);
    
    if (query.isEmpty()) {
        return allContacts;
    }
    
    std::vector<Contact> result;
    QString loweredQuery = query.toLower();
    
    for (const auto& contact : allContacts) {
        if (QString::fromStdString(contact.getLastName()).toLower().contains(loweredQuery) ||
            QString::fromStdString(contact.getFirstName()).toLower().contains(loweredQuery) ||
            QString::fromStdString(contact.getMiddleName()).toLower().contains(loweredQuery) ||
            QString::fromStdString(contact.getEmail()).toLower().contains(loweredQuery)) {
            result.push_back(contact);
        }
    }
    
    return result;
}

QString FileStorage::getLastError() const {
    return m_lastError;
}

bool FileStorage::saveContacts(const std::vector<Contact>& contacts) {
    QJsonArray jsonArray;
    
    for (const auto& contact : contacts) {
        jsonArray.append(contactToJson(contact));
    }
    
    QJsonDocument document(jsonArray);
    QFile file(m_filePath);
    
    if (!file.open(QIODevice::WriteOnly)) {
        m_lastError = "Не удалось открыть файл для записи";
        return false;
    }
    
    file.write(document.toJson());
    return true;
}

bool FileStorage::loadContacts(std::vector<Contact>& contacts) const {
    QFile file(m_filePath);
    
    if (!file.exists()) {
        return true; // Файл еще не создан - это нормально
    }
    
    if (!file.open(QIODevice::ReadOnly)) {
        const_cast<FileStorage*>(this)->m_lastError = "Не удалось открыть файл для чтения";
        return false;
    }
    
    QJsonDocument document = QJsonDocument::fromJson(file.readAll());
    if (!document.isArray()) {
        const_cast<FileStorage*>(this)->m_lastError = "Некорректный формат файла";
        return false;
    }
    
    contacts.clear();
    QJsonArray jsonArray = document.array();
    
    for (const auto& value : jsonArray) {
        if (!value.isObject()) continue;
        contacts.push_back(jsonToContact(value.toObject()));
    }
    
    return true;
}

QJsonObject FileStorage::contactToJson(const Contact& contact) const {
    QJsonObject json;
    json["id"] = contact.getId();
    json["lastName"] = QString::fromStdString(contact.getLastName());
    json["firstName"] = QString::fromStdString(contact.getFirstName());
    json["middleName"] = QString::fromStdString(contact.getMiddleName());
    json["birthDate"] = QString::fromStdString(contact.getBirthDate());
    json["address"] = QString::fromStdString(contact.getAddress());
    json["email"] = QString::fromStdString(contact.getEmail());
    
    QJsonArray phonesArray;
    for (const auto& phone : contact.getPhoneNumbers()) {
        QJsonObject phoneJson;
        phoneJson["number"] = QString::fromStdString(phone.getNumber());
        phoneJson["type"] = QString::fromStdString(phone.getType());
        phonesArray.append(phoneJson);
    }
    json["phones"] = phonesArray;
    
    return json;
}

Contact FileStorage::jsonToContact(const QJsonObject& json) const {
    Contact contact;
    contact.setId(json["id"].toInt());
    contact.setLastName(json["lastName"].toString().toStdString());
    contact.setFirstName(json["firstName"].toString().toStdString());
    contact.setMiddleName(json["middleName"].toString().toStdString());
    contact.setBirthDate(json["birthDate"].toString().toStdString());
    contact.setAddress(json["address"].toString().toStdString());
    contact.setEmail(json["email"].toString().toStdString());
    
    QJsonArray phonesArray = json["phones"].toArray();
    for (const auto& value : phonesArray) {
        QJsonObject phoneJson = value.toObject();
        PhoneNumber phone(
            phoneJson["number"].toString().toStdString(),
            phoneJson["type"].toString().toStdString()
        );
        contact.addPhoneNumber(phone);
    }
    
    return contact;
} 