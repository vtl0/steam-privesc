#include <stdbool.h>
#include <tchar.h>
#include <Windows.h>
#include <TlHelp32.h>

#include "process.h"

DWORD get_pid_from_name_tlhelp(TCHAR* name) {
  PROCESSENTRY32 pe32;
  HANDLE snapshot;
  DWORD pid;

  pid = 0;
  pe32.dwSize = sizeof(pe32);
  snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
  if (snapshot == INVALID_HANDLE_VALUE || !Process32First(snapshot, &pe32)) {
    return 0;
  }

  do {
    if (_tcsicmp(name, pe32.szExeFile) == 0) {
      pid = pe32.th32ProcessID;
      break;
    }
  } while (Process32Next(snapshot, &pe32));

  CloseHandle(snapshot);
  return pid;
}

bool inject_dll_loadlibrary(HANDLE handle, char* dll_path) {
  FARPROC load_library;
  HANDLE thread;
  LPVOID path;
  HMODULE kernel32;
  size_t str_len;

  str_len = strlen(dll_path) + 1;
  path = VirtualAllocEx(handle, NULL, str_len,
                                 MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

  if (NULL == path) {
    return 0;
  }

  if (!WriteProcessMemory(handle, path, dll_path, str_len, NULL)) {
    return 0;
  }

  kernel32 = GetModuleHandleA("kernel32.dll");
  if (kernel32 == NULL) {
    return 0;
  }

  load_library = GetProcAddress(kernel32, "LoadLibraryA");
  if (load_library == NULL) {
    return 0;
  }

  thread = CreateRemoteThread(
      handle, NULL, 0, (LPTHREAD_START_ROUTINE)load_library, path, 0, NULL);
  if (thread == NULL) {
    return 0;
  }

  Sleep(1000);
  VirtualFreeEx(handle, path, 0, MEM_RELEASE);
  CloseHandle(thread);
  return 1;
}