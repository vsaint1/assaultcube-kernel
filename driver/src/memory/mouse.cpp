#include "mouse.h"


NTSTATUS mouse::init(MOUSE_OBJECT* mouse_obj)
{
	UNICODE_STRING mouse_class = RTL_CONSTANT_STRING(L"\\Driver\\MouClass");

	PDRIVER_OBJECT class_driver_object = NULL;
	NTSTATUS status = ObReferenceObjectByName(&mouse_class, OBJ_CASE_INSENSITIVE, NULL, 0, *IoDriverObjectType, KernelMode, NULL, (void**)&class_driver_object);
	
	if (!NT_SUCCESS(status)) { return status; }

	UNICODE_STRING mouhid = RTL_CONSTANT_STRING(L"\\Driver\\MouHID");

	PDRIVER_OBJECT hid_driver_object = NULL;
	status = ObReferenceObjectByName(&mouhid, OBJ_CASE_INSENSITIVE, NULL, 0, *IoDriverObjectType, KernelMode, NULL, (void**)&hid_driver_object);

	if (!NT_SUCCESS(status))
	{
		if (class_driver_object) { ObDereferenceObject(class_driver_object); }
		return status;
	}

	PVOID class_driver_base = NULL;

	PDEVICE_OBJECT hid_device_object = hid_driver_object->DeviceObject;
	while (hid_device_object && !mouse_obj->service_callback)
	{
		PDEVICE_OBJECT class_device_object = class_driver_object->DeviceObject;
		while (class_device_object && !mouse_obj->service_callback)
		{
			if (!class_device_object->NextDevice && !mouse_obj->mouse_device)
			{
				mouse_obj->mouse_device = class_device_object;
			}

			PULONG_PTR device_extension = (PULONG_PTR)hid_device_object->DeviceExtension;
			ULONG_PTR device_ext_size = ((ULONG_PTR)hid_device_object->DeviceObjectExtension - (ULONG_PTR)hid_device_object->DeviceExtension) / 4;
			class_driver_base = class_driver_object->DriverStart;
			for (ULONG_PTR i = 0; i < device_ext_size; i++)
			{
				if (device_extension[i] == (ULONG_PTR)class_device_object && device_extension[i + 1] > (ULONG_PTR)class_driver_object)
				{
					mouse_obj->service_callback = (MouseClassServiceCallback)(device_extension[i + 1]);
					break;
				}
			}
			class_device_object = class_device_object->NextDevice;
		}
		hid_device_object = hid_device_object->AttachedDevice;
	}

	if (!mouse_obj->mouse_device)
	{
		PDEVICE_OBJECT target_device_object = class_driver_object->DeviceObject;
		while (target_device_object)
		{
			if (!target_device_object->NextDevice)
			{
				mouse_obj->mouse_device = target_device_object;
				break;
			}
			target_device_object = target_device_object->NextDevice;
		}
	}

	ObDereferenceObject(class_driver_object);
	ObDereferenceObject(hid_driver_object);

	printf("MouseClazz initialized\n");

	return STATUS_SUCCESS;
}

void mouse::move(MOUSE_OBJECT mouse_obj, int x, int y, char button_flag)
{

	ULONG input_data;
	KIRQL irql{ 0 };
	MOUSE_INPUT_DATA mid = { 0 };

	mid.LastX = x;
	mid.LastY = y;
	mid.ButtonFlags = button_flag;

	KeRaiseIrql(DISPATCH_LEVEL, &irql);
	mouse_obj.service_callback(mouse_obj.mouse_device, &mid, (PMOUSE_INPUT_DATA)&mid + 1, &input_data);
	KeLowerIrql(irql);
}
