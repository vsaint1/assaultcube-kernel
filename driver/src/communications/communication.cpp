#include "communication.h"

 MOUSE_OBJECT mouse_obj{ 0 };

NTSTATUS com::on_request(PDEVICE_OBJECT device_object, PIRP irp) {
  UNREFERENCED_PARAMETER(device_object);

   if (!mouse_obj.mouse_device || !mouse_obj.service_callback)
  	mouse::init(&mouse_obj);


  PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(irp);

  switch (stack->Parameters.DeviceIoControl.IoControlCode) {
  case PROCESS_ID_REQUEST: {
    printf("PROCESS_ID_REQUEST\n");

    KPROCESSID_REQUEST *request = (KPROCESSID_REQUEST *)irp->AssociatedIrp.SystemBuffer;

    printf("request->process_name: %wZ\n", request->process_name);

    char module_str[16]{};

    crt::string_to_c_str(&request->process_name, module_str);

    if (crt::strcmp(module_str, "") == 0) {
      irp->IoStatus.Status = STATUS_INVALID_PARAMETER;
      irp->IoStatus.Information = 0;
      IoCompleteRequest(irp, IO_NO_INCREMENT);
      break;
    }

    const char *process_name = strcmp(module_str, "") == 0 ? "ac_client.exe" : module_str;

    int pid = process::get_process_id_by_name(IoGetCurrentProcess(), process_name);

    printf("pid %d\n", pid);
    memset(module_str, 0, sizeof(module_str));
    irp->IoStatus.Status = STATUS_SUCCESS;
    irp->IoStatus.Information = *(int *)&pid;
    pid = 0;
    IoCompleteRequest(irp, IO_NO_INCREMENT);

    break;
  }
  case MODULE_BASE_REQUEST: {
    printf("MODULE_BASE_REQUEST\n");

    KERNEL_MODULE_REQUEST *request = (KERNEL_MODULE_REQUEST *)irp->AssociatedIrp.SystemBuffer;

    printf("pid %d, module_name %wZ\n", request->pid, request->module_name);

    char module_str[16]{};

    crt::string_to_c_str(&request->module_name, module_str);

    if (crt::strcmp(module_str, "") == 0) {
      irp->IoStatus.Status = STATUS_INVALID_PARAMETER;
      irp->IoStatus.Information = 0;
      IoCompleteRequest(irp, IO_NO_INCREMENT);
      break;
    }

    const char *module_name = strcmp(module_str, "") == 0 ? "ac_client.exe" : module_str;

    printf("module %s\n", module_name);

    uintptr_t base = process::get_module_base(request->pid, module_name);

    memset(module_str, 0, sizeof(module_str));
    irp->IoStatus.Status = STATUS_SUCCESS;
    irp->IoStatus.Information = *(uintptr_t *)&base;
    IoCompleteRequest(irp, IO_NO_INCREMENT);
    break;
  }
  case READ_REQUEST: {
    printf("READ_REQUEST\n");

    KERNEL_READ_REQUEST *request = (KERNEL_READ_REQUEST *)irp->AssociatedIrp.SystemBuffer;

    printf("pid %d, address %p, size %d\n", request->src_pid, request->src_address, request->size);

    auto status = memory::read(request->src_pid, request->src_address, request->p_buffer, request->size);

    irp->IoStatus.Status = status;
    irp->IoStatus.Information = sizeof(PKERNEL_READ_REQUEST);
    IoCompleteRequest(irp, IO_NO_INCREMENT);
    break;
  }
  case WRITE_REQUEST: {
    printf("WRITE_REQUEST\n");

    KERNEL_WRITE_REQUEST *request = (KERNEL_WRITE_REQUEST *)irp->AssociatedIrp.SystemBuffer;

    printf("pid %d, address %p, size %d\n", request->src_pid, request->src_address, request->size);

    auto status = memory::write(request->src_pid, request->src_address, request->p_buffer, request->size);

    irp->IoStatus.Status = status;
    irp->IoStatus.Information = sizeof(PKERNEL_WRITE_REQUEST);
    IoCompleteRequest(irp, IO_NO_INCREMENT);
    break;
  }
  default:

    irp->IoStatus.Status = STATUS_INVALID_DEVICE_REQUEST;
    irp->IoStatus.Information = 0;
    IoCompleteRequest(irp, IO_NO_INCREMENT);
    break;
  }

  return STATUS_SUCCESS;
}

NTSTATUS com::on_create(PDEVICE_OBJECT device_object, PIRP irp) {
  UNREFERENCED_PARAMETER(device_object);

  irp->IoStatus.Status = STATUS_SUCCESS;
  irp->IoStatus.Information = 0;
  IoCompleteRequest(irp, IO_NO_INCREMENT);
  return irp->IoStatus.Status;
}

NTSTATUS com::on_close(PDEVICE_OBJECT device_object, PIRP irp) {
  UNREFERENCED_PARAMETER(device_object);

  irp->IoStatus.Status = STATUS_SUCCESS;
  irp->IoStatus.Information = 0;
  IoCompleteRequest(irp, IO_NO_INCREMENT);
  return irp->IoStatus.Status;
}

NTSTATUS com::unsupported_io(PDEVICE_OBJECT device_object, PIRP irp) {
  UNREFERENCED_PARAMETER(device_object);

  irp->IoStatus.Status = STATUS_NOT_IMPLEMENTED;
  irp->IoStatus.Information = 0;

  IoCompleteRequest(irp, IO_NO_INCREMENT);

  return irp->IoStatus.Status;
}
