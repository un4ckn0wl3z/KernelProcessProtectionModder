#pragma warning(disable: 4996)
#include <ntifs.h>
#include <ntddk.h>
#include <aux_klib.h>
#include "IOCTLs.h"
#include "Common.h"

void DriverCleanup(PDRIVER_OBJECT DriverObject);

NTSTATUS CreateClose(_In_ PDEVICE_OBJECT DeviceObject, _In_ PIRP Irp);
NTSTATUS DeviceControl(_In_ PDEVICE_OBJECT DeviceObject, _In_ PIRP Irp);
WINDOWS_VERSION GetWindowsVersion();


UNICODE_STRING deviceName = RTL_CONSTANT_STRING(L"\\Device\\PModder");
UNICODE_STRING symlink = RTL_CONSTANT_STRING(L"\\??\\PModder");

extern "C"
NTSTATUS
DriverEntry(
	_In_ PDRIVER_OBJECT DriverObject,
	_In_ PUNICODE_STRING RegistryPath)
{
	UNREFERENCED_PARAMETER(RegistryPath);
	DbgPrint(("[+] DriverEntry called!\n"));
	DriverObject->DriverUnload = DriverCleanup;
	DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = DeviceControl;
	DriverObject->MajorFunction[IRP_MJ_CREATE] = CreateClose;
	DriverObject->MajorFunction[IRP_MJ_CLOSE] = CreateClose;

	PDEVICE_OBJECT deviceObject;

	NTSTATUS status = IoCreateDevice(
		DriverObject,
		0,
		&deviceName,
		FILE_DEVICE_UNKNOWN,
		0,
		FALSE,
		&deviceObject
	);

	if (!NT_SUCCESS(status))
	{
		DbgPrint("[!] Failed to create Device Object (0x%08X)\n", status);
		return status;
	}

	status = IoCreateSymbolicLink(&symlink, &deviceName);

	if (!NT_SUCCESS(status))
	{
		DbgPrint("[!] Failed to create symlink (0x%08X)\n", status);
		IoDeleteDevice(deviceObject);
		return status;
	}

	return STATUS_SUCCESS;
}


