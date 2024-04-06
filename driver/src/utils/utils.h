#pragma once

#ifdef _DEBUG
#define printf(...) DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "[ACube] - " __VA_ARGS__)
#else
#define printf(...)
#endif
