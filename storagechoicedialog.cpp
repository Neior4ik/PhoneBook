#include "storagechoicedialog.h"
#include <QVBoxLayout>

StorageChoiceDialog::StorageChoiceDialog(QWidget* parent) 
    : QDialog(parent)
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowTitle("Выбор хранилища");
    
    auto layout = new QVBoxLayout(this);
    
    dbRadio = new QRadioButton("База данных SQLite", this);
    fileRadio = new QRadioButton("Файл JSON", this);
    dbRadio->setChecked(true);  // По умолчанию выбрана БД
    
    auto okButton = new QPushButton("OK", this);
    
    layout->addWidget(dbRadio);
    layout->addWidget(fileRadio);
    layout->addWidget(okButton);
    
    connect(okButton, &QPushButton::clicked, this, &QDialog::accept);
}

bool StorageChoiceDialog::useDatabase() const {
    return dbRadio->isChecked();
}