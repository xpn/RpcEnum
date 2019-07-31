#pragma once
#include <Windows.h>

#define MAX_SIMPLE_DICT_ENTRIES			0x200

#define ULONG_PTR_T ULONG_PTR
#define PTR_T *

#define MAX_RPC_INTERFACE_ANNOTATION			64
#define SIMPLE_DICT_SMALL_ARRAY					4


#define RPC_CORE_DESCRIPTION					"Windows 10 64bits runtime core"
#define RPC_CORE_IS_WOW64						FALSE

#define ULONG_PTR_T ULONG_PTR
#define PTR_T *


#define MAX_RPC_INTERFACE_ANNOTATION			64
#define SIMPLE_DICT_SMALL_ARRAY					4


//==============================================================================
// From Winnt.h
// The following structures are redefined to support Wow64 ptr
//
struct _RTL_CRITICAL_SECTION_T;

typedef struct _LIST_ENTRY_T {
	struct _LIST_ENTRY PTR_T Flink;
	struct _LIST_ENTRY PTR_T Blink;
} LIST_ENTRY_T, * PLIST_ENTRY_T;


typedef struct _RTL_CRITICAL_SECTION_DEBUG_T {
	WORD   									Type;
	WORD   									CreatorBackTraceIndex;
	struct _RTL_CRITICAL_SECTION_T PTR_T	CriticalSection;
	LIST_ENTRY_T							ProcessLocksList;
	DWORD 									EntryCount;
	DWORD 									ContentionCount;
	DWORD 									Flags;
	WORD   									CreatorBackTraceIndexHigh;
	WORD   									SpareWORD;
} RTL_CRITICAL_SECTION_DEBUG_T, PTR_T PRTL_CRITICAL_SECTION_DEBUG_T;

typedef struct _RTL_CRITICAL_SECTION_T {
	PRTL_CRITICAL_SECTION_DEBUG_T	DebugInfo;
	//
	//  The following three fields control entering and exiting the critical
	//  section for the resource
	//
	LONG 						LockCount;
	LONG 						RecursionCount;
	VOID PTR_T					OwningThread;        // from the thread's ClientId->UniqueThread
	VOID PTR_T					LockSemaphore;
	VOID PTR_T					SpinCount;			// force size on 64-bit systems when packed
} RTL_CRITICAL_SECTION_T, PTR_T PRTL_CRITICAL_SECTION_T;

//==============================================================================
// From RpcDceP.h
//
typedef struct _RPC_DISPATCH_TABLE_T {
	UINT							DispatchTableCount;
	RPC_DISPATCH_FUNCTION  PTR_T	DispatchTable;
	ULONG_PTR_T                      Reserved;
} RPC_DISPATCH_TABLE_T, PTR_T PRPC_DISPATCH_TABLE_T;

typedef struct _RPC_PROTSEQ_ENDPOINT_T {
	UCHAR PTR_T RpcProtocolSequence;
	UCHAR PTR_T Endpoint;
} RPC_PROTSEQ_ENDPOINT_T, PTR_T PRPC_PROTSEQ_ENDPOINT_T;

typedef struct _RPC_SERVER_INTERFACE_T {
	UINT					Length;
	RPC_IF_ID				InterfaceId;
	RPC_IF_ID				TransferSyntax;
	PRPC_DISPATCH_TABLE_T	DispatchTable;
	UINT					RpcProtseqEndpointCount;
	PRPC_PROTSEQ_ENDPOINT_T RpcProtseqEndpoint;
	RPC_MGR_EPV PTR_T		DefaultManagerEpv;
	void const PTR_T		InterpreterInfo;
	UINT					Flags;
} RPC_SERVER_INTERFACE_T, PTR_T PRPC_SERVER_INTERFACE_T;


typedef struct _NDR_EXPR_DESC_T
{
	const unsigned short PTR_T	pOffset;
	const unsigned char	PTR_T	pFormatExpr;
} NDR_EXPR_DESC_T;


