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


typedef enum _WINDOWS_VERSION
{
	WINDOWS_UNSUPPORTED,
	WINDOWS_REDSTONE_1,		// 14393,
	WINDOWS_REDSTONE_2,		// 15063,
	WINDOWS_REDSTONE_3,		// 16299,
	WINDOWS_REDSTONE_4,		// 17134,
	WINDOWS_REDSTONE_5,		// 17763
	WINDOWS_19H1, 			// 18362
	WINDOWS_19H2,			// 18363
	WINDOWS_20H1,			// 19041
	WINDOWS_20H2,			// 19042
	WINDOWS_21H1,			// 19043
	WINDOWS_21H2			// 19044
} WINDOWS_VERSION, * PWINDOWS_VERSION;


const ULONG PROCESS_PROTECTION_OFFSET[] =
{
	0x00,   // placeholder
	0x00,   // placeholder
	0x00,   // placeholder
	0x00,   // placeholder
	0x00,   // placeholder
	0x6c8,  // REDSTONE_5
	0x00,   // placeholder
	0x00,   // placeholder
	0x00,   // placeholder
	0x00,   // placeholder
	0x00,   // placeholder
	0x87a    // WINDOWS_21H2
};

struct InputParameters
{
	int pid;
};

struct OutputParameters
{
	bool result;
};