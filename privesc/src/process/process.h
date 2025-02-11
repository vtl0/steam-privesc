#ifndef _PROCESS_H
#define _PROCESS_H

#include <stdbool.h>
#include <stdint.h>
#include <Windows.h>

DWORD get_pid_from_name_tlhelp(TCHAR* name);
bool inject_dll_loadlibrary(HANDLE handle, char* dll_path);

#endif  // _PROCESS_H