//
// Created by Sargis Boyajyan on 26.02.24.
//

// You may need to build the project (run Qt uic code generator) to get "ui_add_user_dialog.h" resolved

#include <QDialogButtonBox>
#include <QRegularExpressionValidator>
#include <QRegularExpression>
#include <QMessageBox>
#include "add_user_dialog.h"
#include "ui_add_user_dialog.h"


AddUserDialog::AddUserDialog(QWidget *parent) :
        QDialog(parent), ui(new Ui::AddUserDialog) {
    ui->setupUi(this);

    connect(ui->okButton, &QPushButton::clicked, [=] {
        this->done(QDialog::Accepted);
    });

    connect(ui->cancelButton, &QPushButton::clicked, [=] {
        this->done(QDialog::Rejected);
    });

	connect(this, &QDialog::accepted, this, &AddUserDialog::validateAndAccept);
}

AddUserDialog::~AddUserDialog() {
    delete ui;
}

QString AddUserDialog::getUsername() {
    return ui->usernameEdit->text();
}

QString AddUserDialog::getFirstName() {
    return ui->firstNameEdit->text();
}

QString AddUserDialog::getLastName() {
    return ui->lastNameEdit->text();
}

QStringList AddUserDialog::getPreferredGames() {
    QStringList checkedItems;

    if (ui->xo3CheckBox->isChecked()) {
        checkedItems << ui->xo3CheckBox->text();
    }

    if (ui->xo4CheckBox->isChecked()) {
        checkedItems << ui->xo4CheckBox->text();
    }

    if (ui->xo5CheckBox->isChecked()) {
        checkedItems << ui->xo5CheckBox->text();
    }

    return checkedItems;
}

void AddUserDialog::validateAndAccept() {
	// Perform your validation using a regular expression
	QRegularExpressionValidator validator(QRegularExpression("^[A-Za-z0-9_-]{1,16}$"), this);
	int pos = 0;
	QString user = ui->usernameEdit->text();
	QString firstName = ui->firstNameEdit->text();
	QString lastName = ui->lastNameEdit->text();

	QStringList warnings;

	if (validator.validate(user, pos) != QValidator::Acceptable) {
		warnings.append("Invalid Username. Please check your data.");
	}

	validator.setRegularExpression(QRegularExpression("^[A-Z][a-z]*$"));

	if (validator.validate(firstName, pos) != QValidator::Acceptable) {
		warnings.append("Invalid First name. Please check your data.");
	}

	validator.setRegularExpression(QRegularExpression("^(O'|d')[A-Z][a-z]*$|^[A-Z][a-z]*$"));
	if (validator.validate(lastName, pos) != QValidator::Acceptable) {
		warnings.append("Invalid Last name. Please check your data.");
	}

	if (!ui->xo3CheckBox->isChecked() && !ui->xo4CheckBox->isChecked() && !ui->xo5CheckBox->isChecked()) {
		warnings.append("At least one game should be selected.");
	}

	if(!warnings.isEmpty()) {
		QMessageBox::critical(this, "Validation Error", warnings.join('\n'));
		reject();
	}
}

