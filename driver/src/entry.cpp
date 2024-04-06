#include "communications/communication.h"
#include "memory/mouse.h"

UNICODE_STRING device_name = RTL_CONSTANT_STRING(L"\\Device\\acube");
UNICODE_STRING device_link = RTL_CONSTANT_STRING(L"\\DosDevices\\acube");
PDEVICE_OBJECT device_object{ 0 };

PDEVICE_OBJECT kbd_device{ 0 };

NTSTATUS driver_unload(PDRIVER_OBJECT driver_object) {

	printf("unloaded successfully\n");
	//IoDetachDevice(kbd_device);
	IoDeleteDevice(driver_object->DeviceObject);
	IoDeleteSymbolicLink(&device_link);

	return STATUS_SUCCESS;
}

extern "C" NTSTATUS DriverEntry(PDRIVER_OBJECT driver_object, PUNICODE_STRING registry_path) {
	UNREFERENCED_PARAMETER(registry_path);

	driver_object->DriverUnload = *(DRIVER_UNLOAD*)driver_unload;

	printf("driver loaded successfully\n");

	UNICODE_STRING kbd_class = RTL_CONSTANT_STRING(L"\\Device\\KeyboardClass0");

	if (DRIVER_RUNNING("FACEIT.sys") || DRIVER_RUNNING("EMAC-Driver-x64.sys") || DRIVER_RUNNING("bedaisy.sys") || DRIVER_RUNNING("vgk.sys") || DRIVER_RUNNING("EasyAntiCheat_EOS.sys")) {
		printf("Aborting Anti-cheat driver was found running\n");
		return STATUS_UNSUCCESSFUL;
	}


	IoCreateDevice(driver_object, 0, &device_name, FILE_DEVICE_UNKNOWN, FILE_DEVICE_SECURE_OPEN, FALSE, &device_object);
	IoCreateSymbolicLink(&device_link, &device_name);


	driver_object->MajorFunction[IRP_MJ_DEVICE_CONTROL] = com::on_request;
	driver_object->MajorFunction[IRP_MJ_CREATE] = com::on_create;
	driver_object->MajorFunction[IRP_MJ_CLOSE] = com::on_close;

	device_object->Flags |= DO_DIRECT_IO;
	device_object->Flags &= ~DO_DEVICE_INITIALIZING;


	/*NTSTATUS status = IoAttachDevice(device_object, &kbd_class, &kbd_device);
	printf("Attached to kbdclass.sys\n");
	if (!NT_SUCCESS(status)) {
		IoDeleteDevice(device_object);
		IoDeleteSymbolicLink(&device_link);
		return status;
	}*/



	return STATUS_SUCCESS;
}
