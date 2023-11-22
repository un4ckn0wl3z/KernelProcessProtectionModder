#include "KernelProcessProtectionModder.h"
#include <QDebug>
#include <QMessageBox>

KernelProcessProtectionModder::KernelProcessProtectionModder(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::KernelProcessProtectionModderClass())
{
    ui->setupUi(this);
    this->m_client = new ModderClient(this);

    if (!this->m_client->isDriverAvailable()) {
        QMessageBox::warning(this, "Driver not found", "Please install Driver before execute this program", QMessageBox::Ok);
        exit(EXIT_FAILURE);
    }

    connect(ui->unprotectButton, &QPushButton::clicked, [=]() {
        if (ui->pidLineEdit->text() == "") return;
        bool result = this->m_client->unprotect(ui->pidLineEdit->text().toInt());
        if (!result) {
            QMessageBox::critical(this, "Error", "Cannot unprotect target pid", QMessageBox::Ok);
        }
        QMessageBox::information(this, "Success", "Target process unprotected!", QMessageBox::Ok);
    });

    connect(ui->protectButton, &QPushButton::clicked, [=]() {
        if (ui->pidLineEdit->text() == "") return;
        bool result = this->m_client->protect(ui->pidLineEdit->text().toInt());
        if (!result) {
            QMessageBox::critical(this, "Error", "Cannot protect target pid", QMessageBox::Ok);
        }
        QMessageBox::information(this, "Success", "Target process protected!", QMessageBox::Ok);
    });

    connect(ui->grantPrivsButton, &QPushButton::clicked, [=]() {
        if (ui->pidLineEdit->text() == "") return;
        bool result = this->m_client->grantPrivs(ui->pidLineEdit->text().toInt());
        if (!result) {
            QMessageBox::critical(this, "Error", "Cannot grant privs for target pid", QMessageBox::Ok);
        }
        QMessageBox::information(this, "Success", "Target process grant privs!", QMessageBox::Ok);
    });

}

KernelProcessProtectionModder::~KernelProcessProtectionModder()
{
    delete ui;
}
