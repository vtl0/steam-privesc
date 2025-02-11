#include <stdint.h>
#include <string.h>
#include <Windows.h>

#include "binary.h"

int get_section(void* module_base,
                char* section_name,
                struct bin_section* sect) {
  PIMAGE_DOS_HEADER dos_header;
  PIMAGE_NT_HEADERS nt_header;
  PIMAGE_SECTION_HEADER first_section;

  if (sect == NULL)
    return -1;

  sect->base = NULL;
  sect->size = 0;

  dos_header = (PIMAGE_DOS_HEADER)module_base;
  // check if module_base points to a valid module base
  if (dos_header->e_magic != IMAGE_DOS_SIGNATURE)
    return -1;

  nt_header = (PIMAGE_NT_HEADERS)((uint8_t*)dos_header + dos_header->e_lfanew);
  if (nt_header->Signature != IMAGE_NT_SIGNATURE)
    return -1;

  first_section = IMAGE_FIRST_SECTION(nt_header);
  for (int i = 0; i < nt_header->FileHeader.NumberOfSections; i++) {
    PIMAGE_SECTION_HEADER current_section = &first_section[i];
    if (strcmp(section_name, (char*)current_section->Name) == 0) {
      sect->base = (uint8_t*)module_base + current_section->VirtualAddress;
      sect->size = current_section->Misc.VirtualSize;

      return 0;
    }
  }

  return -1;
}