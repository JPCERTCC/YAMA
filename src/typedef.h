#ifndef TYPEDEF_H
#define TYPEDEF_H

#include <Windows.h>

// [WARNING]
// THIS HEADER FILE CONFILICT WITH FOLLOWING LIBRARY
//
//  #include <winternl.h>
//
// DO NOT INCLUDE THIS HEADER FILE BEFORE INCLUDING <winternl.h>

/*
 * Namespace to define undocumented 32/64bit structures
 */

namespace nt {
typedef enum _THREADINFOCLASS {
    ThreadBasicInformation,           // q: THREAD_BASIC_INFORMATION
    ThreadTimes,                      // q: KERNEL_USER_TIMES
    ThreadPriority,                   // s: KPRIORITY
    ThreadBasePriority,               // s: LONG
    ThreadAffinityMask,               // s: KAFFINITY
    ThreadImpersonationToken,         // s: HANDLE
    ThreadDescriptorTableEntry,       // q: DESCRIPTOR_TABLE_ENTRY (or WOW64_DESCRIPTOR_TABLE_ENTRY)
    ThreadEnableAlignmentFaultFixup,  // s: BOOLEAN
    ThreadEventPair,                  //
    ThreadQuerySetWin32StartAddress,  // q: PVOID
    ThreadZeroTlsCell,                // 10
    ThreadPerformanceCount,           // q: LARGE_INTEGER
    ThreadAmILastThread,              // q: ULONG
    ThreadIdealProcessor,             // s: ULONG
    ThreadPriorityBoost,              // qs: ULONG
    ThreadSetTlsArrayAddress,         //
    ThreadIsIoPending,                // q: ULONG
    ThreadHideFromDebugger,           // s: void
    ThreadBreakOnTermination,         // qs: ULONG
    ThreadSwitchLegacyState,          //
    ThreadIsTerminated,               // 20, q: ULONG
    ThreadLastSystemCall,             // q: THREAD_LAST_SYSCALL_INFORMATION
    ThreadIoPriority,                 // qs: ULONG
    ThreadCycleTime,                  // q: THREAD_CYCLE_TIME_INFORMATION
    ThreadPagePriority,               // q: ULONG
    ThreadActualBasePriority,         //
    ThreadTebInformation,             // q: THREAD_TEB_INFORMATION (requires THREAD_GET_CONTEXT + THREAD_SET_CONTEXT)
    ThreadCSwitchMon,                 //
    ThreadCSwitchPmu,                 //
    ThreadWow64Context,               // q: WOW64_CONTEXT
    ThreadGroupInformation,           // 30, q: GROUP_AFFINITY
    ThreadUmsInformation,             //
    ThreadCounterProfiling,           //
    ThreadIdealProcessorEx,           // q: PROCESSOR_NUMBER
    ThreadCpuAccountingInformation,   // since WIN8
    ThreadSuspendCount,               // since WINBLUE
    ThreadHeterogeneousCpuPolicy,     // KHETERO_CPU_POLICY // since THRESHOLD
    ThreadContainerId,
    ThreadNameInformation,
    ThreadProperty,
    ThreadSelectedCpuSets,
    ThreadSystemThreadInformation,
    MaxThreadInfoClass
} THREADINFOCLASS;

typedef enum _LDR_DLL_LOAD_REASON {
    LoadReasonStaticDependency,
    LoadReasonStaticForwarderDependency,
    LoadReasonDynamicForwarderDependency,
    LoadReasonDelayloadDependency,
    LoadReasonDynamicLoad,
    LoadReasonAsImageLoad,
    LoadReasonAsDataLoad,
    LoadReasonUnknown = -1
} LDR_DLL_LOAD_REASON;

typedef enum _MEMORY_INFORMATION_CLASS {
    MemoryBasicInformation,           // MEMORY_BASIC_INFORMATION
    MemoryWorkingSetInformation,      // MEMORY_WORKING_SET_INFORMATION
    MemoryMappedFilenameInformation,  // UNICODE_STRING
    MemoryRegionInformation,          // MEMORY_REGION_INFORMATION
    MemoryWorkingSetExInformation,    // MEMORY_WORKING_SET_EX_INFORMATION
    MemorySharedCommitInformation,    // MEMORY_SHARED_COMMIT_INFORMATION
    MemoryImageInformation,           // MEMORY_IMAGE_INFORMATION
    MemoryRegionInformationEx,
    MemoryPrivilegedBasicInformation,
    MemoryEnclaveImageInformation,  // MEMORY_ENCLAVE_IMAGE_INFORMATION // since REDSTONE3
    MemoryBasicInformationCapped
} MEMORY_INFORMATION_CLASS;

}  // namespace nt

/*
 * Namespace to define undocument 64bit NT structures
 */