NTSTATUS DeviceControl(_In_ PDEVICE_OBJECT DeviceObject, _In_ PIRP Irp)
{
	UNREFERENCED_PARAMETER(DeviceObject);

	NTSTATUS status = STATUS_SUCCESS;
	ULONG_PTR length = 0;


	// check Windows version
	WINDOWS_VERSION windowsVersion = GetWindowsVersion();

	if (windowsVersion == WINDOWS_UNSUPPORTED)
	{
		status = STATUS_NOT_SUPPORTED;
		KdPrint(("[!] Windows Version Unsupported\n"));

		Irp->IoStatus.Status = status;
		Irp->IoStatus.Information = length;

		IoCompleteRequest(Irp, IO_NO_INCREMENT);

		return status;
	}

	PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(Irp);

	switch (stack->Parameters.DeviceIoControl.IoControlCode)
	{
	case PMODDER_UNPROTECT:
	{
		DbgPrint(("[+] PMODDER_UNPROTECT requested!\n"));
		if (stack->Parameters.DeviceIoControl.InputBufferLength < sizeof(InputParameters))
		{
			status = STATUS_BUFFER_TOO_SMALL;
			DbgPrint(("[!] STATUS_BUFFER_TOO_SMALL\n"));
			break;
		}
		InputParameters* input = (InputParameters*)stack->Parameters.DeviceIoControl.Type3InputBuffer;

		if (input == nullptr)
		{
			status = STATUS_INVALID_PARAMETER;
			DbgPrint(("[!] STATUS_INVALID_PARAMETER\n"));
			break;
		}

		PEPROCESS eProcess = NULL;
		status = PsLookupProcessByProcessId((HANDLE)input->pid, &eProcess);

		if (!NT_SUCCESS(status))
		{
			DbgPrint("[!] PsLookupProcessByProcessId failed (0x%08X)\n", status);
			break;
		}
		DbgPrint("[+] Got EPROCESS for PID %d (0x%08X)\n", input->pid, eProcess);

		PROCESS_PROTECTION_INFO* psProtection = (PROCESS_PROTECTION_INFO*)(((ULONG_PTR)eProcess) + PROCESS_PROTECTION_OFFSET[windowsVersion]);


		if (psProtection == nullptr)
		{
			status = STATUS_INVALID_PARAMETER;
			DbgPrint("[!] Failed to read PROCESS_PROTECTION_INFO\n");
			break;
		}

		// 0 the values
		psProtection->SignatureLevel = 0;
		psProtection->SectionSignatureLevel = 0;
		psProtection->Protection.Type = 0;
		psProtection->Protection.Signer = 0;

		DbgPrint("[+] Removing Process Protection for PID %d\n", input->pid);

		// dereference eProcess
		ObDereferenceObject(eProcess);

		break;
	}
	case PMODDER_PROTECT:
	{
		DbgPrint(("[+] PMODDER_PROTECT requested!\n"));
		if (stack->Parameters.DeviceIoControl.InputBufferLength < sizeof(InputParameters))
		{
			status = STATUS_BUFFER_TOO_SMALL;
			DbgPrint(("[!] STATUS_BUFFER_TOO_SMALL\n"));
			break;
		}
		InputParameters* input = (InputParameters*)stack->Parameters.DeviceIoControl.Type3InputBuffer;

		if (input == nullptr)
		{
			status = STATUS_INVALID_PARAMETER;
			DbgPrint(("[!] STATUS_INVALID_PARAMETER\n"));
			break;
		}

		PEPROCESS eProcess = NULL;
		status = PsLookupProcessByProcessId((HANDLE)input->pid, &eProcess);

		if (!NT_SUCCESS(status))
		{
			DbgPrint("[!] PsLookupProcessByProcessId failed (0x%08X)\n", status);
			break;
		}
		DbgPrint("[+] Got EPROCESS for PID %d (0x%08X)\n", input->pid, eProcess);


		PROCESS_PROTECTION_INFO* psProtection = (PROCESS_PROTECTION_INFO*)(((ULONG_PTR)eProcess) + PROCESS_PROTECTION_OFFSET[windowsVersion]);


		if (psProtection == nullptr)
		{
			status = STATUS_INVALID_PARAMETER;
			DbgPrint("[!] Failed to read PROCESS_PROTECTION_INFO\n");
			break;
		}
		psProtection->SignatureLevel = 30;
		psProtection->SectionSignatureLevel = 28;
		psProtection->Protection.Type = 2;
		psProtection->Protection.Signer = 6;

		DbgPrint("[+] Setting Process Protection for PID %d\nn", input->pid);

		// dereference eProcess
		ObDereferenceObject(eProcess);

		break;
	}
	default:
		status = STATUS_INVALID_DEVICE_REQUEST;
		DbgPrint("[!] STATUS_INVALID_DEVICE_REQUEST\n");
		break;
	}

	Irp->IoStatus.Status = status;
	Irp->IoStatus.Information = length;

	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	return status;
}


NTSTATUS CreateClose(_In_ PDEVICE_OBJECT DeviceObject, _In_ PIRP Irp)
{
	UNREFERENCED_PARAMETER(DeviceObject);
	DbgPrint("[+] PModder CreateClose called\n");
	Irp->IoStatus.Status = STATUS_SUCCESS;
	Irp->IoStatus.Information = 0;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}

void DriverCleanup(PDRIVER_OBJECT DriverObject)
{
	DbgPrint("[+] PModder DriverCleanup called\n");
	IoDeleteSymbolicLink(&symlink);
	IoDeleteDevice(DriverObject->DeviceObject);
}



WINDOWS_VERSION
GetWindowsVersion()
{
	RTL_OSVERSIONINFOW info;
	info.dwOSVersionInfoSize = sizeof(info);

	NTSTATUS status = RtlGetVersion(&info);

	if (!NT_SUCCESS(status))
	{
		DbgPrint("[!] RtlGetVersion failed (0x%08X)\n", status);
		return WINDOWS_UNSUPPORTED;
	}

	DbgPrint("[+] Windows Version %d.%d\n", info.dwMajorVersion, info.dwBuildNumber);

	if (info.dwMajorVersion != 10)
	{
		return WINDOWS_UNSUPPORTED;
	}

	switch (info.dwBuildNumber)
	{
	case 17763:
		return WINDOWS_REDSTONE_5;
	case 19044:
		return WINDOWS_21H2;
	default:
		return WINDOWS_UNSUPPORTED;
	}
}