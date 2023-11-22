#include "ModderClient.h"
#include "..\Driver\IOCTLs.h"
#include "..\Driver\Common.h"
#include <QDebug>

ModderClient::ModderClient(QObject *parent)
	: QObject(parent)
{
    HANDLE hDriver = CreateFile(
        L"\\\\.\\PModder",
        GENERIC_WRITE,
        FILE_SHARE_WRITE,
        nullptr,
        OPEN_EXISTING,
        0,
        nullptr);

    if (hDriver == INVALID_HANDLE_VALUE)
    {
        qDebug() << "[ModderClient] - Cannot open handler for driver";
        this->m_isDriverAvailable = false;
        return;
    }

    this->m_diver = hDriver;
}

ModderClient::~ModderClient()
{
	CloseHandle(this->m_diver);
}

bool ModderClient::unprotect(int pid)
{
    InputParameters input;
    input.pid = pid;

    qDebug() << "[ModderClient] - target pid: " << input.pid;

    BOOL success = DeviceIoControl(
        this->m_diver,
        PMODDER_UNPROTECT,
        &input,          // pointer to the data
        sizeof(input),   // the size of the data
        nullptr,
        0,
        nullptr,
        nullptr);

    if (!success)
    {
        return false;
    }

    return true;
}

bool ModderClient::protect(int pid)
{
    InputParameters input;
    input.pid = pid;

    qDebug() << "[ModderClient] - target pid: " << input.pid;

    BOOL success = DeviceIoControl(
        this->m_diver,
        PMODDER_PROTECT,
        &input,          // pointer to the data
        sizeof(input),   // the size of the data
        nullptr,
        0,
        nullptr,
        nullptr);

    if (!success)
    {
        return false;
    }

    return true;
}

bool ModderClient::grantPrivs(int pid)
{
    InputParameters input;
    input.pid = pid;

    qDebug() << "[ModderClient] - target pid: " << input.pid;

    BOOL success = DeviceIoControl(
        this->m_diver,
        PMODDER_GRANT,
        &input,          // pointer to the data
        sizeof(input),   // the size of the data
        nullptr,
        0,
        nullptr,
        nullptr);

    if (!success)
    {
        return false;
    }

    return true;
}

bool ModderClient::isDriverAvailable() const
{
    return this->m_isDriverAvailable;
}