namespace nt64 {

typedef struct _UNICODE_STRING {
    union {
        struct {
            WORD Length;
            WORD MaximumLength;
        } u;
        ULONGLONG dummyalign;
    };
    ULONGLONG Buffer;
} UNICODE_STRING, *PUNICODE_STRING;

typedef struct _STRING {
    USHORT Length;         // 0x0
    USHORT MaximumLength;  // 0x2
    ULONGLONG Buffer;      // 0x8
} STRING, *PSTRING;

typedef LONG KPRIORITY;

typedef struct _CLIENT_ID {
    HANDLE UniqueProcess;
    HANDLE UniqueThread;
} CLIENT_ID, *PCLIENT_ID;

typedef struct _GDI_TEB_BATCH {
    ULONG Offset : 31;              // 0x0
    ULONG HasRenderingCommand : 1;  // 0x0
    ULONGLONG HDC;                  // 0x8
    ULONG Buffer[310];              // 0x10
} GDI_TEB_BATCH, *PGDI_TEB_BATCH;

typedef struct _NT_TIB {
    ULONGLONG ExceptionList;  // 0x0
    ULONGLONG StackBase;      // 0x8
    ULONGLONG StackLimit;     // 0x10
    ULONGLONG SubSystemTib;   // 0x18
    union {
        ULONGLONG FiberData;  // 0x20
        ULONG Version;        // 0x20
    };
    ULONGLONG ArbitraryUserPointer;  // 0x28
    ULONGLONG Self;                  // 0x30
} NT_TIB, *PNT_TIB;

typedef struct _RTL_DRIVE_LETTER_CURDIR {
    USHORT Flags;
    USHORT Length;
    ULONG TimeStamp;
    STRING DosPath;
} RTL_DRIVE_LETTER_CURDIR, *PRTL_DRIVE_LETTER_CURDIR;

struct _CURDIR {
    struct _UNICODE_STRING DosPath;  // 0x0
    VOID* Handle;                    // 0x10
};

typedef struct _RTL_USER_PROCESS_PARAMETERS {
    ULONG MaximumLength;                                    // 0x0
    ULONG Length;                                           // 0x4
    ULONG Flags;                                            // 0x8
    ULONG DebugFlags;                                       // 0xc
    ULONGLONG ConsoleHandle;                                // 0x10
    ULONG ConsoleFlags;                                     // 0x18
    ULONGLONG StandardInput;                                // 0x20
    ULONGLONG StandardOutput;                               // 0x28
    ULONGLONG StandardError;                                // 0x30
    struct _CURDIR CurrentDirectory;                        // 0x38
    UNICODE_STRING DllPath;                                 // 0x50
    UNICODE_STRING ImagePathName;                           // 0x60
    UNICODE_STRING CommandLine;                             // 0x70
    ULONGLONG Environment;                                  // 0x80
    ULONG StartingX;                                        // 0x88
    ULONG StartingY;                                        // 0x8c
    ULONG CountX;                                           // 0x90
    ULONG CountY;                                           // 0x94
    ULONG CountCharsX;                                      // 0x98
    ULONG CountCharsY;                                      // 0x9c
    ULONG FillAttribute;                                    // 0xa0
    ULONG WindowFlags;                                      // 0xa4
    ULONG ShowWindowFlags;                                  // 0xa8
    UNICODE_STRING WindowTitle;                             // 0xb0
    UNICODE_STRING DesktopInfo;                             // 0xc0
    UNICODE_STRING ShellInfo;                               // 0xd0
    UNICODE_STRING RuntimeData;                             // 0xe0
    struct _RTL_DRIVE_LETTER_CURDIR CurrentDirectores[32];  // 0xf0
    ULONGLONG EnvironmentSize;                              // 0x3f0
    ULONGLONG EnvironmentVersion;                           // 0x3f8
    ULONGLONG PackageDependencyData;                        // 0x400
    ULONG ProcessGroupId;                                   // 0x408
    ULONG LoaderThreads;                                    // 0x40c
} RTL_USER_PROCESS_PARAMETERS, *PRTL_USER_PROCESS_PARAMETERS;

typedef struct _PEB {
    UCHAR InheritedAddressSpace;     // 0x0
    UCHAR ReadImageFileExecOptions;  // 0x1
    UCHAR BeingDebugged;             // 0x2
    union {
        UCHAR BitField;  // 0x3
        struct {
            UCHAR ImageUsesLargePages : 1;           // 0x3
            UCHAR IsProtectedProcess : 1;            // 0x3
            UCHAR IsImageDynamicallyRelocated : 1;   // 0x3
            UCHAR SkipPatchingUser32Forwarders : 1;  // 0x3
            UCHAR IsPackagedProcess : 1;             // 0x3
            UCHAR IsAppContainer : 1;                // 0x3
            UCHAR IsProtectedProcessLight : 1;       // 0x3
            UCHAR IsLongPathAwareProcess : 1;        // 0x3
        };
    };
    UCHAR Padding0[4];                               // 0x4
    ULONGLONG Mutant;                                // 0x8
    ULONGLONG ImageBaseAddress;                      // 0x10
    ULONGLONG Ldr;                                   // 0x18
    PRTL_USER_PROCESS_PARAMETERS ProcessParameters;  // 0x20
    ULONGLONG SubSystemData;                         // 0x28
    ULONGLONG ProcessHeap;                           // 0x30
    ULONGLONG FastPebLock;                           // 0x38
    ULONGLONG AtlThunkSListPtr;                      // 0x40
    ULONGLONG IFEOKey;                               // 0x48
    union {
        ULONG CrossProcessFlags;  // 0x50
        struct {
            ULONG ProcessInJob : 1;                // 0x50
            ULONG ProcessInitializing : 1;         // 0x50
            ULONG ProcessUsingVEH : 1;             // 0x50
            ULONG ProcessUsingVCH : 1;             // 0x50
            ULONG ProcessUsingFTH : 1;             // 0x50
            ULONG ProcessPreviouslyThrottled : 1;  // 0x50
            ULONG ProcessCurrentlyThrottled : 1;   // 0x50
            ULONG ProcessImagesHotPatched : 1;     // 0x50
            ULONG ReservedBits0 : 24;              // 0x50
        };
    };
    UCHAR Padding1[4];  // 0x54
    union {
        ULONGLONG KernelCallbackTable;  // 0x58
        ULONGLONG UserSharedInfoPtr;    // 0x58
    };
    ULONG SystemReserved;                          // 0x60
    ULONG AtlThunkSListPtr32;                      // 0x64
    ULONGLONG ApiSetMap;                           // 0x68
    ULONG TlsExpansionCounter;                     // 0x70
    UCHAR Padding2[4];                             // 0x74
    ULONGLONG TlsBitmap;                           // 0x78
    ULONG TlsBitmapBits[2];                        // 0x80
    ULONGLONG ReadOnlySharedMemoryBase;            // 0x88
    ULONGLONG SharedData;                          // 0x90
    ULONGLONG ReadOnlyStaticServerData;            // 0x98
    ULONGLONG AnsiCodePageData;                    // 0xa0
    ULONGLONG OemCodePageData;                     // 0xa8
    ULONGLONG UnicodeCaseTableData;                // 0xb0
    ULONG NumberOfProcessors;                      // 0xb8
    ULONG NtGlobalFlag;                            // 0xbc
    union _LARGE_INTEGER CriticalSectionTimeout;   // 0xc0
    ULONGLONG HeapSegmentReserve;                  // 0xc8
    ULONGLONG HeapSegmentCommit;                   // 0xd0
    ULONGLONG HeapDeCommitTotalFreeThreshold;      // 0xd8
    ULONGLONG HeapDeCommitFreeBlockThreshold;      // 0xe0
    ULONG NumberOfHeaps;                           // 0xe8
    ULONG MaximumNumberOfHeaps;                    // 0xec
    ULONGLONG ProcessHeaps;                        // 0xf0
    ULONGLONG GdiSharedHandleTable;                // 0xf8
    ULONGLONG ProcessStarterHelper;                // 0x100
    ULONG GdiDCAttributeList;                      // 0x108
    UCHAR Padding3[4];                             // 0x10c
    ULONGLONG LoaderLock;                          // 0x110
    ULONG OSMajorVersion;                          // 0x118
    ULONG OSMinorVersion;                          // 0x11c
    USHORT OSBuildNumber;                          // 0x120
    USHORT OSCSDVersion;                           // 0x122
    ULONG OSPlatformId;                            // 0x124
    ULONG ImageSubsystem;                          // 0x128
    ULONG ImageSubsystemMajorVersion;              // 0x12c
    ULONG ImageSubsystemMinorVersion;              // 0x130
    UCHAR Padding4[4];                             // 0x134
    ULONGLONG ActiveProcessAffinityMask;           // 0x138
    ULONG GdiHandleBuffer[60];                     // 0x140
    ULONGLONG PostProcessInitRoutine;              // 0x230
    ULONGLONG TlsExpansionBitmap;                  // 0x238
    ULONG TlsExpansionBitmapBits[32];              // 0x240
    ULONG SessionId;                               // 0x2c0
    UCHAR Padding5[4];                             // 0x2c4
    union _ULARGE_INTEGER AppCompatFlags;          // 0x2c8
    union _ULARGE_INTEGER AppCompatFlagsUser;      // 0x2d0
    ULONGLONG pShimData;                           // 0x2d8
    ULONGLONG AppCompatInfo;                       // 0x2e0
    struct _STRING CSDVersion;                     // 0x2e8
    ULONGLONG ActivationContextData;               // 0x2f8
    ULONGLONG ProcessAssemblyStorageMap;           // 0x300
    ULONGLONG SystemDefaultActivationContextData;  // 0x308
    ULONGLONG SystemAssemblyStorageMap;            // 0x310
    ULONGLONG MinimumStackCommit;                  // 0x318
    ULONGLONG SparePointers[4];                    // 0x320
    ULONG SpareUlongs[5];                          // 0x340
    ULONGLONG WerRegistrationData;                 // 0x358
    ULONGLONG WerShipAssertPtr;                    // 0x360
    ULONGLONG pUnused;                             // 0x368
    ULONGLONG pImageHeaderHash;                    // 0x370
} PEB, *PPEB;

typedef struct _TEB {
    NT_TIB NtTib;
    ULONGLONG EnvironmentPointer;
    CLIENT_ID ClientId;
    ULONGLONG ActiveRpcHandle;
    ULONGLONG ThreadLocalStoragePointer;
    PPEB ProcessEnvironmentBlock;
    ULONG LastErrorValue;
    ULONG CountOfOwnedCriticalSections;
    ULONGLONG CsrClientThread;
    ULONGLONG Win32ThreadInfo;
    ULONG User32Reserved[26];
    ULONG UserReserved[5];
    ULONGLONG WOW32Reserved;
    LCID CurrentLocale;
    ULONG FpSoftwareStatusRegister;
    ULONGLONG SystemReserved1[54];
    NTSTATUS ExceptionCode;
    ULONGLONG ActivationContextStackPointer;
    UCHAR SpareBytes[24];
    ULONG TxFsContext;
    GDI_TEB_BATCH GdiTebBatch;
    CLIENT_ID RealClientId;
    HANDLE GdiCachedProcessHandle;
    ULONG GdiClientPID;
    ULONG GdiClientTID;
    ULONGLONG GdiThreadLocalInfo;
    ULONG_PTR Win32ClientInfo[62];
    ULONGLONG glDispatchTable[233];
    ULONG_PTR glReserved1[29];
    ULONGLONG glReserved2;
    ULONGLONG glSectionInfo;
    ULONGLONG glSection;
    ULONGLONG glTable;
    ULONGLONG glCurrentRC;
    ULONGLONG glContext;
    NTSTATUS LastStatusValue;
    UNICODE_STRING StaticUnicodeString;
    WCHAR StaticUnicodeBuffer[261];
    ULONGLONG DeallocationStack;
    ULONGLONG TlsSlots[64];
    LIST_ENTRY TlsLinks;
    ULONGLONG Vdm;
    ULONGLONG ReservedForNtRpc;
    ULONGLONG DbgSsReserved[2];
    ULONG HardErrorMode;
    ULONGLONG Instrumentation[11];
    GUID ActivityId;
    ULONGLONG SubProcessTag;
    ULONGLONG EtwLocalData;
    ULONGLONG EtwTraceData;
    ULONGLONG WinSockData;
    ULONG GdiBatchCount;
    union {
        PROCESSOR_NUMBER CurrentIdealProcessor;
        ULONG IdealProcessorValue;
        struct {
            UCHAR ReservedPad0;
            UCHAR ReservedPad1;
            UCHAR ReservedPad2;
            UCHAR IdealProcessor;
        };
    };

    ULONG GuaranteedStackBytes;
    ULONGLONG ReservedForPerf;
    ULONGLONG ReservedForOle;
    ULONG WaitingOnLoaderLock;
    ULONGLONG SavedPriorityState;
    ULONG_PTR SoftPatchPtr1;
    ULONGLONG ThreadPoolData;
    ULONGLONG TlsExpansionSlots;
    ULONGLONG DeallocationBStore;
    ULONGLONG BStoreLimit;
    ULONG MuiGeneration;
    ULONG IsImpersonating;
    ULONGLONG NlsCache;
    ULONGLONG pShimData;
    ULONG HeapVirtualAffinity;
    HANDLE CurrentTransactionHandle;
    ULONGLONG ActiveFrame;  // PTEB_ACTIVE_FRAME ActiveFrame;
    ULONGLONG FlsData;
    ULONGLONG PreferredLanguages;
    ULONGLONG UserPrefLanguages;
    ULONGLONG MergedPrefLanguages;
    ULONG MuiImpersonation;
    union {
        USHORT CrossTebFlags;
        USHORT SpareCrossTebBits : 16;
    };
    union {
        USHORT SameTebFlags;
        struct {
            USHORT SafeThunkCall : 1;
            USHORT InDebugPrint : 1;
            USHORT HasFiberData : 1;
            USHORT SkipThreadAttach : 1;
            USHORT WerInShipAssertCode : 1;
            USHORT RanProcessInit : 1;
            USHORT ClonedThread : 1;
            USHORT SuppressDebugMsg : 1;
            USHORT DisableUserStackWalk : 1;
            USHORT RtlExceptionAttached : 1;
            USHORT InitialThread : 1;
            USHORT SessionAware : 1;
            USHORT SpareSameTebBits : 4;
        };
    };
    ULONGLONG TxnScopeEnterCallback;
    ULONGLONG TxnScopeExitCallback;
    ULONGLONG TxnScopeContext;
    ULONG LockCount;
    ULONG SpareUlong0;
    ULONGLONG ResourceRetValue;
    ULONGLONG ReservedForWdf;
} TEB, *PTEB;

typedef struct _THREAD_BASIC_INFORMATION {
    NTSTATUS ExitStatus;
    PTEB TebBaseAddress;
    CLIENT_ID ClientId;
    ULONG_PTR AffinityMask;
    KPRIORITY Priority;
    LONG BasePriority;
} THREAD_BASIC_INFORMATION, *PTHREAD_BASIC_INFORMATION;

typedef struct LDR_DATA_TABLE_ENTRY {
    LIST_ENTRY InLoadOrderLinks;
    LIST_ENTRY InMemoryOrderLinks;
    // one of the LIST_ENTRY does not exist. maybe undocumented.
    // union {
    //     LIST_ENTRY InInitializationOrderLinks;
    //     LIST_ENTRY InProgressLinks;
    // };
    ULONGLONG DllBase;
    ULONGLONG EntryPoint;
    ULONG SizeOfImage;
    UNICODE_STRING FullDllName;
    UNICODE_STRING BaseDllName;
    union {
        UCHAR FlagGroup[4];
        ULONG Flags;
        struct {
            ULONG PackagedBinary : 1;
            ULONG MarkedForRemoval : 1;
            ULONG ImageDll : 1;
            ULONG LoadNotificationsSent : 1;
            ULONG TelemetryEntryProcessed : 1;
            ULONG ProcessStaticImport : 1;
            ULONG InLegacyLists : 1;
            ULONG InIndexes : 1;
            ULONG ShimDll : 1;
            ULONG InExceptionTable : 1;
            ULONG ReservedFlags1 : 2;
            ULONG LoadInProgress : 1;
            ULONG LoadConfigProcessed : 1;
            ULONG EntryProcessed : 1;
            ULONG ProtectDelayLoad : 1;
            ULONG ReservedFlags3 : 2;
            ULONG DontCallForThreads : 1;
            ULONG ProcessAttachCalled : 1;
            ULONG ProcessAttachFailed : 1;
            ULONG CorDeferredValidate : 1;
            ULONG CorImage : 1;
            ULONG DontRelocate : 1;
            ULONG CorILOnly : 1;
            ULONG ReservedFlags5 : 3;
            ULONG Redirected : 1;
            ULONG ReservedFlags6 : 2;
            ULONG CompatDatabaseProcessed : 1;
        };
    };
    ULONGLONG padding;
    USHORT ObsoleteLoadCount;
    USHORT TlsIndex;
    LIST_ENTRY HashLinks;
    ULONG TimeDateStamp;
    struct _ACTIVATION_CONTEXT* EntryPointActivationContext;
    ULONGLONG Lock;
    ULONGLONG DdagNode;
    LIST_ENTRY NodeModuleLink;
    struct _LDRP_LOAD_CONTEXT* LoadContext;
    ULONGLONG ParentDllBase;
    ULONGLONG SwitchBackContext;
    UCHAR BaseAddressIndexNode[0x10];
    UCHAR MappingInfoIndexNode[0x10];
    ULONG_PTR OriginalBase;
    LARGE_INTEGER LoadTime;
    ULONG BaseNameHashValue;
    nt::LDR_DLL_LOAD_REASON LoadReason;
    ULONG ImplicitPathOptions;
    ULONG ReferenceCount;
    ULONG DependentLoadFlags;
    UCHAR SigningLevel;
} LDR_DATA_TABLE_ENTRY, *PLDR_DATA_TABLE_ENTRY;

typedef struct _MEMORY_IMAGE_INFORMATION {
    ULONGLONG ImageBase;
    SIZE_T SizeOfImage;
    union {
        ULONG ImageFlags;
        struct {
            ULONG ImagePartialMap : 1;
            ULONG ImageNotExecutable : 1;
            ULONG ImageSigningLevel : 4;  // REDSTONE3
            ULONG Reserved : 26;
        };
    };
} MEMORY_IMAGE_INFORMATION, *PMEMORY_IMAGE_INFORMATION;

}  // namespace nt64

