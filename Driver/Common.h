#pragma once
typedef struct _PS_PROTECTION
{
    UCHAR Type : 3;
    UCHAR Audit : 1;
    UCHAR Signer : 4;
} PS_PROTECTION, * PPS_PROTECTION;

typedef struct _PROCESS_PROTECTION_INFO
{
    UCHAR SignatureLevel;
    UCHAR SectionSignatureLevel;
    PS_PROTECTION Protection;
} PROCESS_PROTECTION_INFO, * PPROCESS_PROTECTION_INFO;

struct InputParameters
{
	int pid;
};

struct OutputParameters
{
	bool result;
};