#include <stdint.h>
#include <Windows.h>

#include "patches.h"

int patch_near_call(BYTE* address, BYTE* dest, BYTE** original) {
  MEMORY_BASIC_INFORMATION mbi;
  uint32_t rel;
  DWORD old_prot = -1;

  if (!VirtualQuery(address, &mbi, sizeof(mbi)))
    return -1;

  // not code
  if (!(mbi.Protect & PAGE_EXECUTE_READ || mbi.Protect & PAGE_EXECUTE))
    return -1;

  if (!(mbi.Protect & PAGE_EXECUTE_READWRITE)) {
    if (VirtualProtect(address, 5, PAGE_EXECUTE_READWRITE, &old_prot) == FALSE)
      return -1;    
  }

  // not an rel call, so restore old protection and quit
  if (*address != 0xE8) {
    if (old_prot != -1)
      VirtualProtect(address, 5, old_prot, &old_prot);
    return -1;
  }
  // not true on steam 32 bit client (but we never know the future)
  if (sizeof(void*) == 8) {
    uintptr_t diff = (uintptr_t)dest - ((uintptr_t)address + 5);
    // not possible
    if (diff & 0xFFFFFFFF00000000) {
      VirtualProtect(address, 5, old_prot, &old_prot);
      return -1;
    }
  }
  rel = (uint32_t)dest - ((uint32_t)address + 5);
  *original = (void*)((uintptr_t)address + 5 + *(uint32_t*)&address[1]);
  *(uint32_t*)&address[1] = rel;

  VirtualProtect(address, 5, old_prot, &old_prot);
  return 0;
}