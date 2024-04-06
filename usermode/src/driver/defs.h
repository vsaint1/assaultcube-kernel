#pragma once
#include <optional>
#include <string_view>
#include <vector>
#include <charconv>
#include <memory>
#include "../utils/macros.h"
#include <winternl.h>

#define FILE_DEVICE_UNKNOWN 0x00000022
#define METHOD_BUFFERED 0
#define FILE_ANY_ACCESS 0
#define FILE_SPECIAL_ACCESS (FILE_ANY_ACCESS)

#define CTL_CODE(DeviceType, Function, Method, Access) (((DeviceType) << 16) | ((Access) << 14) | ((Function) << 2) | (Method))

#define PROCESS_ID_REQUEST CTL_CODE(FILE_DEVICE_UNKNOWN, 0x555, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)

typedef struct _KPROCESSID_REQUEST {
	UNICODE_STRING process_name;
} KPROCESSID_REQUEST, * PKPROCESSID_REQUEST;

#define MOUSE_REQUEST CTL_CODE(FILE_DEVICE_UNKNOWN, 0x666, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)

typedef struct _KMOUSE_REQUEST {
	long x;
	long y;
	unsigned char button_flags;
} KMOUSE_REQUEST, * PKMOUSE_REQUEST;


#define MODULE_BASE_REQUEST CTL_CODE(FILE_DEVICE_UNKNOWN, 0x777, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)

typedef struct _KERNEL_MODULE_REQUEST {
	unsigned int pid;
	UNICODE_STRING module_name;
} KERNEL_MODULE_REQUEST, * PKERNEL_MODULE_REQUEST;

#define READ_REQUEST CTL_CODE(FILE_DEVICE_UNKNOWN, 0x888, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)

typedef struct _KERNEL_READ_REQUEST {
	int src_pid;
	PVOID src_address;
	PVOID p_buffer;
	SIZE_T size;

} KERNEL_READ_REQUEST, * PKERNEL_READ_REQUEST;