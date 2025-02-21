#pragma once
#include <QDialog>
#include <QRadioButton>
#include <QPushButton>
#include <QVBoxLayout>

class StorageChoiceDialog : public QDialog {
    Q_OBJECT
public:
    explicit StorageChoiceDialog(QWidget* parent = nullptr);
    bool useDatabase() const;
    
private:
    QRadioButton* dbRadio;
    QRadioButton* fileRadio;
}; 