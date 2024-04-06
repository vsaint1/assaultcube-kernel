#pragma once
#include "../utils/defs.h"


namespace memory {
	NTSTATUS write(int pid,void* address, void* target, size_t size);

	NTSTATUS write_safe(int pid,void* address, void* target, size_t size);

	NTSTATUS read(int pid, void* address, void* target,size_t size);
}