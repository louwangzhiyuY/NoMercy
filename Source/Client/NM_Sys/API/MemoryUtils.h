#pragma once

namespace AddressRange 
{
	inline BOOLEAN IsUserAddress(PVOID Address) 
	{
		return reinterpret_cast<SIZE_T>(Address) < (static_cast<SIZE_T>(1) << (8 * sizeof(SIZE_T) - 1));
	}

	inline BOOLEAN IsKernelAddress(PVOID Address) 
	{
		return reinterpret_cast<SIZE_T>(Address) >= (static_cast<SIZE_T>(1) << (8 * sizeof(SIZE_T) - 1));
	}
}

namespace VirtualMemory 
{
	// Allocates non-paged not initialized memory from pool:
	_IRQL_requires_max_(DISPATCH_LEVEL)
	PVOID AllocFromPool(SIZE_T Bytes, BOOLEAN FillByZeroes = TRUE);

	// Such as AllocFromPool but with executable rights:
	_IRQL_requires_max_(DISPATCH_LEVEL)
	PVOID AllocFromPoolExecutable(SIZE_T Bytes);

	// Allocates zero-initialized null-terminated ANSI-string (char*):
	_IRQL_requires_max_(DISPATCH_LEVEL)
	LPSTR AllocAnsiString(SIZE_T Characters);

	// Allocates zero-initialized null-terminated Unicode string (wchar_t*);
	_IRQL_requires_max_(DISPATCH_LEVEL)
	LPWSTR AllocWideString(SIZE_T Characters);

	// Allocates zero-initialized buffer to contain an array:
	_IRQL_requires_max_(DISPATCH_LEVEL)
	PVOID AllocArray(SIZE_T ElementSize, SIZE_T ElementsCount);

	// Free memory allocated by Alloc[FromPool[Executable]/AnsiString/WideString/Array]:
	_IRQL_requires_max_(DISPATCH_LEVEL)
	VOID FreePoolMemory(__drv_freesMem(Mem) PVOID Address);

	_IRQL_requires_max_(APC_LEVEL)
	PVOID AllocNonCachedNorInitialized(SIZE_T Bytes);

	_IRQL_requires_max_(APC_LEVEL)
	VOID FreeNonCachedMemory(PVOID Address, SIZE_T Bytes);

	// Secure user memory from page rights descending or memory freeing:
	_IRQL_requires_max_(APC_LEVEL)
	BOOLEAN SecureMemory(__in_data_source(USER_MODE) PVOID UserAddress, SIZE_T Size, ULONG ProtectRights, /* PAGE_*** */ OUT PHANDLE SecureHandle);

	// Secure user memory of specified process from page rights descending or memory freeing:
	_IRQL_requires_max_(APC_LEVEL)
	BOOLEAN SecureProcessMemory(PEPROCESS Process,__in_data_source(USER_MODE) PVOID UserAddress, SIZE_T Size, ULONG ProtectRights, /* PAGE_*** */ OUT PHANDLE SecureHandle);

	_IRQL_requires_max_(APC_LEVEL)
	VOID UnsecureMemory(HANDLE SecureHandle);

	_IRQL_requires_max_(APC_LEVEL)
	VOID UnsecureProcessMemory(PEPROCESS Process, HANDLE SecureHandle);

	// Check whether access to address causes page fault:
	_IRQL_requires_max_(DISPATCH_LEVEL)
	BOOLEAN IsAddressValid(PVOID Address);

	_IRQL_requires_max_(APC_LEVEL)
	BOOLEAN CheckUserMemoryReadable(__in_data_source(USER_MODE) PVOID UserAddress, SIZE_T Size);

	_IRQL_requires_max_(APC_LEVEL)
    BOOLEAN CheckProcessMemoryReadable(PEPROCESS Process, __in_data_source(USER_MODE) PVOID UserAddress, SIZE_T Size);

	_IRQL_requires_max_(APC_LEVEL)
	BOOLEAN CheckUserMemoryWriteable(__in_data_source(USER_MODE) PVOID UserAddress, SIZE_T Size);

