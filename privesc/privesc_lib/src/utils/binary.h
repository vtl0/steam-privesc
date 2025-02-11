#ifndef _BINARY_H
#define _BINARY_H

struct bin_section {
  void* base;
  size_t size;
};

int get_section(void* module_base,
                char *section_name,
                struct bin_section* sect);

#endif // _BINARY_H