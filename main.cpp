#include <QApplication>
#include <QMessageBox>
#include "mainwindow.h"
#include "databasestorage.h"
#include "filestorage.h"
#include "storagechoicedialog.h"
#include <QDir>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    
    // Показываем диалог выбора хранилища
    StorageChoiceDialog dialog;
    if (dialog.exec() != QDialog::Accepted) {
        return 0;
    }
    
    try {
        std::unique_ptr<IStorage> storage;
        if (dialog.useDatabase()) {
            QString dbPath = QDir::currentPath() + "/phonebook.db";
            storage = std::make_unique<DatabaseStorage>(dbPath);
        } else {
            QString filePath = QDir::currentPath() + "/phonebook.json";
            storage = std::make_unique<FileStorage>(filePath);
        }
        
        MainWindow w(std::move(storage));
        w.show();
        
        return a.exec();
    } catch (const std::exception& e) {
        QMessageBox::critical(nullptr, "Ошибка",
            QString("Не удалось инициализировать приложение: %1").arg(e.what()));
        return 1;
    }
}