namespace nt32 {

typedef struct _UNICODE_STRING {
    union {
        struct {
            WORD Length;
            WORD MaximumLength;
        } u;
        ULONG dummyalign;
    };
    ULONG Buffer;
} UNICODE_STRING, *PUNICODE_STRING;

typedef struct _STRING {
    USHORT Length;         // 0x0
    USHORT MaximumLength;  // 0x2
    ULONG Buffer;          // 0x8
} STRING, *PSTRING;

typedef LONG KPRIORITY;

typedef struct _CLIENT_ID {
    HANDLE UniqueProcess;
    HANDLE UniqueThread;
} CLIENT_ID, *PCLIENT_ID;

typedef struct _GDI_TEB_BATCH {
    ULONG Offset : 31;              // 0x0
    ULONG HasRenderingCommand : 1;  // 0x0
    ULONG HDC;                      // 0x8
    ULONG Buffer[310];              // 0x10
} GDI_TEB_BATCH, *PGDI_TEB_BATCH;

typedef struct _NT_TIB {
    ULONG ExceptionList;  // 0x0
    ULONG StackBase;      // 0x8
    ULONG StackLimit;     // 0x10
    ULONG SubSystemTib;   // 0x18
    union {
        ULONG FiberData;  // 0x20
        ULONG Version;    // 0x20
    };
    ULONG ArbitraryUserPointer;  // 0x28
    ULONG Self;                  // 0x30
} NT_TIB, *PNT_TIB;

typedef struct _RTL_DRIVE_LETTER_CURDIR {
    USHORT Flags;
    USHORT Length;
    ULONG TimeStamp;
    STRING DosPath;
} RTL_DRIVE_LETTER_CURDIR, *PRTL_DRIVE_LETTER_CURDIR;

struct _CURDIR {
    struct _UNICODE_STRING DosPath;  // 0x0
    VOID* Handle;                    // 0x10
};

typedef struct _RTL_USER_PROCESS_PARAMETERS {
    ULONG MaximumLength;                                    // 0x0
    ULONG Length;                                           // 0x4
    ULONG Flags;                                            // 0x8
    ULONG DebugFlags;                                       // 0xc
    ULONG ConsoleHandle;                                    // 0x10
    ULONG ConsoleFlags;                                     // 0x18
    ULONG StandardInput;                                    // 0x20
    ULONG StandardOutput;                                   // 0x28
    ULONG StandardError;                                    // 0x30
    struct _CURDIR CurrentDirectory;                        // 0x38
    UNICODE_STRING DllPath;                                 // 0x50
    UNICODE_STRING ImagePathName;                           // 0x60
    UNICODE_STRING CommandLine;                             // 0x70
    ULONG Environment;                                      // 0x80
    ULONG StartingX;                                        // 0x88
    ULONG StartingY;                                        // 0x8c
    ULONG CountX;                                           // 0x90
    ULONG CountY;                                           // 0x94
    ULONG CountCharsX;                                      // 0x98
    ULONG CountCharsY;                                      // 0x9c
    ULONG FillAttribute;                                    // 0xa0
    ULONG WindowFlags;                                      // 0xa4
    ULONG ShowWindowFlags;                                  // 0xa8
    UNICODE_STRING WindowTitle;                             // 0xb0
    UNICODE_STRING DesktopInfo;                             // 0xc0
    UNICODE_STRING ShellInfo;                               // 0xd0
    UNICODE_STRING RuntimeData;                             // 0xe0
    struct _RTL_DRIVE_LETTER_CURDIR CurrentDirectores[32];  // 0xf0
    ULONG EnvironmentSize;                                  // 0x3f0
    ULONG EnvironmentVersion;                               // 0x3f8
    ULONG PackageDependencyData;                            // 0x400
    ULONG ProcessGroupId;                                   // 0x408
    ULONG LoaderThreads;                                    // 0x40c
} RTL_USER_PROCESS_PARAMETERS, *PRTL_USER_PROCESS_PARAMETERS;

typedef struct _PEB {
    UCHAR InheritedAddressSpace;     // 0x0
    UCHAR ReadImageFileExecOptions;  // 0x1
    UCHAR BeingDebugged;             // 0x2
    union {
        UCHAR BitField;  // 0x3
        struct {
            UCHAR ImageUsesLargePages : 1;           // 0x3
            UCHAR IsProtectedProcess : 1;            // 0x3
            UCHAR IsImageDynamicallyRelocated : 1;   // 0x3
            UCHAR SkipPatchingUser32Forwarders : 1;  // 0x3
            UCHAR IsPackagedProcess : 1;             // 0x3
            UCHAR IsAppContainer : 1;                // 0x3
            UCHAR IsProtectedProcessLight : 1;       // 0x3
            UCHAR IsLongPathAwareProcess : 1;        // 0x3
        };
    };
    UCHAR Padding0[4];                               // 0x4
    ULONG Mutant;                                    // 0x8
    ULONG ImageBaseAddress;                          // 0x10
    ULONG Ldr;                                       // 0x18
    PRTL_USER_PROCESS_PARAMETERS ProcessParameters;  // 0x20
    ULONG SubSystemData;                             // 0x28
    ULONG ProcessHeap;                               // 0x30
    ULONG FastPebLock;                               // 0x38
    ULONG AtlThunkSListPtr;                          // 0x40
    ULONG IFEOKey;                                   // 0x48
    union {
        ULONG CrossProcessFlags;  // 0x50
        struct {
            ULONG ProcessInJob : 1;                // 0x50
            ULONG ProcessInitializing : 1;         // 0x50
            ULONG ProcessUsingVEH : 1;             // 0x50
            ULONG ProcessUsingVCH : 1;             // 0x50
            ULONG ProcessUsingFTH : 1;             // 0x50
            ULONG ProcessPreviouslyThrottled : 1;  // 0x50
            ULONG ProcessCurrentlyThrottled : 1;   // 0x50
            ULONG ProcessImagesHotPatched : 1;     // 0x50
            ULONG ReservedBits0 : 24;              // 0x50
        };
    };
    UCHAR Padding1[4];  // 0x54
    union {
        ULONG KernelCallbackTable;  // 0x58
        ULONG UserSharedInfoPtr;    // 0x58
    };
    ULONG SystemReserved;                         // 0x60
    ULONG AtlThunkSListPtr32;                     // 0x64
    ULONG ApiSetMap;                              // 0x68
    ULONG TlsExpansionCounter;                    // 0x70
    UCHAR Padding2[4];                            // 0x74
    ULONG TlsBitmap;                              // 0x78
    ULONG TlsBitmapBits[2];                       // 0x80
    ULONG ReadOnlySharedMemoryBase;               // 0x88
    ULONG SharedData;                             // 0x90
    ULONG ReadOnlyStaticServerData;               // 0x98
    ULONG AnsiCodePageData;                       // 0xa0
    ULONG OemCodePageData;                        // 0xa8
    ULONG UnicodeCaseTableData;                   // 0xb0
    ULONG NumberOfProcessors;                     // 0xb8
    ULONG NtGlobalFlag;                           // 0xbc
    union _LARGE_INTEGER CriticalSectionTimeout;  // 0xc0
    ULONG HeapSegmentReserve;                     // 0xc8
    ULONG HeapSegmentCommit;                      // 0xd0
    ULONG HeapDeCommitTotalFreeThreshold;         // 0xd8
    ULONG HeapDeCommitFreeBlockThreshold;         // 0xe0
    ULONG NumberOfHeaps;                          // 0xe8
    ULONG MaximumNumberOfHeaps;                   // 0xec
    ULONG ProcessHeaps;                           // 0xf0
    ULONG GdiSharedHandleTable;                   // 0xf8
    ULONG ProcessStarterHelper;                   // 0x100
    ULONG GdiDCAttributeList;                     // 0x108
    UCHAR Padding3[4];                            // 0x10c
    ULONG LoaderLock;                             // 0x110
    ULONG OSMajorVersion;                         // 0x118
    ULONG OSMinorVersion;                         // 0x11c
    USHORT OSBuildNumber;                         // 0x120
    USHORT OSCSDVersion;                          // 0x122
    ULONG OSPlatformId;                           // 0x124
    ULONG ImageSubsystem;                         // 0x128
    ULONG ImageSubsystemMajorVersion;             // 0x12c
    ULONG ImageSubsystemMinorVersion;             // 0x130
    UCHAR Padding4[4];                            // 0x134
    ULONG ActiveProcessAffinityMask;              // 0x138
    ULONG GdiHandleBuffer[60];                    // 0x140
    ULONG PostProcessInitRoutine;                 // 0x230
    ULONG TlsExpansionBitmap;                     // 0x238
    ULONG TlsExpansionBitmapBits[32];             // 0x240
    ULONG SessionId;                              // 0x2c0
    UCHAR Padding5[4];                            // 0x2c4
    union _ULARGE_INTEGER AppCompatFlags;         // 0x2c8
    union _ULARGE_INTEGER AppCompatFlagsUser;     // 0x2d0
    ULONG pShimData;                              // 0x2d8
    ULONG AppCompatInfo;                          // 0x2e0
    struct _STRING CSDVersion;                    // 0x2e8
    ULONG ActivationContextData;                  // 0x2f8
    ULONG ProcessAssemblyStorageMap;              // 0x300
    ULONG SystemDefaultActivationContextData;     // 0x308
    ULONG SystemAssemblyStorageMap;               // 0x310
    ULONG MinimumStackCommit;                     // 0x318
    ULONG SparePointers[4];                       // 0x320
    ULONG SpareUlongs[5];                         // 0x340
    ULONG WerRegistrationData;                    // 0x358
    ULONG WerShipAssertPtr;                       // 0x360
    ULONG pUnused;                                // 0x368
    ULONG pImageHeaderHash;                       // 0x370
} PEB, *PPEB;

typedef struct _TEB {
    NT_TIB NtTib;
    ULONG EnvironmentPointer;
    CLIENT_ID ClientId;
    ULONG ActiveRpcHandle;
    ULONG ThreadLocalStoragePointer;
    PPEB ProcessEnvironmentBlock;
    ULONG LastErrorValue;
    ULONG CountOfOwnedCriticalSections;
    ULONG CsrClientThread;
    ULONG Win32ThreadInfo;
    ULONG User32Reserved[26];
    ULONG UserReserved[5];
    ULONG WOW32Reserved;
    LCID CurrentLocale;
    ULONG FpSoftwareStatusRegister;
    ULONG SystemReserved1[54];
    NTSTATUS ExceptionCode;
    ULONG ActivationContextStackPointer;
    UCHAR SpareBytes[24];
    ULONG TxFsContext;
    GDI_TEB_BATCH GdiTebBatch;
    CLIENT_ID RealClientId;
    HANDLE GdiCachedProcessHandle;
    ULONG GdiClientPID;
    ULONG GdiClientTID;
    ULONG GdiThreadLocalInfo;
    ULONG_PTR Win32ClientInfo[62];
    ULONG glDispatchTable[233];
    ULONG_PTR glReserved1[29];
    ULONG glReserved2;
    ULONG glSectionInfo;
    ULONG glSection;
    ULONG glTable;
    ULONG glCurrentRC;
    ULONG glContext;
    NTSTATUS LastStatusValue;
    UNICODE_STRING StaticUnicodeString;
    WCHAR StaticUnicodeBuffer[261];
    ULONG DeallocationStack;
    ULONG TlsSlots[64];
    LIST_ENTRY TlsLinks;
    ULONG Vdm;
    ULONG ReservedForNtRpc;
    ULONG DbgSsReserved[2];
    ULONG HardErrorMode;
    ULONG Instrumentation[11];
    GUID ActivityId;
    ULONG SubProcessTag;
    ULONG EtwLocalData;
    ULONG EtwTraceData;
    ULONG WinSockData;
    ULONG GdiBatchCount;
    union {
        PROCESSOR_NUMBER CurrentIdealProcessor;
        ULONG IdealProcessorValue;
        struct {
            UCHAR ReservedPad0;
            UCHAR ReservedPad1;
            UCHAR ReservedPad2;
            UCHAR IdealProcessor;
        };
    };

    ULONG GuaranteedStackBytes;
    ULONG ReservedForPerf;
    ULONG ReservedForOle;
    ULONG WaitingOnLoaderLock;
    ULONG SavedPriorityState;
    ULONG_PTR SoftPatchPtr1;
    ULONG ThreadPoolData;
    ULONG TlsExpansionSlots;
    ULONG DeallocationBStore;
    ULONG BStoreLimit;
    ULONG MuiGeneration;
    ULONG IsImpersonating;
    ULONG NlsCache;
    ULONG pShimData;
    ULONG HeapVirtualAffinity;
    HANDLE CurrentTransactionHandle;
    ULONG ActiveFrame;  // PTEB_ACTIVE_FRAME ActiveFrame;
    ULONG FlsData;
    ULONG PreferredLanguages;
    ULONG UserPrefLanguages;
    ULONG MergedPrefLanguages;
    ULONG MuiImpersonation;
    union {
        USHORT CrossTebFlags;
        USHORT SpareCrossTebBits : 16;
    };
    union {
        USHORT SameTebFlags;
        struct {
            USHORT SafeThunkCall : 1;
            USHORT InDebugPrint : 1;
            USHORT HasFiberData : 1;
            USHORT SkipThreadAttach : 1;
            USHORT WerInShipAssertCode : 1;
            USHORT RanProcessInit : 1;
            USHORT ClonedThread : 1;
            USHORT SuppressDebugMsg : 1;
            USHORT DisableUserStackWalk : 1;
            USHORT RtlExceptionAttached : 1;
            USHORT InitialThread : 1;
            USHORT SessionAware : 1;
            USHORT SpareSameTebBits : 4;
        };
    };
    ULONG TxnScopeEnterCallback;
    ULONG TxnScopeExitCallback;
    ULONG TxnScopeContext;
    ULONG LockCount;
    ULONG SpareUlong0;
    ULONG ResourceRetValue;
    ULONG ReservedForWdf;
} TEB, *PTEB;

typedef struct _THREAD_BASIC_INFORMATION {
    NTSTATUS ExitStatus;
    PTEB TebBaseAddress;
    CLIENT_ID ClientId;
    ULONG_PTR AffinityMask;
    KPRIORITY Priority;
    LONG BasePriority;
} THREAD_BASIC_INFORMATION, *PTHREAD_BASIC_INFORMATION;

typedef struct LDR_DATA_TABLE_ENTRY {
    LIST_ENTRY InLoadOrderLinks;
    LIST_ENTRY InMemoryOrderLinks;
    // one of the LIST_ENTRY does not exist. maybe undocumented.
    // union {
    //     LIST_ENTRY InInitializationOrderLinks;
    //     LIST_ENTRY InProgressLinks;
    // };
    ULONG DllBase;
    ULONG EntryPoint;
    ULONG SizeOfImage;
    UNICODE_STRING FullDllName;
    UNICODE_STRING BaseDllName;
    union {
        UCHAR FlagGroup[4];
        ULONG Flags;
        struct {
            ULONG PackagedBinary : 1;
            ULONG MarkedForRemoval : 1;
            ULONG ImageDll : 1;
            ULONG LoadNotificationsSent : 1;
            ULONG TelemetryEntryProcessed : 1;
            ULONG ProcessStaticImport : 1;
            ULONG InLegacyLists : 1;
            ULONG InIndexes : 1;
            ULONG ShimDll : 1;
            ULONG InExceptionTable : 1;
            ULONG ReservedFlags1 : 2;
            ULONG LoadInProgress : 1;
            ULONG LoadConfigProcessed : 1;
            ULONG EntryProcessed : 1;
            ULONG ProtectDelayLoad : 1;
            ULONG ReservedFlags3 : 2;
            ULONG DontCallForThreads : 1;
            ULONG ProcessAttachCalled : 1;
            ULONG ProcessAttachFailed : 1;
            ULONG CorDeferredValidate : 1;
            ULONG CorImage : 1;
            ULONG DontRelocate : 1;
            ULONG CorILOnly : 1;
            ULONG ReservedFlags5 : 3;
            ULONG Redirected : 1;
            ULONG ReservedFlags6 : 2;
            ULONG CompatDatabaseProcessed : 1;
        };
    };
    ULONG padding;
    USHORT ObsoleteLoadCount;
    USHORT TlsIndex;
    LIST_ENTRY HashLinks;
    ULONG TimeDateStamp;
    struct _ACTIVATION_CONTEXT* EntryPointActivationContext;
    ULONG Lock;
    ULONG DdagNode;
    LIST_ENTRY NodeModuleLink;
    struct _LDRP_LOAD_CONTEXT* LoadContext;
    ULONG ParentDllBase;
    ULONG SwitchBackContext;
    UCHAR BaseAddressIndexNode[0x10];
    UCHAR MappingInfoIndexNode[0x10];
    ULONG_PTR OriginalBase;
    LARGE_INTEGER LoadTime;
    ULONG BaseNameHashValue;
    nt::LDR_DLL_LOAD_REASON LoadReason;
    ULONG ImplicitPathOptions;
    ULONG ReferenceCount;
    ULONG DependentLoadFlags;
    UCHAR SigningLevel;
} LDR_DATA_TABLE_ENTRY, *PLDR_DATA_TABLE_ENTRY;

typedef struct _MEMORY_IMAGE_INFORMATION {
    ULONG ImageBase;
    SIZE_T SizeOfImage;
    union {
        ULONG ImageFlags;
        struct {
            ULONG ImagePartialMap : 1;
            ULONG ImageNotExecutable : 1;
            ULONG ImageSigningLevel : 4;  // REDSTONE3
            ULONG Reserved : 26;
        };
    };
} MEMORY_IMAGE_INFORMATION, *PMEMORY_IMAGE_INFORMATION;

}  // namespace nt32

#endif  // TYPEDEF_H