	_IRQL_requires_max_(APC_LEVEL)
    BOOLEAN CheckProcessMemoryWriteable(PEPROCESS Process, __in_data_source(USER_MODE) PVOID UserAddress, SIZE_T Size);
}

namespace Heap 
{
	// Creates a growable heap and returns a HeapHandle:
	_IRQL_requires_max_(APC_LEVEL)
	PVOID CreateHeap();

	// Allocates memory from heap and returns allocated buffer address:
	_IRQL_requires_max_(APC_LEVEL)
	PVOID AllocHeap(PVOID HeapHandle, SIZE_T Size, BOOLEAN ZeroMemory = TRUE);

	_IRQL_requires_max_(APC_LEVEL)
	VOID FreeHeap(PVOID HeapHandle, _Frees_ptr_opt_ PVOID Address);

	// Free all of heap memory and destroy the heap object:
	_IRQL_requires_max_(APC_LEVEL)
	VOID DestroyHeap(PVOID HeapHandle);

	// Class for simplification of heap using:
	class HeapObject final
	{
		public:
			_IRQL_requires_max_(APC_LEVEL)
			HeapObject();

			_IRQL_requires_max_(APC_LEVEL)
			~HeapObject();
		
			BOOLEAN IsHeapValid() const;
		
			_IRQL_requires_max_(APC_LEVEL)
			PVOID Alloc(SIZE_T Size, BOOLEAN ZeroMemory = TRUE) const;
		
			_IRQL_requires_max_(APC_LEVEL)
			VOID Free(PVOID Address) const;
		
			_IRQL_requires_max_(APC_LEVEL)
			LPSTR AllocAnsiString(SIZE_T Characters) const;
		
			_IRQL_requires_max_(APC_LEVEL)
			LPWSTR AllocWideString(SIZE_T Characters) const;
		
			_IRQL_requires_max_(APC_LEVEL)
			PVOID AllocArray(SIZE_T ElementSize, SIZE_T ElementsCount) const;

		private:
			PVOID HeapHandle;
	};
}

namespace PhysicalMemory 
{
	extern const int DMI_SIZE;

	// DMI/SMBIOS size is 65536 bytes (0xF0000..0xFFFFF in physical memory):
	_IRQL_requires_max_(DISPATCH_LEVEL)
	BOOLEAN ReadDmiMemory(OUT PVOID Buffer, SIZE_T Size);

	// Allocates physically-contiguous memory;
	// Highest acceptable address is a highest physical 
	// address that can be used to allocate memory:
	_IRQL_requires_max_(DISPATCH_LEVEL)
	PVOID AllocPhysicalMemory(PVOID64 HighestAcceptableAddress, SIZE_T Size);

	_IRQL_requires_max_(DISPATCH_LEVEL)
	VOID FreePhysicalMemory(PVOID BaseVirtualAddress);

	// Map physical address-space to virtual memory:
	_IRQL_requires_max_(DISPATCH_LEVEL)
	PVOID MapPhysicalMemory(PVOID64 PhysicalAddress, SIZE_T Size);

	_IRQL_requires_max_(DISPATCH_LEVEL)
	VOID UnmapPhysicalMemory(PVOID64 MappedPhysicalMemory, SIZE_T Size);

	// Get the physical address for non-paged virtual memory:
	_IRQL_requires_max_(APC_LEVEL)
	PVOID64 GetPhysicalAddress(PVOID VirtualAddress, OPTIONAL PEPROCESS Process = NULL);

	_IRQL_requires_max_(DISPATCH_LEVEL)
	BOOLEAN ReadPhysicalMemory(IN PVOID64 PhysicalAddress, OUT PVOID Buffer, SIZE_T Length);

	_IRQL_requires_max_(DISPATCH_LEVEL)
	BOOLEAN WritePhysicalMemory(OUT PVOID64 PhysicalAddress, IN PVOID Buffer, SIZE_T Length);
}

