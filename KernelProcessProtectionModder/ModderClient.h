#pragma once
#include <Windows.h>
#include <QObject>

class ModderClient  : public QObject
{
	Q_OBJECT

public:
	ModderClient(QObject *parent);
	~ModderClient();

	bool unprotect(int pid);
	bool protect(int pid);
	bool isDriverAvailable() const;

private:
	HANDLE m_diver = NULL;
	bool m_isDriverAvailable = true;
};
