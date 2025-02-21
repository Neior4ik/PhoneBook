#pragma once
#include <QObject>
#include <QDialog>
#include <QLineEdit>
#include <QDateEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QVector>
#include <QTableWidget>
#include "contact.h"
#include "phonewidget.h"

class ContactDialog : public QDialog {
    Q_OBJECT

public:
    explicit ContactDialog(QWidget* parent = nullptr);
    explicit ContactDialog(const Contact& initialContact, QWidget* parent = nullptr);
    ~ContactDialog();

    Contact getContact() const;
    void setContact(const Contact& newContact);

private slots:
    void onAddPhone();
    void onRemovePhone();
    void onAccept();

private:
    void setupUI();
    void setupConnections();
    bool validateInputs() const;
    void clearPhoneWidgets();
    void addPhoneWidget();

    QLineEdit* firstNameEdit{nullptr};
    QLineEdit* lastNameEdit{nullptr};
    QLineEdit* middleNameEdit{nullptr};
    QDateEdit* birthDateEdit{nullptr};
    QLineEdit* emailEdit{nullptr};
    QLineEdit* addressEdit{nullptr};
    QPushButton* addPhoneButton{nullptr};
    QPushButton* okButton{nullptr};
    QPushButton* cancelButton{nullptr};
    QList<PhoneWidget*> phoneWidgets;
    Contact contact;
};
