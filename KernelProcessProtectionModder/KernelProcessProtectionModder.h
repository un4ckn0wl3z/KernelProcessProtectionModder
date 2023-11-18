#pragma once

#include <QtWidgets/QWidget>
#include "ui_KernelProcessProtectionModder.h"
#include "ModderClient.h"

QT_BEGIN_NAMESPACE
namespace Ui { class KernelProcessProtectionModderClass; };
QT_END_NAMESPACE

class KernelProcessProtectionModder : public QWidget
{
    Q_OBJECT

public:
    KernelProcessProtectionModder(QWidget *parent = nullptr);
    ~KernelProcessProtectionModder();

private:
    Ui::KernelProcessProtectionModderClass *ui;
    ModderClient *m_client;
};