/*
 * MIDL Stub Descriptor
 */
typedef struct _MIDL_STUB_DESC_T {
	void  PTR_T						RpcInterfaceInformation;
	void  PTR_T						pfnAllocate;
	void  PTR_T						pfnFree;
	void  PTR_T						pAutoHandle;
	const VOID  PTR_T				apfnNdrRundownRoutines;
	const VOID  PTR_T				aGenericBindingRoutinePairs;
	const VOID  PTR_T				apfnExprEval;
	const VOID  PTR_T				aXmitQuintuple;
	const unsigned char  PTR_T		pFormatTypes;
	int								fCheckBounds;
	/* Ndr library version. */
	unsigned long					Version;
	VOID PTR_T						pMallocFreeStruct;
	long							MIDLVersion;
	const COMM_FAULT_OFFSETS  PTR_T	CommFaultOffsets;
	// New fields for version 3.0+
	const VOID PTR_T				aUserMarshalQuadruple;
	// Notify routines - added for NT5, MIDL 5.0
	const VOID PTR_T				NotifyRoutineTable;
	/*
	 * Reserved for future use.
	 */
	ULONG_PTR_T						mFlags;
	// International support routines - added for 64bit post NT5
	const VOID	PTR_T				CsRoutineTables;
	void  PTR_T						ProxyServerInfo;
	const NDR_EXPR_DESC_T	PTR_T	pExprInfo;
	// Fields up to now present in win2000 release.
} MIDL_STUB_DESC_T, PTR_T PMIDL_STUB_DESC_T;


/*
 * Server Interpreter's information strucuture.
 */
typedef struct  _MIDL_SERVER_INFO_T {
	PMIDL_STUB_DESC_T				pStubDesc;
	const VOID	PTR_T	PTR_T		DispatchTable;
	const unsigned char		PTR_T	ProcString;
	const unsigned short	PTR_T	FmtStringOffset;
	const VOID PTR_T PTR_T			ThunkTable;
	RPC_IF_ID PTR_T					pTransferSyntax;
	ULONG_PTR_T						nCount;
	VOID PTR_T						pSyntaxInfo;
} MIDL_SERVER_INFO_T, PTR_T PMIDL_SERVER_INFO_T;

//==============================================================================
// Common private structures from rpctr4.dll.
// These structures seems to be constant on all the runtime versions.
//
#pragma pack(1)
typedef struct _SIMPLE_DICT_T {
	VOID PTR_T PTR_T	pArray;
	UINT				ArraySizeInBytes;	//to change : countof array elements
	UINT				NumberOfEntries;
	VOID PTR_T			SmallArray[SIMPLE_DICT_SMALL_ARRAY];
}SIMPLE_DICT_T, PTR_T PSIMPLE_DICT_T;

typedef struct _QUEUE_T {
	VOID PTR_T	Tail;
	VOID PTR_T	Head;
	ULONG		Lentgh;
	VOID PTR_T	SmallArray[SIMPLE_DICT_SMALL_ARRAY];
}QUEUE_T;

typedef struct _MUTEX_T {
	RTL_CRITICAL_SECTION_T	CriticalSection;
}MUTEX_T;

typedef struct _EVENT_T {
	ULONG	hEvent;
} EVENT_T;

#pragma pack()

#define RPC_ADDRESS_TYPE_DG		0x400000
#define RPC_ADDRESS_TYPE_LRPC	0x800000
#define RPC_ADDRESS_TYPE_OSF	0x800

