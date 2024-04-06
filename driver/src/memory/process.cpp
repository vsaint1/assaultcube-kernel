#include "process.h"

uintptr_t process::get_kernel_module_base(char* module_name)
{
	ULONG64 module_base = 0;
	ULONG module_size = 0;
	PRTL_PROCESS_MODULES modules = NULL;
	NTSTATUS status = ZwQuerySystemInformation(SystemModuleInformation, 0, 0, &module_size);

	if (status != STATUS_INFO_LENGTH_MISMATCH)
		return 0;


	modules = (PRTL_PROCESS_MODULES)ExAllocatePool2(POOL_FLAG_NON_PAGED, module_size, KM_POOL_TAG);
	if (!modules)
		return 0;

	status = ZwQuerySystemInformation(SystemModuleInformation, modules, module_size, &module_size);
	if (!NT_SUCCESS(status)) {
		ExFreePoolWithTag(modules, KM_POOL_TAG);
		return 0;
	}

	PRTL_PROCESS_MODULE_INFORMATION module = modules->Modules;
	for (ULONG i = 0; i < modules->NumberOfModules; i++) {

		if (strcmp((char*)module[i].FullPathName + module[i].OffsetToFileName, module_name) == 0) {
			module_base = (ULONG64)module[i].ImageBase;
			break;
		}
	}

	ExFreePoolWithTag(modules, KM_POOL_TAG);
	return module_base;
}

// https://www.vergiliusproject.com/kernels/x64/Windows%2010%20%7C%202016 
// windows 22h2 version

int process::get_process_id_by_name(PEPROCESS start_proc, const char* name)
{

	PLIST_ENTRY list_entry;
	PEPROCESS curr_proc = start_proc;

	do
	{
		PKPROCESS kproc = (PKPROCESS)curr_proc;
		PDISPATCHER_HEADER header = (PDISPATCHER_HEADER)kproc;
		LPSTR current_process_name = (LPSTR)((PUCHAR)curr_proc + 0x5a8); // _EPROCESS.ImageFileName

		if (header->SignalState == 0 && strcmp(current_process_name, name) == 0)
		{	
			return (int)PsGetProcessId(curr_proc);
		}

		list_entry = (PLIST_ENTRY)((PUCHAR)curr_proc + 0x448); // _EPROCESS.ActiveProcessLinks
		curr_proc = (PEPROCESS)(list_entry->Flink);
		curr_proc = (PEPROCESS)((PUCHAR)curr_proc - 0x448); // _EPROCESS.ActiveProcessLinks 

	} while (start_proc != curr_proc);

	return 0;
}

uintptr_t process::get_module_base(int pid, const char* module_name)
{
	PEPROCESS proc = {};

	if (!NT_SUCCESS(PsLookupProcessByProcessId((HANDLE)pid, &proc)))
		return {};

	PEB* ppeb = (PEB*)PsGetProcessPeb(proc);

	if (!ppeb)
		return {};

	KAPC_STATE apc = { 0 };

	KeStackAttachProcess(proc, &apc);

	PEB_LDR_DATA* pldr = (PEB_LDR_DATA*)ppeb->Ldr;

	if (!pldr) {
		KeUnstackDetachProcess(&apc);

		return {};
	}

	ANSI_STRING cstr = { 0 };
	UNICODE_STRING name = { 0 };
	void* addr = nullptr;

	RtlInitAnsiString(&cstr, module_name);

	RtlAnsiStringToUnicodeString(&name, &cstr, TRUE);

	addr = MmGetSystemRoutineAddress(&name);


	for (LIST_ENTRY* list = (LIST_ENTRY*)pldr->InLoadOrderModuleList.Flink; list != &pldr->InLoadOrderModuleList; list = (LIST_ENTRY*)list->Flink)
	{
		PLDR_DATA_TABLE_ENTRY pentry = CONTAINING_RECORD(list, LDR_DATA_TABLE_ENTRY, InLoadOrderLinks);


		if (RtlCompareUnicodeString(&name, &pentry->BaseDllName, FALSE) == 0) {

			printf("Module Name: %wZ\n", pentry->BaseDllName);
			printf("Module Base: %p\n", pentry->DllBase);
			printf("Module Size: %d\n", pentry->SizeOfImage);

			uintptr_t module_base = (uintptr_t)pentry->DllBase;

			RtlFreeUnicodeString(&name);

			KeUnstackDetachProcess(&apc);
			return module_base;

		}

	}

	KeUnstackDetachProcess(&apc);
	RtlFreeUnicodeString(&name);

	printf("Module not found\n");

	return {};
}
