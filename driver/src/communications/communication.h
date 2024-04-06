#pragma once

#include "../memory/memory.h"
#include "../memory/process.h"
#include "../memory/mouse.h"

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


#define WRITE_REQUEST CTL_CODE(FILE_DEVICE_UNKNOWN, 0x999, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)


typedef struct _KERNEL_WRITE_REQUEST {
  int src_pid;
  PVOID src_address;
  PVOID p_buffer;
  SIZE_T size;

} KERNEL_WRITE_REQUEST, *PKERNEL_WRITE_REQUEST;

namespace com {

	NTSTATUS on_request(PDEVICE_OBJECT device_object, PIRP irp);

	NTSTATUS on_create(PDEVICE_OBJECT device_object, PIRP irp);

	NTSTATUS on_close(PDEVICE_OBJECT device_object, PIRP irp);

	NTSTATUS unsupported_io(PDEVICE_OBJECT device_object, PIRP irp);
};