#pragma pack(1)
typedef struct _RPC_SERVER_T {
	MUTEX_T						Mutex;
	ULONG							__bIslistening;
	ULONG							bIsListening;
	ULONG							MinimumCallThreads;
	ULONG							Wait;
	ULONG							OutCalls;
	ULONG							Unk1;
	ULONG							InCalls;
	ULONG							Unk2;
	SIMPLE_DICT_T					AddressDict;
	ULONG							lAvailableCalls;
	ULONG							Unk3;
	SIMPLE_DICT_T					_ProtSeqQueue;
	ULONG							Unk4[8];
	ULONG							OutPackets;
	ULONG							Unk5;
	MUTEX_T						Mutex2;
	ULONG							MaxCalls;
	ULONG							Unk6;
	VOID PTR_T					hEvent;
	ULONG							Unk7[4];
	SIMPLE_DICT_T					InterfaceDict;
	ULONG							_bIsListening;
	ULONG							bIsMaxCalls1234;
	ULONG							Unk8[6];
	ULONG							InPackets;
	ULONG							Unk9;
	RPC_FORWARD_FUNCTION PTR_T	pRpcForwardFunction;
	ULONG							Unk10[6];
	SIMPLE_DICT_T					AuthenInfoDict;
	LIST_ENTRY_T					RpcIfGroupListEntry;
	ULONG	PTR_T					__SRWLock;
	LIST_ENTRY_T					field_1E0;
}RPC_SERVER_T, PTR_T PRPC_SERVER_T;

typedef struct _RPC_INTERFACE_T
{
	PRPC_SERVER_T				pRpcServer;
	ULONG						Flags;
	ULONG						Unk1;
	MUTEX_T					Mutex;
	ULONG						EpMapperFlags;
	ULONG						Unk2;
	RPC_MGR_EPV PTR_T			pMgrEpv;
	RPC_IF_CALLBACK_FN PTR_T	IfCallbackFn;
	RPC_SERVER_INTERFACE_T	RpcServerInterface;
	PMIDL_SYNTAX_INFO			pSyntaxInfo;
	VOID PTR_T				pTransfertSyntaxes;
	ULONG						TransfertSyntaxesCount;
	ULONG						__Field_C4;
	ULONG						NbTypeManager;
	ULONG						MaxRpcSize;
	UUID_VECTOR  PTR_T		pUuidVector;
	SIMPLE_DICT_T				RpcInterfaceManagerDict;
	UCHAR						Annotation[MAX_RPC_INTERFACE_ANNOTATION];
	ULONG						IsCallSizeLimitReached;
	ULONG						currentNullManagerCalls;
	ULONG						__Field_150;
	ULONG						__Field_154;
	ULONG						__Field_158;
	ULONG						SecurityCallbackInProgress;
	ULONG						SecurityCacheEntry;
	ULONG						field_164;
	VOID PTR_T				__SecurityCacheEntries[16];
	SIMPLE_DICT_T				FwEpDict;
	ULONG						Unk3[6];
	struct RPCP_INTERFACE_GROUP PTR_T pRpcpInterfaceGroup;
}RPC_INTERFACE_T, PTR_T PRPC_INTERFACE_T;

#define RPC_ADDRESS_TYPE_DG		0x400000
#define RPC_ADDRESS_TYPE_LRPC	0x800000
#define RPC_ADDRESS_TYPE_OSF	0x800

typedef struct _RPC_ADDRESS_T {
	VOID PTR_T				pVtable;
	ULONG					Magic;
	ULONG					TypeOfAddress;
	ULONG					ReferenceCounter;
	ULONG					Unk1[3];
	WCHAR PTR_T				Name;
	WCHAR PTR_T				Protocole;
	WCHAR PTR_T				Address;
	ULONG					bNamed;
	ULONG					EpAddAddressFlag;
	SIMPLE_DICT_T			__LRPCSassociationSimpleDict;
	ULONG					__Field_68;
	ULONG					Unk3;
	ULONG					NbActiveCalls;
	ULONG					__Field_74;
	ULONG					Unk4[6];
	ULONG					__Field_90;
	MUTEX_T					Mutex;
}RPC_ADDRESS_T;

#pragma pack()