namespace Mdl {
	_IRQL_requires_max_(APC_LEVEL)
	PMDL AllocMdlAndLockPages(PVOID Address, ULONG Size, KPROCESSOR_MODE AccessMode = KernelMode, LOCK_OPERATION Operation = IoReadAccess, OPTIONAL PEPROCESS Process = NULL);

	_IRQL_requires_max_(APC_LEVEL)
	VOID UnlockPagesAndFreeMdl(PMDL Mdl);

	// Maps the specified Mdl from specified process (or from kernel) 
	// to address space of desired process:
	//	- Mdl			: Mdl you want to map
	//	- MappedMemory   : address of variable that receives the base address of mapped memory
	//	- SrcProcess	 : target process from which we want to map memory (or NULL to current process or if VA is a kernel address)
	//	- DestProcess	: process to which memory should be mapped (or NULL to current process or to map to kernel memory)
	//	- NeedLock	   : whether need to lock memory pages by MmProbeAndLock[Process]Pages (if pages already locked, set it to FALSE)
	//	- AccessMode	 : KernelMode/UserMode - maps to kernel or current user address space
	//	- Protect		: PAGE_*** protection rights for the mapped memory
	//	- CacheType	  : cache type of mapped memory
	//	- UserRequestedAddress : if AccessMode == UserMode it specifies the target address to which memory will map,
	//							 or NULL to choose address automatically (if AccessMode == KernelMode this param must be NULL)							  
	// You can unmap memory with UnmapMdl function.
	_IRQL_requires_max_(APC_LEVEL)
	NTSTATUS MapMdl(IN PMDL Mdl, OUT PVOID* MappedMemory, OPTIONAL PEPROCESS SrcProcess, OPTIONAL PEPROCESS DestProcess, BOOLEAN NeedLock, KPROCESSOR_MODE AccessMode = KernelMode, 
		ULONG Protect = PAGE_READWRITE, MEMORY_CACHING_TYPE CacheType = MmNonCached, OPTIONAL PVOID UserRequestedAddress = NULL	
	);

	_IRQL_requires_max_(APC_LEVEL)
	VOID UnmapMdl(IN PMDL Mdl, IN PVOID MappedMemory, BOOLEAN NeedUnlock);

	// Result type of MapMemory function:
	using MAPPING_INFO = struct 
	{
		PMDL Mdl;
		PVOID BaseAddress;
	};
	using PMAPPING_INFO = MAPPING_INFO*;

	// Maps the memory from specified process (or from kernel) 
	// to address space of desired process:
	//	- MappingInfo	: output struct with mapping info (necessary for unmapping)
	//	- SrcProcess	 : target process from which we want to map memory (or NULL to current process or if VA is a kernel address)
	//	- DestProcess	: process to which memory should be mapped (or NULL to current process or to map to kernel memory)
	//	- VirtualAddress : usermode or kernelmode address of memory we want to map
	//	- Size		   : size in bytes of mapped block
	//	- AccessMode	 : KernelMode/UserMode - maps to kernel or current user address space
	//	- Protect		: PAGE_*** protection rights for the mapped memory
	//	- CacheType	  : cache type of mapped memory
	//	- UserRequestedAddress : if AccessMode == UserMode it specifies the target address to which memory will map,
	//							 or NULL to choose address automatically (if AccessMode == KernelMode this param must be NULL)							  
	// You can unmap memory with UnmapMemory function.
	_IRQL_requires_max_(APC_LEVEL)
	NTSTATUS MapMemory(OUT PMAPPING_INFO MappingInfo, OPTIONAL PEPROCESS SrcProcess, OPTIONAL PEPROCESS DestProcess, IN PVOID VirtualAddress, ULONG Size, 
		KPROCESSOR_MODE AccessMode = KernelMode, ULONG Protect = PAGE_READWRITE, MEMORY_CACHING_TYPE CacheType = MmNonCached, OPTIONAL PVOID UserRequestedAddress = NULL
	);

	_IRQL_requires_max_(APC_LEVEL)
	VOID UnmapMemory(IN PMAPPING_INFO MappingInfo);
}